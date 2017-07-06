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

#ifndef FUERTE_NODE_VPACK_H
#define FUERTE_NODE_VPACK_H

#include <nan.h>
#include <velocypack/Buffer.h>
#include <velocypack/Slice.h>
#include <velocypack/Builder.h>
#include <velocypack/velocypack-aliases.h>

//replaces arangodb string ref
#include <experimental/string_view>

namespace arangodb {
  using StringRef = std::experimental::string_view;
}

namespace arangodb { namespace fuerte { namespace js {

// types
struct BuilderContext;
using VPBuffer = ::arangodb::velocypack::Buffer<uint8_t>;

// constants
static uint8_t const AttributeBase = 0x30;
static uint8_t const KeyAttribute = 0x31;
static uint8_t const RevAttribute = 0x32;
static uint8_t const IdAttribute = 0x33;
static uint8_t const FromAttribute = 0x34;
static uint8_t const ToAttribute = 0x35;

static int const TRI_ERROR_NO_ERROR = 0;
static int const TRI_ERROR_BAD_PARAMETER = 2;
static int const TRI_ERROR_OUT_OF_MEMORY = 4;
static int const TRI_ERROR_NOT_IMPLEMENTED = 8;

// functions

// decode to js object
v8::Local<v8::Value> TRI_VPackToV8(v8::Isolate* isolate, VPackSlice const& slice, 
  VPackOptions const* options, VPackSlice const* base = nullptr);

// encode to vpack
int TRI_V8ToVPack(v8::Isolate* isolate, VPackBuilder& builder, 
  v8::Local<v8::Value> const value, bool keepTopLevelOpen);

// this functions does most of the work (template!)
template <bool performAllChecks, bool inObject>
int V8ToVPack(BuilderContext& context, v8::Local<v8::Value> const parameter, 
  arangodb::StringRef const& attributeName);

NAN_METHOD(vpackDecode);
NAN_METHOD(vpackEncode);
NAN_MODULE_INIT(InitVPack);

}}}

#endif
