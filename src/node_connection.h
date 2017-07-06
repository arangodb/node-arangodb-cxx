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

#ifndef FUERTE_NODE_CONNECTION_H
#define FUERTE_NODE_CONNECTION_H

#include <iostream>
#include "node_upstream.h"
#include "object_wrap.h"

namespace arangodb { namespace fuerte { namespace js {

// NConnection is a node wrapper around the fuerte Connection class.
class NConnection : public ObjectWrap<NConnection, fu::Connection, std::shared_ptr<fu::Connection>> {
  friend class PendingRequest;
public:
  friend class NConnectionBuilder;
  NConnection(): ObjectWrap(nullptr) {}
  NConnection(std::shared_ptr<::fu::Connection> conn): ObjectWrap(std::move(conn)) {}

  static NAN_MODULE_INIT(Init) {
    auto tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("Connection").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tpl, "nativeSendRequest", NConnection::sendRequest);

    auto itpl = tpl->InstanceTemplate();
    Nan::SetAccessor(itpl, toString("requestsLeft"), NConnection::getRequestsLeft);

    initClass("Connection", target, tpl);
  }

  // Node constructor
  static NAN_METHOD(New);
  // requestsLeft returns the number of unfinished requests
  static NAN_GETTER(getRequestsLeft);
  // sendRequest starts sending a request
  static NAN_METHOD(sendRequest);
};

}}}
#endif

