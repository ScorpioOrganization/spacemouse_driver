#pragma once

#include "spacemouse_driver/input_types.hpp"

namespace spacemouse_driver
{

struct BitMaskMapping
{
  Button button;
  uint8_t report_id;
  uint8_t byte_index;
  uint8_t bit_index;

  std::optional<bool> parse(const uint8_t * data, const size_t) const
  {
    if (report_id != data[0]) {
      return std::nullopt;
    }
    return (data[byte_index] & (1 << bit_index)) != 0;
  }
};

struct ByteCodeMapping
{
  Button button;
  uint8_t report_id;
  uint8_t code;

  std::optional<bool> parse(const uint8_t * data, const size_t length) const
  {
    if (report_id != data[0]) {
      return std::nullopt;
    }
    for (size_t i = 1; i < length; ++i) {
      if (data[i] == code) {
        return true;
      }
    }
    return false;
  }
};

using ButtonMapping = std::variant<BitMaskMapping, ByteCodeMapping>;

struct AxisMapping
{
  Axis axis;
  uint8_t report_id;
  uint8_t byte_low_idx;
  uint8_t byte_high_idx;
  bool invert;

  std::optional<int16_t> parse(const uint8_t * data, const size_t length) const
  {
    if (report_id != data[0]) {
      return std::nullopt;
    }
    if (byte_low_idx >= length || byte_high_idx >= length) {
      return std::nullopt;
    }
    uint8_t byte_low = data[byte_low_idx];
    uint8_t byte_high = data[byte_high_idx];
    int16_t raw_data = (static_cast<int16_t>(byte_high) << 8) | byte_low;
    if (invert) {raw_data = -raw_data;}
    return raw_data;
  }
};

}  // namespace spacemouse_driver
