#pragma once

#include <hidapi/hidapi.h>

#include <array>
#include <optional>
#include <cstdint>
#include <string>

#include "magic_enum/magic_enum.hpp"

#include "spacemouse_driver/input_types.hpp"
#include "spacemouse_driver/device_model.hpp"
#include "types/mapping_types.hpp"

namespace spacemouse_driver {

struct DeviceInfo {
  std::string path;
  uint16_t vid;
  uint16_t pid;
  int interface;
};

struct DeviceConfig {
  std::optional<Model> model;
  uint16_t vid;
  uint16_t pid;
  std::optional<int> interface;

  int16_t axis_div;

  std::array<AxisMapping, AxisCount> axis_mappings;
  std::array<std::optional<ButtonMapping>, ButtonCount> button_mappings{ };

  AxisMapping get_axis_mapping(Axis axis) const {
    return axis_mappings[*magic_enum::enum_index(axis)];
  }

  std::optional<ButtonMapping> get_button_mapping(Button button) const {
    return button_mappings[*magic_enum::enum_index(button)];
  }

  constexpr DeviceConfig(
    Model m, uint16_t v, uint16_t p, std::optional<int> i, int16_t div,
    const std::array<AxisMapping, AxisCount>& axes,
    const std::array<std::optional<ButtonMapping>, ButtonCount>& buttons)
  : model(m), vid(v), pid(p), interface(i), axis_div(div), axis_mappings(axes), button_mappings(
      buttons) { }

  constexpr DeviceConfig()
  : model(std::nullopt), vid(0), pid(0), interface(0), axis_div(1),
    axis_mappings{}, button_mappings{} { }
};

struct DeviceHandle {
  hid_device* hid_handle;
  DeviceConfig config;
  std::string path;

  DeviceHandle(hid_device* hid_dev, const DeviceConfig& conf, const std::string& dev_path)
  : hid_handle(hid_dev), config(conf), path(dev_path) { }

  std::string get_name() const {
    return std::string(magic_enum::enum_name(config.model.value())) + " (" + path + ")";
  }
};

}  // namespace spacemouse_driver
