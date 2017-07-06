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

#include "node_response.h"
#include "node_vpack.h"

namespace arangodb { namespace fuerte { namespace js {

// NResponse
const char* response_is_null("C++ Response is nullptr - maybe you did not receive a response - please check the error code!");

NAN_METHOD(NResponse::New) {
  if (info.IsConstructCall()) {
    auto obj = new NResponse();
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    info.GetReturnValue().Set(NResponse::NewInstance().ToLocalChecked());
 }
}

// Return the entire response payload as a decoded V8 object/array/value.
NAN_GETTER(NResponse::getBody) {
  try {
    auto key = toString("__body");
    if (Nan::Has(info.This(), key).FromJust()) {
      auto result = Nan::Get(info.This(), key);
      info.GetReturnValue().Set(result.ToLocalChecked());
    } else {
      auto result = buildV8Body(info);
      Nan::Set(info.This(), key, result);
      info.GetReturnValue().Set(result);
    }
  } catch (std::exception const& e) {
    Nan::ThrowError("Reponse.body binding failed with exception");
  }
}

v8::Local<v8::Value> NResponse::buildV8Body(const Nan::PropertyCallbackInfo<v8::Value>& info) {
  auto res = self(info);
  if (res) {
    // Check content type 
    if (res->isContentTypeVPack()) {
      auto& slices = res->slices();
      if (slices.size() == 0) {
        // Empty response
        return Nan::Undefined();
      } else if (slices.size() == 1) {
        // Single response 
        auto isolate = info.GetIsolate();
        auto options = &::arangodb::velocypack::Options::Defaults;
        auto value = TRI_VPackToV8(isolate, slices[0], options);
        return value;
      } else {
        // Multiple responses
        v8::Local<v8::Array> array;
        std::size_t index = 0;
        auto isolate = info.GetIsolate();
        auto options = &::arangodb::velocypack::Options::Defaults;
        for (auto const& slice : slices) {
          auto value = TRI_VPackToV8(isolate, slice, options);
          array->Set(index++, value);
        }
        return array;
      }
    } else {
      auto payload = res->payloadAsString();
      auto payloadAsString = Nan::New(payload).ToLocalChecked();
      if (res->isContentTypeJSON()) {
        // Parse json
        Nan::JSON NanJSON;
        auto result = NanJSON.Parse(payloadAsString);
        if (!result.IsEmpty()) {
          return result.ToLocalChecked();
        } else {
          throw std::runtime_error("Response.body failed to parse json payload");
        }
      } else if (res->isContentTypeText()) {
        // Plain text content
        return payloadAsString;
      } else {
        // Unknown content type
        auto msg = "Response.body unknown content type: " + res->contentTypeString();
        throw std::runtime_error(msg);
      }
    }
  } else {
    throw std::runtime_error(response_is_null);
  }
}

NAN_GETTER(NResponse::getSlices) {
  try {
    auto key = toString("__slices");
    if (Nan::Has(info.This(), key).FromJust()) {
      auto result = Nan::Get(info.This(), key);
      info.GetReturnValue().Set(result.ToLocalChecked());
    } else {
      auto result = buildV8Slices(info);
      Nan::Set(info.This(), key, result);
      info.GetReturnValue().Set(result);
    }
  } catch (std::exception const& e) {
    Nan::ThrowError("Reponse.getSlices binding failed with exception");
  }
}

v8::Local<v8::Value> NResponse::buildV8Slices(const Nan::PropertyCallbackInfo<v8::Value>& info) {
  auto res = self(info);
  if (res) {
    v8::Local<v8::Array> array = Nan::New<v8::Array>();
    if (res->isContentTypeVPack()) {
      std::size_t index = 0;
      for (auto const& slice : res->slices()) {
        auto buf = Nan::CopyBuffer(slice.startAs<char>(), slice.byteSize());
        //array->Set(index++, buf.ToLocalChecked());
      }
      return array;
    } else {
      // No vpack content type
      return Nan::Undefined();
    }
  } else {
    throw std::runtime_error(response_is_null);
  }
}

NAN_GETTER(NResponse::getPayload) {
  try {
    auto key = toString("__payload");
    if (Nan::Has(info.This(), key).FromJust()) {
      auto result = Nan::Get(info.This(), key);
      info.GetReturnValue().Set(result.ToLocalChecked());
    } else {
      auto result = buildV8Payload(info);
      Nan::Set(info.This(), key, result);
      info.GetReturnValue().Set(result);
    }
  } catch (std::exception const& e) {
    Nan::ThrowError("Reponse.getPayload binding failed with exception");
  }
}

v8::Local<v8::Value> NResponse::buildV8Payload(const Nan::PropertyCallbackInfo<v8::Value>& info) {
  auto res = self(info);
  if (res) {
    auto payload = res->payload();
    auto payloadSize = boost::asio::buffer_size(payload);
    auto payloadData = boost::asio::buffer_cast<const char*>(payload);
    auto buf = Nan::CopyBuffer(payloadData, payloadSize);
    return buf.ToLocalChecked();
  } else {
    throw std::runtime_error(response_is_null);
  }
}

NAN_GETTER(NResponse::getResponseCode) {
  try {
    auto res = self(info);
    if (res) {
      if (res->header.responseCode) {
        auto code = res->header.responseCode.get();
        info.GetReturnValue().Set(Nan::New<v8::Uint32>(code));
      } else {
        info.GetReturnValue().Set(Nan::Undefined());
      }
    } else {
      Nan::ThrowError(response_is_null);
      return;
    }
  } catch(std::exception const& e) {
    Nan::ThrowError("Response.responseCode binding failed with exception");
  }
}

NAN_GETTER(NResponse::getHeader) {
  try {
    auto key = toString("__header");
    if (Nan::Has(info.This(), key).FromJust()) {
      auto header = Nan::Get(info.This(), key);
      info.GetReturnValue().Set(header.ToLocalChecked());
    } else {
      auto header = buildV8Header(info);
      Nan::Set(info.This(), key, header);
      info.GetReturnValue().Set(header);
    }
  } catch (std::exception const& e) {
    Nan::ThrowError("Reponse.header binding failed with exception");
  }
}

v8::Local<v8::Object> NResponse::buildV8Header(const Nan::PropertyCallbackInfo<v8::Value>& info) {
  auto res = self(info);
  if (res) {
    auto& header = res->header;
    auto result = Nan::New<v8::Object>();
    if (header.meta) {
      auto meta = header.meta.value();
      auto isolate = info.GetIsolate()->GetCurrentContext();
      for (auto& pair : meta) {
        auto key = Nan::New(pair.first).ToLocalChecked();
        auto value = Nan::New(pair.second).ToLocalChecked();
        auto done = result->Set(isolate, key, value);
        if (!done.FromJust()) {
          throw std::runtime_error("Unable to write meta value to v8 object.");
        }
      }
    }
    return result;
  } else {
    throw std::runtime_error(response_is_null);
  }
}


NAN_GETTER(NResponse::getContentType) {
  try {
    auto res = self(info);
    if (res) {
      auto contentType = res->contentType();
      if (contentType != fu::ContentType::Unset) {
        info.GetReturnValue().Set(Nan::New(fu::to_string(contentType)).ToLocalChecked());
      } else {
        info.GetReturnValue().Set(Nan::Undefined());
      }
    } else {
      Nan::ThrowError(response_is_null);
      return;
    }
  } catch (std::exception const& e) {
    Nan::ThrowError("Response.contentType binding failed with exception");
  }
}

}}}
