/*
 * Input.cpp
 *
 *  Created on: Oct 29, 2016
 *      Author: neal
 */

#include <GLFW/glfw3.h>

#include "Kondion.h"

namespace Kondion {
namespace Input {

/*
 * -Input devices, adds input types:
 * Touch screen (pointer(s))
 * Mouse (allow multiple) (button, pointer, axis)
 * Keyboard (buttons, text)
 * Controller (buttons + hat, axis, sliders)
 * midi?
 *
 * -Input types:
 * Pointer - XY
 * Axis - more XY
 * Slider - X
 * Button
 *
 * -Virtual Input devices:
 * Virtual slider (Button up/down, axis direction)
 * Virtual axis (up down left right or cursor)
 * Virtual Cursor (up down left right or cursor)
 *
 * -Controls
 * List of binds go somewhere
 * vector: struct or something {buttonname, device, key/button/axis, ... }
 * associated with js input object
 *
 * -Game input:
 * All input is in numbers
 * js: (input["key_shoot"].x >= 0.5) is when button is down
 * js: (input["move_lr"].y * forward) for axis
 *
 * device:
 * 0: system input (mouse, keyboard, touchscreen?)
 *     0-399: keyboard
 *     400-407: mouse buttons
 *     408 - scroll, x and y axis
 *     409 - mouse position on window, subpixel
 *     410 - 0-1 cursor in window
 * 1: virtual input
 * 2:
 *
 * 10-26: Joystick 1-16
 *     (will a controller ever have that many buttons?)
 *     0-99 - Buttons
 *     100-199 - Axis
 */

// I don't know how to avoid these
std::vector<Control*> controls;

/**
 * Add a new control.
 * @param name What you want the name of the control to be.
 * @param device The device the control with be connected to.
 * @param button The button or axis of the device.
 * @return The index of the new control.
 */
uint16_t AddControl(const std::string& name, uint8_t device, uint16_t button) {

  Control* f = new Control;
  f->button = button;
  f->device = device;
  f->name = name;
  int compared = -1;
  uint16_t i = 0;
  if (controls.size() != 0) {
    i = controls.size();  // last element
    //printf("chirp chirp %i\n", name.compare(controls[i]->name));
    //while (i != 0 && (compared = name.compare(controls[i]->name)) > 0) {
    //printf("birds: %i, compared: %i\n", i, compared);
    //i--;
    //}
    printf("BIRDS! %s\n", name.c_str());
    while (i != 0 && compared != 0) {
      i--;
      compared = name.compare(controls[i]->name);
      //compared = strcmp(name.c_str(), controls[i]->name.c_str());
      printf("%s -- %s comparison: %i\n", name.c_str(),
             controls[i]->name.c_str(), compared);
    };
    i++;
  }

  f->alternate = (compared == 0);
  controls.insert(controls.begin() + i, f);
  //printf("birds: %i\n", i);
  return i;
}

uint16_t ControlIndex(const std::string& name) {
  for (unsigned short i = 0; i < controls.size(); i++) {
    if (controls[i]->name.compare(name) == 0)
      return i;
  }
  return controls.size();
}

Control* Get(uint16_t i) {
  return controls[i];
}

uint16_t Count() {
  return controls.size();
}

float Value(uint16_t i) {
  //printf("poop %f\n", controls[i]->x);
  if (i >= controls.size())
    return 0.0f;

  float r = controls[i]->x;
  i++;
  while (i < controls.size() && controls[i]->alternate) {
    r = (std::abs(r) < std::abs(controls[i]->x)) ? controls[i]->x : r;
    i++;
    //r = std::max(r, controls[i]->px);
  }
  return r;
}

float ValuePrev(uint16_t i) {
  if (i >= controls.size())
    return 0.0f;
  float r = controls[i]->px;
  i++;
  while (i < controls.size() && controls[i]->alternate) {
    r = (std::abs(r) < std::abs(controls[i]->px)) ? controls[i]->px : r;
    i++;
    //r = std::max(r, controls[i]->px);
  }
  return r;
}

void MouseLock(bool a) {
  glfwSetInputMode(Window::w, GLFW_CURSOR,
                   a ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Setup() {

  for (unsigned char i = 0; i < 15; i++) {
    std::cout << "Controller " << int(i) << ": "
              << (glfwJoystickPresent(i) ? "EXISTS" : "NOPE") << "\n";
    if (glfwJoystickPresent(i)) {
      std::cout << "-- " << glfwGetJoystickName(i) << "\n";
      int amt;
      glfwGetJoystickAxes(i, &amt);
      std::cout << "Axis #: " << amt << " Button #: ";
      glfwGetJoystickButtons(i, &amt);
      std::cout << amt << "\n";
    }
  }
  //printf("Joysticks Detected: %i");

}

void Update() {
  for (unsigned short i = 0; i < controls.size(); i++) {
    controls[i]->px = controls[i]->x;
    //controls[i]->py = controls[i]->y;
    if (controls[i]->device == 0) {
      // system controller
      if (controls[i]->button < MOUSE_BUTTON) {
        // keyboard
        controls[i]->x = glfwGetKey(Window::w, controls[i]->button);
        //controls[i]->y = 0;
      } else if (controls[i]->button < MOUSE_BUTTON + 8) {
        controls[i]->x = glfwGetMouseButton(Window::w,
                                            controls[i]->button - 400);
        //controls[i]->y = 0;
      } else if (controls[i]->button == MOUSE_POSX) {
        double x, y;
        glfwGetCursorPos(Window::w, &x, &y);
        controls[i]->x = x;
        //controls[i]->y = y;
      } else if (controls[i]->button == MOUSE_POSY) {
        double x, y;
        glfwGetCursorPos(Window::w, &x, &y);
        controls[i]->x = y;
        //controls[i]->y = y; else if (controls[i]->button == MOUSE_SCROLLX) {

      } else if (controls[i]->button == MOUSE_SCROLLY) {

      } else if (controls[i]->button == MOUSE_INSIDE) {

      }
    } else if (controls[i]->device >= 10) {
      unsigned char joy = controls[i]->device - 10;
      if (controls[i]->button < CONTROLLER_AXIS) {
        // it is a button
        int dummy;
        controls[i]->x =
            glfwGetJoystickButtons(joy, &dummy)[controls[i]->button];
      } else {  // else if (whatever), but nothing yet
        int dummy;
        controls[i]->x = glfwGetJoystickAxes(joy, &dummy)[controls[i]->button
            - CONTROLLER_AXIS];
      }
    }
  }
}

void DebugPrint() {
  for (uint16_t i = 0; i < controls.size(); i++) {
    printf("Name: %s Alt: %i X: %4.2f \n", controls[i]->name.c_str(),
           controls[i]->alternate, controls[i]->x);
    //std::cout << "name: " << controls[i]->name << " alternate:" << controls[i]->alternate;
  }
}
}
}

