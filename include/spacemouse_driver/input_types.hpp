#pragma once

#include <optional>
#include <array>
#include <variant>
#include <chrono>
#include <string>

#include "magic_enum/magic_enum.hpp"

namespace spacemouse_driver {

enum class Axis
{
  LinearX,
  LinearY,
  LinearZ,
  AngularX,
  AngularY,
  AngularZ
};
constexpr size_t AxisCount = magic_enum::enum_count<Axis>();

enum class Button
{
  Button1,
  Button2,
  Button3,
  Button4,
  Button5,
  Button6,
  Button7,
  Button8,
  Button9,
  Button10,
  Button11,
  Button12,
  Escape,
  Enter,
  Tab,
  Shift,
  Ctrl,
  Alt,
  Space,
  Menu,
  Delete,
  V1,
  V2,
  V3,
  Rotate,
  Top,
  Front,
  Right,
  Lock,
  Iso,
  Fit
};
constexpr size_t ButtonCount = magic_enum::enum_count<Button>();

struct StickInput
{
  std::array<double, AxisCount> axis;

  std::string to_string() const {
    std::string result = "";
    for (size_t i = 0; i < AxisCount; ++i) {
      auto axis_name = magic_enum::enum_name(static_cast<Axis>(i));
      result += std::string(axis_name) + ": " + std::to_string(axis[i]) + " ";
    }
    return result;
  }

  bool operator==(const StickInput& other) const {
    for (size_t i = 0; i < AxisCount; ++i) {
      if (axis[i] != other.axis[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const StickInput& other) const {
    return !(*this == other);
  }
};

using ButtonInput = bool;

struct Input
{
  StickInput stick;
  std::array<ButtonInput, ButtonCount> buttons;

  bool operator==(const Input& other) const {
    if (stick != other.stick) {
      return false;
    }
    for (size_t i = 0; i < ButtonCount; ++i) {
      if (buttons[i] != other.buttons[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const Input& other) const {
    return !(*this == other);
  }
};

}  // namespace spacemouse_driver
