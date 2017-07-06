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

#ifndef FUERTE_NODE_CONNECTION_BUILDER_H
#define FUERTE_NODE_CONNECTION_BUILDER_H

#include <iostream>
#include "node_upstream.h"
#include "object_wrap.h"

namespace arangodb { namespace fuerte { namespace js {

// NConnectionBuilder is a node wrapper around the fuerte ConnectionBuilder class.
class NConnectionBuilder : public ObjectWrap<NConnectionBuilder, fu::ConnectionBuilder, std::unique_ptr<fu::ConnectionBuilder>> {
  friend class NConnection;
  NConnectionBuilder(): ObjectWrap() {}

public:
  static NAN_MODULE_INIT(Init) {
    auto tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("ConnectionBuilder").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1); //should be equal to the number of data members
    
    Nan::SetPrototypeMethod(tpl, "nativeConnect", NConnectionBuilder::nativeConnect);
  
    auto itpl = tpl->InstanceTemplate();
    Nan::SetAccessor(itpl, toString("nativeHost"), NConnectionBuilder::getHost, NConnectionBuilder::setHost);
    Nan::SetAccessor(itpl, toString("userName"), NConnectionBuilder::getUserName, NConnectionBuilder::setUserName);
    Nan::SetAccessor(itpl, toString("password"), NConnectionBuilder::getPassword, NConnectionBuilder::setPassword);

    initClass("ConnectionBuilder", target, tpl);
  }

  // Node constructor
  static NAN_METHOD(New);
  // Open connection to server.
  static NAN_METHOD(nativeConnect);
  // Get server URL.
  static NAN_GETTER(getHost);
  // Set server URL.
  static NAN_SETTER(setHost);
  // Get authentication username 
  static NAN_GETTER(getUserName);
  // Set authentication username 
  static NAN_SETTER(setUserName);
  // Get authentication password
  static NAN_GETTER(getPassword);
  // Set authentication password
  static NAN_SETTER(setPassword);
};

}}}
#endif

