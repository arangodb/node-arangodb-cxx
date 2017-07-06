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

#include "node_connection_builder.h"
#include "node_connection.h"
#include "node_request.h"
#include <iostream>
#include <memory>
#include <mutex>
#include <atomic>
#include <fuerte/FuerteLogger.h>
#include <fuerte/helper.h>

namespace arangodb { namespace fuerte { namespace js {

///////////////////////////////////////////////////////////////////////////////
// NConnectionBuilder /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NAN_METHOD(NConnectionBuilder::New) {
  try {
    if (info.IsConstructCall()) {
      auto obj = new NConnectionBuilder();
      obj->Wrap(info.This());
      info.GetReturnValue().Set(info.This());
    } else {
      info.GetReturnValue().Set(NConnectionBuilder::NewInstance().ToLocalChecked());
    }
 } catch(std::exception const& e){
   Nan::ThrowError("ConnectionBuilder.New binding failed with exception");
 }
}

NAN_METHOD(NConnectionBuilder::nativeConnect) {
  try {
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {info.This()};
    v8::Local<v8::Object> connInstance;
    bool ok = NConnection::NewInstance(argc, argv).ToLocal(&connInstance);
    if(!ok){
      Nan::ThrowError("ConnectionBuilder.connect binding failed with exception"
                      " - please check connection parameters and server");
      return;
    }
    info.GetReturnValue().Set(connInstance);
  } catch(std::exception const& e) {
    std::cerr << "## DRIVER LEVEL EXCEPTION - START ##" << std::endl;
    std::cerr << e.what() << std::endl;
    std::cerr << "## DRIVER LEVEL EXCEPTION - END   ##" << std::endl;
    Nan::ThrowError("ConnectionBuilder.connect binding failed with exception"
                    " - Make sure the server is up and running");
  }
}

NAN_GETTER(NConnectionBuilder::getHost) {
  try {
    auto result = toString(self(info)->host());
    info.GetReturnValue().Set(result);
  } catch(std::exception const& e) {
    std::string msg = std::string("ConnectionBuilder.getHost binding failed with exception: ") + e.what();
    Nan::ThrowError(msg.c_str());
  }
}

NAN_SETTER(NConnectionBuilder::setHost) {
  try {
    self(info)->host(to<std::string>(value));
  } catch(std::exception const& e) {
    std::string msg = std::string("ConnectionBuilder.setHost binding failed with exception: ") + e.what();
    Nan::ThrowError(msg.c_str());
  }
}

NAN_GETTER(NConnectionBuilder::getUserName) {
  try {
    auto result = toString(self(info)->user());
    info.GetReturnValue().Set(result);
  } catch(std::exception const& e) {
    std::string msg = std::string("ConnectionBuilder.getUserName binding failed with exception: ") + e.what();
    Nan::ThrowError(msg.c_str());
  }
}

NAN_SETTER(NConnectionBuilder::setUserName) {
  try {
    auto user = to<std::string>(value);
    auto cb = self(info);
    cb->user(user);
    if ((!user.empty()) && (cb->authenticationType() == AuthenticationType::None)) {
      cb->authenticationType(AuthenticationType::Basic);
    }
  } catch(std::exception const& e) {
    std::string msg = std::string("ConnectionBuilder.setUserName binding failed with exception: ") + e.what();
    Nan::ThrowError(msg.c_str());
  }
}

NAN_GETTER(NConnectionBuilder::getPassword) {
  try {
    auto result = toString(self(info)->password());
    info.GetReturnValue().Set(result);
  } catch(std::exception const& e) {
    std::string msg = std::string("ConnectionBuilder.getPassword binding failed with exception: ") + e.what();
    Nan::ThrowError(msg.c_str());
  }
}

NAN_SETTER(NConnectionBuilder::setPassword) {
  try {
    auto password = to<std::string>(value);
    auto cb = self(info);
    cb->password(password);
    if ((!password.empty()) && (cb->authenticationType() == AuthenticationType::None)) {
      cb->authenticationType(AuthenticationType::Basic);
    }
  } catch(std::exception const& e) {
    Nan::ThrowError("ConnectionBuilder.setPassword binding failed with exception");
  }
}

}}}
