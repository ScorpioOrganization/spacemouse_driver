#pragma once

#include <optional>
#include <array>
#include <variant>
#include <chrono>
#include <string>

#include "magic_enum/magic_enum.hpp"

namespace spacemouse_driver {

/**
 * @brief Enumeration of SpaceMouse movement axes
 *
 * Represents the six degrees of freedom available on SpaceMouse devices:
 * three linear axes and three angular axes.
 */
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

/**
 * @brief Enumeration of SpaceMouse buttons
 *
 * Represents all possible buttons that can be found on various SpaceMouse models.
 * Not all buttons are available on all devices.
 */
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

/**
 * @brief Represents input from the SpaceMouse stick
 *
 * Contains normalized values for all six degrees of freedom.
 * Values typically range from -1.0 to 1.0, where 0.0 represents no movement.
 */
struct StickInput {
  std::array<double, AxisCount> axis;  // Array of axis values indexed by Axis enum

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

  auto operator[](Axis a) const {
    return axis[*magic_enum::enum_index(a)];
  }
};

/**
 * @brief Type alias for button input state
 *
 * Represents the pressed state of a button: true = pressed, false = released
 */
using ButtonInput = bool;

/**
 * @brief Complete input state from a SpaceMouse device
 *
 * Combines stick input (6DOF movement) with the state of all buttons.
 * This represents a complete device state at a given moment.
 */
struct Input {
  StickInput stick;  // Current stick position and orientation
  std::array<ButtonInput, ButtonCount> buttons;  // State of all buttons indexed by Button enum

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

  auto operator[](const Button& b) const {
    return buttons[*magic_enum::enum_index(b)];
  }

  auto operator[](const Axis& a) const {
    return stick[a];
  }
};

}  // namespace spacemouse_driver
