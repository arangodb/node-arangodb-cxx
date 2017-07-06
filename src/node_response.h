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
#pragma once

#ifndef FUERTE_NODE_RESPONSE_H
#define FUERTE_NODE_RESPONSE_H

#include "node_upstream.h"
#include "object_wrap.h"

namespace arangodb { namespace fuerte { namespace js {

// NResponse is a node wrapper around the fuerte Response class.
class NResponse : public ObjectWrap<NResponse, fu::Response, std::unique_ptr<fu::Response>> {
    friend class PendingRequest;
    NResponse(): ObjectWrap() {}
    NResponse(std::unique_ptr<fu::Response> x): ObjectWrap(std::move(x)) {}

public:
  static NAN_MODULE_INIT(Init) {
    auto tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("Response").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1); //should be equal to the number of data members

    auto itpl = tpl->InstanceTemplate();
    Nan::SetAccessor(itpl, toString("contentType"), NResponse::getContentType);
    Nan::SetAccessor(itpl, toString("responseCode"), NResponse::getResponseCode);
    Nan::SetAccessor(itpl, toString("header"), NResponse::getHeader);
    Nan::SetAccessor(itpl, toString("body"), NResponse::getBody);
    Nan::SetAccessor(itpl, toString("slices"), NResponse::getSlices);
    Nan::SetAccessor(itpl, toString("payload"), NResponse::getPayload);

    initClass("Response", target, tpl);
  }

  // Node constructor
  static NAN_METHOD(New);
  // contentType returns the Content-Type of this response
  static NAN_GETTER(getContentType);
  // responseCode returns the (HTTP) response code from the request
  static NAN_GETTER(getResponseCode);
  // header returns header meta data of this response
  static v8::Local<v8::Object> buildV8Header(const Nan::PropertyCallbackInfo<v8::Value>& info);
  static NAN_GETTER(getHeader);
  // Return the entire response payload as a decoded V8 object/array/value.
  static v8::Local<v8::Value> buildV8Body(const Nan::PropertyCallbackInfo<v8::Value>& info);
  static NAN_GETTER(getBody);
  // Return the entire response payload in a buffer.
  static v8::Local<v8::Value> buildV8Payload(const Nan::PropertyCallbackInfo<v8::Value>& info);
  static NAN_GETTER(getPayload);
  // Return an array containing all slices in the response payload, each as buffer.
  // If the response does not have a velocypack content type, undefined
  // is returned.
  static v8::Local<v8::Value> buildV8Slices(const Nan::PropertyCallbackInfo<v8::Value>& info);
  static NAN_GETTER(getSlices);
};

}}}
#endif
