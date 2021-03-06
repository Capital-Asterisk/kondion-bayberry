/*
 * V8_OKO_Functions.h
 *
 *  Created on: Feb 13, 2017
 *      Author: neal
 */

#ifndef V8_OKO_FUNCTIONS_H_
#define V8_OKO_FUNCTIONS_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "include/libplatform/libplatform.h"
#include "include/v8.h"

#include "Kondion.h"

using namespace v8;

namespace Kondion {
namespace JS {

void Callback_OKO_Camera(const FunctionCallbackInfo<v8::Value>& args) {
  if (!args.IsConstructCall())
    return;
  HandleScope handle_scope(isolate);
  //printf("NEW: Camera\n");
  KObj::OKO_Camera_* o = new KObj::OKO_Camera_();
  //o->components.push_back(new Component::CPN_Cube);
  o->jsObject = new Persistent<v8::Object,
      CopyablePersistentTraits<v8::Object>>(isolate, args.This());
  o->name = "Camera";
  //Kondion::world.push_back(o);
  args.This()->SetInternalField(0, External::New(isolate, o));
  args.GetReturnValue().Set(args.This());
}

void Callback_OKO_GetOffsetPosition(
    const FunctionCallbackInfo<v8::Value>& args) {
  if (args.IsConstructCall() || args.Length() == 0
      || !(args[0]->IsFloat32Array() || args[0]->IsArray()))
    return;
  KObj_Oriented* pointer_this =
          static_cast<KObj_Oriented*>(Local<External>::Cast(
              args.This()->GetInternalField(0))->Value());
  Local<Array> a = Local<Array>::Cast(args[0]);
  a->Set(0, Number::New(isolate, pointer_this->orientation[3][0]));
  a->Set(1, Number::New(isolate, pointer_this->orientation[3][1]));
  a->Set(2, Number::New(isolate, pointer_this->orientation[3][2]));
  //const float* b = glm::value_ptr(pointer_this->offset);
  //b[12] = a->Get(0)->NumberValue();
  //b[13] = a->Get(1)->NumberValue();
  //b[14] = a->Get(2)->NumberValue();
  //pointer_this->transform;
}

void Callback_OKO_SetOffsetPosition(
    const FunctionCallbackInfo<v8::Value>& args) {
  if (args.IsConstructCall() || args.Length() == 0)
    return;
  //printf("eggsa");
  if (!(args[0]->IsFloat32Array() || args[0]->IsArray()))
    return;
  //printf("eggs!");
  Local<Array> a = Local<Array>::Cast(args[0]);
  KObj_Oriented* pointer_this =
      static_cast<KObj_Oriented*>(Local<External>::Cast(
          args.This()->GetInternalField(0))->Value());
  pointer_this->orientation[3][0] = a->Get(0)->NumberValue();
  pointer_this->orientation[3][1] = a->Get(1)->NumberValue();
  pointer_this->orientation[3][2] = a->Get(2)->NumberValue();
  //const float* b = glm::value_ptr(pointer_this->offset);
  //b[12] = a->Get(0)->NumberValue();
  //b[13] = a->Get(1)->NumberValue();
  //b[14] = a->Get(2)->NumberValue();
  //pointer_this->transform;
}

void Callback_OKO_Translate(const FunctionCallbackInfo<v8::Value>& args) {
  if (args.IsConstructCall() || args.Length() < 3)
    return;
  //Local<Array> a = Local<Array>::Cast(args[0]);
  KObj_Oriented* pointer_this =
      static_cast<KObj_Oriented*>(Local<External>::Cast(
          args.This()->GetInternalField(0))->Value());
  pointer_this->orientation = glm::translate(
      pointer_this->orientation,
      glm::vec3(args[0]->NumberValue(), args[1]->NumberValue(),
                args[2]->NumberValue()));
  //pointer_this->offset[3][0] = a->Get(0)->NumberValue();
  //pointer_this->offset[3][1] = a->Get(1)->NumberValue();
  //pointer_this->offset[3][2] = a->Get(2)->NumberValue();
}

void Callback_OKO_PointAt(const FunctionCallbackInfo<v8::Value>& args) {
  //// [x, y, z], [up x, up y, up z], origin
  // at, [up x, up y, up z], origin
  if (args.IsConstructCall())
    return;

  // TODO: set these wwith arguments
  glm::mat4 origin(1.0f);
  glm::mat4 that(1.0f);
  glm::vec4 up(0.0f, 1.0f, 0.0f, 0.0f);

  KObj_Oriented* pointer_this =
      static_cast<KObj_Oriented*>(Local<External>::Cast(
          args.This()->GetInternalField(0))->Value());

  // TODO get origin here

  // If args[0] is an array, or an oko
  if (args[0]->IsArray() || args[0]->IsFloat32Array()) {
    Local<Array> a = Local<Array>::Cast(args[0]);
    that[3][0] = a->Get(0)->NumberValue();
    that[3][1] = a->Get(1)->NumberValue();
    that[3][2] = a->Get(2)->NumberValue();
  } else {
    KObj_Oriented* pointer_that =
        static_cast<KObj_Oriented*>(Local<External>::Cast(args[0]->ToObject()
                                      ->GetInternalField(0))->Value());
    that = origin * pointer_that->orientation;
  }

  if (args[1]->IsArray() || args[1]->IsFloat32Array()) {
    Local<Array> a = Local<Array>::Cast(args[1]);
    up.x = a->Get(0)->NumberValue();
    up.y = a->Get(1)->NumberValue();
    up.z = a->Get(2)->NumberValue();
  }

  // Multiply by the origin
  pointer_this->orientation = origin * pointer_this->orientation;

  // Create a rotation that points towards the target, make this more efficient
  //glm::quat f(
  //    glm::vec3(
  //        -glm::atan(
  //            (pointer_this->orientation[3][1] - that[3][1])
  //                / glm::length(
  //                    glm::vec2(pointer_this->orientation[3][0] - that[3][0],
  //                              pointer_this->orientation[3][2] - that[3][2]))),
  //        glm::atan(pointer_this->orientation[3][0] - that[3][0],
  //                  pointer_this->orientation[3][2] - that[3][2]),
  //        0.0f))
  // Then convert it to Mat4
  glm::mat4 abird(1.0f);
  
  //up.x += glm::sin(float(TimeMs() / 10) / 31.4159) * 0.4f;
  //up.z += glm::cos(float(TimeMs() / 10) / 31.4159) * 0.4f;

  
  // Let's do this manually
  // it's "initial - final" so it doesn't have to be negated later on
  glm::vec3 np = glm::normalize(glm::vec3(pointer_this->orientation[3])
                                  - glm::vec3(that[3]));
  glm::vec3 cross = glm::cross(glm::vec3(up), np);
  up = glm::vec4(glm::cross(np, cross), 0.0f);

  abird[2][0] = np.x;
  abird[2][1] = np.y;
  abird[2][2] = np.z;

  abird[1][0] = up.x;
  abird[1][1] = up.y;
  abird[1][2] = up.z;

  abird[0][0] = cross.x;
  abird[0][1] = cross.y;
  abird[0][2] = cross.z;

  // Set the new matrix's translation to this object's translation
  abird[3][0] = pointer_this->orientation[3][0];
  abird[3][1] = pointer_this->orientation[3][1];
  abird[3][2] = pointer_this->orientation[3][2];

  // set this translation to the new one
  pointer_this->orientation = abird;

  // Multiply by the inverse to undo the effects of the first origin multiplication.
  pointer_this->orientation = glm::inverse(origin) * pointer_this->orientation;

}

void Callback_OKO_PointDir(const FunctionCallbackInfo<v8::Value>& args) {
  //// [x, y, z], [up x, up y, up z], origin
  if (args.IsConstructCall())
    return;

  // TODO: set these wwith arguments
  glm::mat4 origin(1.0f);
  glm::mat4 that(1.0f);
  glm::vec4 up(0.0f, 1.0f, 0.0f, 0.0f);

  KObj_Oriented* pointer_this =
      static_cast<KObj_Oriented*>(Local<External>::Cast(
          args.This()->GetInternalField(0))->Value());

  // TODO get origin here

  // If args[0] is an array, or an oko
  if (args[0]->IsArray() || args[0]->IsFloat32Array()) {
    Local<Array> a = Local<Array>::Cast(args[0]);
    that[3][0] = a->Get(0)->NumberValue();
    that[3][1] = a->Get(1)->NumberValue();
    that[3][2] = a->Get(2)->NumberValue();
  } else
    return;

  if (args[1]->IsArray() || args[1]->IsFloat32Array()) {
    Local<Array> a = Local<Array>::Cast(args[1]);
    up.x = a->Get(0)->NumberValue();
    up.y = a->Get(1)->NumberValue();
    up.z = a->Get(2)->NumberValue();
  }

  // Multiply by the origin
  pointer_this->orientation = origin * pointer_this->orientation;
  glm::mat4 abird(1.0f);
  
  //up.x += glm::sin(float(TimeMs() / 10) / 31.4159) * 0.4f;
  //up.z += glm::cos(float(TimeMs() / 10) / 31.4159) * 0.4f;

  
  // Same as above but with modifications
  glm::vec3 np = glm::normalize(-glm::vec3(that[3]));
  glm::vec3 cross = glm::normalize(glm::cross(glm::vec3(up), np));
  up = glm::vec4(glm::cross(np, cross), 0.0f);

  abird[2][0] = np.x;
  abird[2][1] = np.y;
  abird[2][2] = np.z;

  abird[1][0] = up.x;
  abird[1][1] = up.y;
  abird[1][2] = up.z;

  abird[0][0] = cross.x;
  abird[0][1] = cross.y;
  abird[0][2] = cross.z;

  // Set the new matrix's translation to this object's translation
  abird[3][0] = pointer_this->orientation[3][0];
  abird[3][1] = pointer_this->orientation[3][1];
  abird[3][2] = pointer_this->orientation[3][2];

  // set this translation to the new one
  pointer_this->orientation = abird;

  // Multiply by the inverse to undo the effects of the first origin multiplication.
  pointer_this->orientation = glm::inverse(origin) * pointer_this->orientation;

}

void Callback_OKO_Rotate(const FunctionCallbackInfo<v8::Value>& args) {
  if (args.IsConstructCall() || args.Length() == 0)
    return;
  if (!args[0]->IsArray())
    return;
  Local<Array> a = Local<Array>::Cast(args[0]);
  KObj_Oriented* pointer_this =
      static_cast<KObj_Oriented*>(Local<External>::Cast(
          args.This()->GetInternalField(0))->Value());
  pointer_this->orientation[3][0] = a->Get(0)->NumberValue();
  pointer_this->orientation[3][1] = a->Get(1)->NumberValue();
  pointer_this->orientation[3][2] = a->Get(2)->NumberValue();
}

void Callback_OKO_DirFd(
    const FunctionCallbackInfo<v8::Value>& args) {
  if (args.IsConstructCall() || args.Length() == 0
      || !(args[0]->IsFloat32Array() || args[0]->IsArray()))
    return;
  KObj_Oriented* pointer_this =
          static_cast<KObj_Oriented*>(Local<External>::Cast(
              args.This()->GetInternalField(0))->Value());
  Local<Array> a = Local<Array>::Cast(args[0]);
  a->Set(0, Number::New(isolate, -pointer_this->orientation[2][0]));
  a->Set(1, Number::New(isolate, -pointer_this->orientation[2][1]));
  a->Set(2, Number::New(isolate, -pointer_this->orientation[2][2]));
}

void Callback_OKO_DirUp(
    const FunctionCallbackInfo<v8::Value>& args) {
  if (args.IsConstructCall() || args.Length() == 0
      || !(args[0]->IsFloat32Array() || args[0]->IsArray()))
    return;
  KObj_Oriented* pointer_this =
          static_cast<KObj_Oriented*>(Local<External>::Cast(
              args.This()->GetInternalField(0))->Value());
  Local<Array> a = Local<Array>::Cast(args[0]);
  a->Set(0, Number::New(isolate, pointer_this->orientation[1][0]));
  a->Set(1, Number::New(isolate, pointer_this->orientation[1][1]));
  a->Set(2, Number::New(isolate, pointer_this->orientation[1][2]));
}

void Callback_OKO_DirRt(
    const FunctionCallbackInfo<v8::Value>& args) {
  if (args.IsConstructCall() || args.Length() == 0
      || !(args[0]->IsFloat32Array() || args[0]->IsArray()))
    return;
  KObj_Oriented* pointer_this =
          static_cast<KObj_Oriented*>(Local<External>::Cast(
              args.This()->GetInternalField(0))->Value());
  Local<Array> a = Local<Array>::Cast(args[0]);
  a->Set(0, Number::New(isolate, pointer_this->orientation[0][0]));
  a->Set(1, Number::New(isolate, pointer_this->orientation[0][1]));
  a->Set(2, Number::New(isolate, pointer_this->orientation[0][2]));
}

}
}

#endif /* V8_OKO_FUNCTIONS_H_ */
