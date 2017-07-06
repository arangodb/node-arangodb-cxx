////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2017 ArangoDB GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Christoph Uhde
/// @author Ewout Prangsma
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <string>
#include <stdlib.h>

#include <fuerte/FuerteLogger.h>
#include <fuerte/helper.h>

#include "node_connection.h"
#include "node_connection_builder.h"
#include "node_request.h"
#include "node_response.h"

namespace arangodb { namespace fuerte { namespace js {

static unsigned int getThreadCount() {
  unsigned int count = std::thread::hardware_concurrency();
  auto envVar = getenv("FUERTE_THREAD_COUNT");
  if (envVar) {
    try {
      auto i = std::stoi(envVar);
      if (i >= 1) {
        count = (unsigned int)i;
      }
    } catch (...) {
      throw std::runtime_error("Invalid value in FUERTE_THREAD_COUNT");
    }
  }
  return count;
}

static EventLoopService eventLoopService_(getThreadCount());

///////////////////////////////////////////////////////////////////////////////
// NConnection ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NAN_METHOD(NConnection::New) {
  try {
    if (info.IsConstructCall()) {
      auto obj = new NConnection();
      if (info[0]->IsObject()) { // NConnectionBuilderObject -- exact type check?
        auto conn = unwrap<NConnectionBuilder>(info[0])->cppClass()->connect(eventLoopService_);
        if (conn == nullptr) {
          Nan::ThrowError("Connection.New binding failed with exception - check connection string");
          return;
        }
        obj->setCppClass(conn);
      }
      obj->Wrap(info.This());
      info.GetReturnValue().Set(info.This());
    } else {
      info.GetReturnValue().Set(NConnection::NewInstance().ToLocalChecked());
    }
  } catch(std::exception const& e){
    Nan::ThrowError("Connection.New binding failed with exception");
  }
}

NAN_GETTER(NConnection::getRequestsLeft) {
  try {
    auto result = self(info)->requestsLeft();
    auto rv = Nan::New<v8::Uint32>(static_cast<std::uint32_t>(result));
    info.GetReturnValue().Set(rv);
  } catch(std::exception const& e){
    Nan::ThrowError("Connection.reqestsLeft binding failed with exception");
  }
}

///////////////////////////////////////////////////////////////////////////////
// SendRequest ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const std::string callbackErrorMessage(
  "##################################################\n"
  "#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#\n"
  "#@Failed to create request instance in callback.@#\n"
  "#@This could be an error in you JavaScript Code!@#\n"
  "#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#\n"
  "##################################################\n"
);

class PendingRequest {
 public:
  PendingRequest(v8::Local<v8::Value> const& request, v8::Local<v8::Value> const& callback) :
    jsRequest(v8::Local<v8::Object>::Cast(request)),
    jsCallback(v8::Local<v8::Function>::Cast(callback)) {
    NRequest::CheckedUnwrap(Nan::New(jsRequest));
  }

  // Initialize the async handle (cannot be called in ctor due to shared_from_this)
  void InitAsync() {
    uv_async_init(uv_default_loop(), &async_handle, uvCallbackStatic);
    async_handle.data = this;
  }

  // Start sending the request
  void Start(NConnection* conn) {
    // Clone the request so we keep the one in the JS object alive.
    v8::Local<v8::Object> locJsReq = New(jsRequest);
    auto jsReq = Nan::ObjectWrap::Unwrap<NRequest>(locJsReq);
    auto req = std::unique_ptr<fu::Request>(new fu::Request(*(jsReq->cppClass()))); 
    conn->cppClass()->sendRequest(std::move(req), [this](unsigned err, std::unique_ptr<fu::Request> creq, std::unique_ptr<fu::Response> cres){
      cppCallback(err, std::move(creq), std::move(cres));
    });
  }

 private:
  // cppCallback is called on any of the fuerte EventLoopService threads.
  void cppCallback(unsigned err, std::unique_ptr<fu::Request> creq, std::unique_ptr<fu::Response> cres) {
    // Save data 
    this->error = err; 
    this->cppResponse = std::move(cres);
    // Trigger callback on main event loop
    uv_async_send(&async_handle);
  }

  // Static UV callback.
  static NAUV_WORK_CB(uvCallbackStatic) {
    // Invoke actual callback handler (on event loop)
    auto penReq = static_cast<PendingRequest*>(async->data);
    penReq->uvCallback();
    // Close handle
    uv_close((uv_handle_t*)async, uvCleanup);
  }

  // Static UV cleanup callback.
  static void uvCleanup(uv_handle_t *handle) {
    auto penReq = static_cast<PendingRequest*>(handle->data);
    delete penReq;
  }

  // uvCallback is called on the main event loop.
  void uvCallback() {
    Nan::HandleScope scope;

    // wrap response
    v8::Local<v8::Value> response;
    if (cppResponse) {
      // Create Response object
      auto resObj = NResponse::NewInstance().ToLocalChecked();
      unwrap<NResponse>(resObj)->setCppClass(std::move(cppResponse));
      // Store request in response 
      resObj->Set(Nan::New("request").ToLocalChecked(), Nan::New(jsRequest));
      response = resObj;
    } else {
      response = Nan::Undefined();
    }

    // Call callback
    const unsigned argc = 2;
    v8::Local<v8::Value> argv[argc] = { Nan::New<v8::Integer>(error), response };
    // call
    jsCallback.Call(argc, argv);
  }

  // members
  Nan::Persistent<v8::Object> jsRequest;
  Nan::Callback jsCallback;
  uv_async_t async_handle;
  unsigned error;
  std::unique_ptr<fu::Response> cppResponse;
};

NAN_METHOD(NConnection::sendRequest) {
  try {
    // Check arguments
    if (info.Length() != 2 ) {
      Nan::ThrowTypeError("Not 2 Arguments");
    }
    if (!info[0]->IsObject()){
      Nan::ThrowTypeError("Request is not an Object");
    }
    if (!info[1]->IsFunction()){
      Nan::ThrowTypeError("Callback is not a Function");
    }

    // Create PendingRequest 
    auto penReq = new PendingRequest(info[0], info[1]);
    penReq->InitAsync();
    // Start request
    auto conn = NConnection::CheckedUnwrap(info.Holder());
    penReq->Start(conn);
  } catch(std::exception const& e){
    Nan::ThrowError("Connection.sendRequest binding failed with exception");
    return;
  }
  FUERTE_LOG_NODE << "exit on fuerte-node success callback" << std::endl;
}

}}}
