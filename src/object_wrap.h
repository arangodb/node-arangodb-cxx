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
/// @author Ewout Prangsma
////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef FUERTE_NODE_OBJECT_WRAP_H
#define FUERTE_NODE_OBJECT_WRAP_H

#include "node_upstream.h"

namespace arangodb { namespace fuerte { namespace js {

// ObjectWrap is a template helper for creating smart ObjectWrap classes.
template <class T, class TCpp, class TPtr>
class ObjectWrap : public Nan::ObjectWrap {
protected:
    ObjectWrap(): _cppClass(new TCpp) {}
    ObjectWrap(TPtr x): _cppClass(std::move(x)) {}

public:
  TCpp* cppClass() {
    return _cppClass.get();
  }

  // NewInstance creates a new instance of the object.
  static Nan::MaybeLocal<v8::Object> NewInstance() {
    auto ctor = Nan::New(constructor());
    return Nan::NewInstance(ctor);
  }
  // NewInstance creates a new instance of the object with given ctor arguments.
  static Nan::MaybeLocal<v8::Object> NewInstance(int argc, v8::Local<v8::Value> argv[]) {
    auto ctor = Nan::New(constructor());
    return Nan::NewInstance(ctor, argc, argv);
  }

  static T* CheckedUnwrap(v8::Handle<v8::Object> handle) {
    // Sanity check the object before we accept it as one of our own
    // wrapped objects
    
    // Basic checks done as asserts by UnWrap()
    if (!handle.IsEmpty() && handle->InternalFieldCount() == 1) {
      // Check the prototype.  This effectively stops inheritance,
      // but since this is created from a factory function and no
      // constructor is exposed that should not be ok.  If you really need
      // inheritance, turn this into a loop walking the prototype chain.
      auto objproto = handle->GetPrototype();
      if (objproto == prototype()) {
        // OK, this is us
        return Nan::ObjectWrap::Unwrap<T>(handle);
      }
    }
    
    Nan::ThrowTypeError(("<this> is not a " + className()).c_str());
    return nullptr;
  }

protected:
  void setCppClass(TPtr x) {
    _cppClass = std::move(x);
  }

  // self unwraps from "this" in a FunctionCallbackInfo to our cppClass.
  template <typename TInfo>
  inline static TCpp* self(Nan::FunctionCallbackInfo<TInfo> const& info) {
    return CheckedUnwrap(info.Holder())->cppClass();
  }

  // self unwraps from "this" in a PropertyCallbackInfo to our cppClass.
  template <typename TInfo>
  inline static TCpp* self(Nan::PropertyCallbackInfo<TInfo> const& info) {
    return CheckedUnwrap(info.Holder())->cppClass();
  }

  // initClass initialize a JS class for this wrapper.
  static void initClass(const std::string& name, v8::Handle<v8::Object> module, v8::Local<v8::FunctionTemplate> tpl) {
    className() = name;
    auto ctor = tpl->GetFunction();
    constructor().Reset(ctor);
    auto obj = ctor->NewInstance();
    prototype().Reset(obj->GetPrototype());
    module->Set(Nan::New(name).ToLocalChecked(), ctor); 
  }

private:
  TPtr _cppClass;
  static std::string& className() {
    static std::string name;
    return name;
  }
  static Nan::Persistent<v8::Function>& constructor() {
    static Nan::Persistent<v8::Function> ctor;
    return ctor;
  }
  static Nan::Persistent<v8::Value>& prototype() {
    static Nan::Persistent<v8::Value> p;
    return p;
  }
};

}}}
#endif
