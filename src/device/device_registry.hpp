#pragma once

#include <array>
#include <optional>

#include "types/device_types.hpp"

#define BUTTON_MAPPINGS(...) std::array<std::optional<ButtonMapping>, ButtonCount>{ __VA_ARGS__ }
#define AXIS_MAPPINGS(...) std::array<AxisMapping, AxisCount>{ __VA_ARGS__ }

#define AXIS(ax, low, high, ch, inv) AxisMapping{ ax, low, high, ch, inv }
#define BYTE_CODE(btn, ch, code) ByteCodeMapping{ btn, ch, code }
#define BIT_MASK(btn, ch, byte_idx, bit_idx) BitMaskMapping{ btn, ch, byte_idx, bit_idx }

#define ANY_INTERFACE std::nullopt

namespace spacemouse_driver {

namespace DeviceRegistry {

constexpr auto WIRELESS_AXIS = AXIS_MAPPINGS(
        AXIS(Axis::LinearX, 0x01, 1, 2, false),
        AXIS(Axis::LinearY, 0x01, 3, 4, true),
        AXIS(Axis::LinearZ, 0x01, 5, 6, true),
        AXIS(Axis::AngularX, 0x01, 7, 8, true),
        AXIS(Axis::AngularY, 0x01, 9, 10, true),
        AXIS(Axis::AngularZ, 0x01, 11, 12, false)
);

constexpr auto WIRELESS_BUTTONS = BUTTON_MAPPINGS(
        BIT_MASK(Button::Button1, 0X03, 1, 0),
        BIT_MASK(Button::Button2, 0x03, 1, 1)
);

inline constexpr auto DEVICES = std::array{
  DeviceConfig(
            Model::SpaceMouseEnterprise,  // Model
            0x256f, 0xc633,  // VID, PID
            ANY_INTERFACE,  // Interface number
            350,            // Axis data divisor (to normalize data into [-1.0; 1.0] range)
            AXIS_MAPPINGS(  // Axis, Report ID, Byte Low IDX, Byte High IDX, Inverse
                AXIS(Axis::LinearX, 0x01, 1, 2, false),
                AXIS(Axis::LinearY, 0x01, 3, 4, true),
                AXIS(Axis::LinearZ, 0x01, 5, 6, true),
                AXIS(Axis::AngularX, 0x01, 7, 8, true),
                AXIS(Axis::AngularY, 0x01, 9, 10, true),
                AXIS(Axis::AngularZ, 0x01, 11, 12, false)
    ),
            BUTTON_MAPPINGS(  // Button, Report ID, Code
                BYTE_CODE(Button::Button1, 0x1C, 0x0D),
                BYTE_CODE(Button::Button2, 0x1C, 0x0E),
                BYTE_CODE(Button::Button3, 0x1C, 0x0F),
                BYTE_CODE(Button::Button4, 0x1C, 0x10),
                BYTE_CODE(Button::Button5, 0x1C, 0x11),
                BYTE_CODE(Button::Button6, 0x1C, 0x12),
                BYTE_CODE(Button::Button7, 0x1C, 0x13),
                BYTE_CODE(Button::Button8, 0x1C, 0x14),
                BYTE_CODE(Button::Button9, 0x1C, 0x15),
                BYTE_CODE(Button::Button10, 0x1C, 0x16),
                BYTE_CODE(Button::Button11, 0x1C, 0x4D),
                BYTE_CODE(Button::Button12, 0x1C, 0x4E),
                BYTE_CODE(Button::Escape, 0x1C, 0x17),
                BYTE_CODE(Button::Enter, 0x1C, 0x24),
                BYTE_CODE(Button::Tab, 0x1C, 0xAF),
                BYTE_CODE(Button::Shift, 0x1C, 0x19),
                BYTE_CODE(Button::Ctrl, 0x1C, 0x1A),
                BYTE_CODE(Button::Alt, 0x1C, 0x18),
                BYTE_CODE(Button::Space, 0x1C, 0xB0),
                BYTE_CODE(Button::Menu, 0x1C, 0x01),
                BYTE_CODE(Button::Delete, 0x1C, 0x25),
                BYTE_CODE(Button::V1, 0x1C, 0x67),
                BYTE_CODE(Button::V2, 0x1C, 0x68),
                BYTE_CODE(Button::V3, 0x1C, 0x69),
                BYTE_CODE(Button::Rotate, 0x1C, 0x09),
                BYTE_CODE(Button::Top, 0x1C, 0x03),
                BYTE_CODE(Button::Front, 0x1C, 0x06),
                BYTE_CODE(Button::Right, 0x1C, 0x05),
                BYTE_CODE(Button::Lock, 0x1C, 0x1B),
                BYTE_CODE(Button::Iso, 0x1C, 0x0B),
                BYTE_CODE(Button::Fit, 0x1C, 0x02)
            )
  ),
  DeviceConfig(
            Model::SpaceMouseWireless,  // via dongle
            0x256F, 0xC652,
            2,
            350,
            WIRELESS_AXIS,
            WIRELESS_BUTTONS
  ),
  DeviceConfig(
            Model::SpaceMouseWireless,  // via USB and BT
            0x256F, 0xC63A,
            ANY_INTERFACE,
            350,
            WIRELESS_AXIS,
            WIRELESS_BUTTONS
  )
};

inline constexpr std::optional<DeviceConfig> get(uint16_t vid, uint16_t pid) {
  for (const auto& device : DEVICES) {
    if (device.vid == vid && device.pid == pid) {
      return device;
    }
  }
  return std::nullopt;
}

inline constexpr bool is_supported(Model model) {
  for (const auto& device : DEVICES) {
    if (device.model == model) {
      return true;
    }
  }
  return false;
}

}  // namespace DeviceRegistry

}  // namespace spacemouse_driver
