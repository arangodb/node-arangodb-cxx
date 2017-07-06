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

#include "node_request.h"
#include "node_vpack.h"

namespace arangodb { namespace fuerte { namespace js {

// NRequest
NAN_METHOD(NRequest::New) {
  if (info.IsConstructCall()) {
    auto obj = new NRequest();
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    info.GetReturnValue().Set(NRequest::NewInstance().ToLocalChecked());
 }
}

NAN_METHOD(NRequest::addBody) { // Buffer | Object
  try {
    if (info.Length() != 1) {
      Nan::ThrowTypeError("Wrong number of Arguments");
      return;
    }
    if (::node::Buffer::HasInstance(info[0])) {
      // Got Node::Buffer 
      auto data = reinterpret_cast<uint8_t*>(::node::Buffer::Data(info[0]));
      auto length = ::node::Buffer::Length(info[0]);
      auto slice = fu::VSlice(data);
      // Check slice length 
      auto sliceLength = slice.byteSize();
      if (sliceLength > length) {
        Nan::ThrowError("Request.addBody: buffer does not contain an entire slice");
        return;
      }
      // Add slice 
      self(info)->addVPack(slice);
      info.GetReturnValue().Set(info.This());
    } else {
      // Got any other V8 value
      VPackBuilder builder;
      auto tri = TRI_V8ToVPack(info.GetIsolate(), builder, info[0], false);
      if (tri != TRI_ERROR_NO_ERROR) {
        std::string errorMessage = std::string("Request.addBody: Error while encoding: TRI_ERROR(") + std::to_string(tri) + ")";
        Nan::ThrowError(errorMessage.c_str());
        return;
      }

      self(info)->addVPack(builder.slice());
      info.GetReturnValue().Set(info.This());
    }
  } catch(std::exception const& e) {
    Nan::ThrowError("Request.addVPack binding failed with exception");
  }
}

NAN_METHOD(NRequest::addSlice) { 
  try {
    // Check arguments
    if (info.Length() != 1) {
      Nan::ThrowTypeError("Wrong number of Arguments");
      return;
    }
    if (!::node::Buffer::HasInstance(info[0])) {
      Nan::ThrowTypeError("Expected Buffer argument");
      return;      
    }
    // Get buffer, create slice
    auto data = reinterpret_cast<uint8_t*>(::node::Buffer::Data(info[0])); 
    auto length = ::node::Buffer::Length(info[0]);
    auto slice = fu::VSlice(data);
    // Check slice length 
    auto sliceLength = slice.byteSize();
    if (sliceLength > length) {
      Nan::ThrowError("Request.addSlice: buffer does not contain an entire slice");
      return;
    }
    // Add slice 
    self(info)->addVPack(slice);
    info.GetReturnValue().Set(info.This());
  } catch(std::exception const& e) {
    Nan::ThrowError("Request.addSlice binding failed with exception");
  }
}

NAN_METHOD(NRequest::addBinary) {
  try {
    if (info.Length() != 1 ) {
      Nan::ThrowTypeError("Wrong number of Arguments");
      return;
    }
    if (!::node::Buffer::HasInstance(info[0])) {
      Nan::ThrowTypeError("Expected Buffer argument");
      return;      
    }
    auto data = reinterpret_cast<uint8_t*>(::node::Buffer::Data(info[0])); /// aaaaijajaiai
    auto length = ::node::Buffer::Length(info[0]);

    self(info)->addBinary(data, length);
    info.GetReturnValue().Set(info.This());
  } catch(std::exception const& e) {
    Nan::ThrowError("Request.addBinary binding failed with exception");
  }
}

NAN_SETTER(NRequest::setPath) {
  try {
    self(info)->header.path = to<std::string>(value);
  } catch(std::exception const& e) {
    Nan::ThrowError("Request.setPath binding failed with exception");
  }
}

NAN_GETTER(NRequest::getPath) {
  try {
    auto& header = self(info)->header;
    if (header.path) {
      auto path = Nan::New(self(info)->header.path.get());
      info.GetReturnValue().Set(path.ToLocalChecked());
    } else {
      info.GetReturnValue().Set(Nan::Undefined());
    }
  } catch(std::exception const& e) {
    Nan::ThrowError("Request.getPath binding failed with exception");
  }
}

NAN_SETTER(NRequest::setDatabase) {
  try {
    self(info)->header.database = to<std::string>(value);
  } catch(std::exception const& e) {
    Nan::ThrowError("Request.setDatabase binding failed with exception");
  }
}

NAN_GETTER(NRequest::getDatabase) {
  try {
    auto& header = self(info)->header;
    if (header.database) {
      auto database = toString(self(info)->header.database.get());
      info.GetReturnValue().Set(database);
    } else {
      info.GetReturnValue().Set(Nan::Undefined());
    }
  } catch(std::exception const& e) {
    Nan::ThrowError("Request.getDatabase binding failed with exception");
  }
}

NAN_SETTER(NRequest::setMethod) {
  try {
    auto method = to<std::string>(value);
    auto verb = fu::to_RestVerb(method);
    if (verb == fu::RestVerb::Illegal) {
      Nan::ThrowTypeError("invalid rest parameter get/put/post/patch/delete are supported");
      return;
    }

    self(info)->header.restVerb = verb;
  } catch(std::exception const& e) {
    Nan::ThrowError("Request.setRestVerb binding failed with exception");
  }
}

NAN_GETTER(NRequest::getMethod) {
  try {
    auto& header = self(info)->header;
    if (header.restVerb) {
      auto verb = toString(fu::to_string(self(info)->header.restVerb.get()));
      info.GetReturnValue().Set(verb);
    } else {
      info.GetReturnValue().Set(Nan::Undefined());
    }
  } catch(std::exception const& e) {
    Nan::ThrowError("Request.getMethod binding failed with exception");
  }
}

NAN_SETTER(NRequest::setContentType) {
  try {
    self(info)->contentType(to<std::string>(value));
  } catch(std::exception const& e) {
    Nan::ThrowError("Request.setContentType binding failed with exception");
  }
}

NAN_GETTER(NRequest::getContentType) {
  try {
    auto& header = self(info)->header;
    info.GetReturnValue().Set(toString(header.contentTypeString()));
  } catch(std::exception const& e) {
    Nan::ThrowError("Request.getContentType binding failed with exception");
  }
}

NAN_SETTER(NRequest::setAcceptType) {
  try {
    self(info)->acceptType(to<std::string>(value));
  } catch(std::exception const& e) {
    Nan::ThrowError("Request.setAcceptType binding failed with exception");
  }
}

NAN_GETTER(NRequest::getAcceptType) {
  try {
    auto& header = self(info)->header;
    info.GetReturnValue().Set(toString(header.acceptTypeString()));
  } catch(std::exception const& e) {
    Nan::ThrowError("Request.getAcceptType binding failed with exception");
  }
}

NAN_METHOD(NRequest::addQueryParameter) {
  try {
    if (info.Length() != 2 ) {
      Nan::ThrowTypeError("Wrong number of Arguments");
      return;
    }
    auto key = to<std::string>(info[0]);
    auto value = to<std::string>(info[1]);
    self(info)->header.addParameter(key, value);
    info.GetReturnValue().Set(info.This());
  } catch(std::exception const& e) {
    Nan::ThrowError("Request.addQueryParameter binding failed with exception");
  }
}

NAN_METHOD(NRequest::addHeader) {
  try {
    if (info.Length() != 2 ) {
      Nan::ThrowTypeError("Wrong number of Arguments");
      return;
    }
    auto key = to<std::string>(info[0]);
    auto value = to<std::string>(info[1]);
    self(info)->header.addMeta(key, value);
    info.GetReturnValue().Set(info.This());
  } catch(std::exception const& e) {
    Nan::ThrowError("Request.addHeader binding failed with exception");
  }
}

}}}