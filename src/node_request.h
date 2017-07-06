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

#ifndef FUERTE_NODE_REQUEST_H
#define FUERTE_NODE_REQUEST_H

#include "node_upstream.h"
#include "object_wrap.h"

namespace arangodb { namespace fuerte { namespace js {

// NRequest is a Node wrapper around the fuerte Request class.
class NRequest : public ObjectWrap<NRequest, fu::Request, std::unique_ptr<fu::Request>> {
    friend class PendingRequest;
    NRequest(): ObjectWrap() {}
    NRequest(std::unique_ptr<fu::Request> x): ObjectWrap(std::move(x)) {}

public:
  // Initialize the node module with all Request methods.
  static NAN_MODULE_INIT(Init) {
    auto tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("Request").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1); // should be equal to the number of data members

    Nan::SetPrototypeMethod(tpl, "addBody", NRequest::addBody);
    Nan::SetPrototypeMethod(tpl, "addSlice", NRequest::addSlice);
    Nan::SetPrototypeMethod(tpl, "addBinary", NRequest::addBinary);
    Nan::SetPrototypeMethod(tpl, "addQueryParameter", NRequest::addQueryParameter);
    Nan::SetPrototypeMethod(tpl, "addHeader", NRequest::addHeader);

    auto itpl = tpl->InstanceTemplate();
    Nan::SetAccessor(itpl, toString("path"), NRequest::getPath, NRequest::setPath);
    Nan::SetAccessor(itpl, toString("database"), NRequest::getDatabase, NRequest::setDatabase);
    Nan::SetAccessor(itpl, toString("method"), NRequest::getMethod, NRequest::setMethod);
    Nan::SetAccessor(itpl, toString("contentType"), NRequest::getContentType, NRequest::setContentType);
    Nan::SetAccessor(itpl, toString("acceptType"), NRequest::getAcceptType, NRequest::setAcceptType);

    initClass("Request", target, tpl);
  }

  // Node Request constructor 
  static NAN_METHOD(New);
  // Add a buffer or object payload.
  static NAN_METHOD(addBody);
  // Add a velocypack Slice payload (in node Buffer).
  static NAN_METHOD(addSlice);
  // Add a binary payload (in a node Buffer).
  static NAN_METHOD(addBinary);
  
  // Get the local path of the request 
  static NAN_GETTER(getPath);
  // Set the local path of the request 
  static NAN_SETTER(setPath);
  // Get the database of the request (defaults to '_system')
  static NAN_GETTER(getDatabase);
  // Set the database of the request (defaults to '_system')
  static NAN_SETTER(setDatabase);
  // Get the method of the request (get|put|post|patch|delete)
  static NAN_GETTER(getMethod);
  // Set the method of the request (get|put|post|patch|delete)
  static NAN_SETTER(setMethod);
  // Get the Content-Type of the request's content
  static NAN_GETTER(getContentType);
  // Set the Content-Type of the request's content
  static NAN_SETTER(setContentType);
  // Get the Accept-type of the request (which content type to accept in response)
  static NAN_GETTER(getAcceptType);
  // Set the Accept-type of the request (which content type to accept in response)
  static NAN_SETTER(setAcceptType);

  // Add a query parameter to the request
  static NAN_METHOD(addQueryParameter);
  // Add a header key/value pair to the request
  static NAN_METHOD(addHeader);
};

}}}
#endif
