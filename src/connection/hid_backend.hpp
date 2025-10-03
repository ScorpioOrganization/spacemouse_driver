/*
 * spacemouse_driver - User space driver for SpaceMouse devices
 * Copyright (C) 2025 Łukasz Kuś
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <hidapi/hidapi.h>

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

#include "types/device_types.hpp"
#include "device/shared_device_manager.hpp"

namespace spacemouse_driver {

class HidBackend
{
private:
  std::shared_ptr<SharedDeviceManager> _shared_device_manager;

public:
  explicit HidBackend(std::shared_ptr<SharedDeviceManager> shared_device_manager);
  virtual ~HidBackend();
  virtual std::vector<DeviceInfo> enumerate();
  virtual std::shared_ptr<DeviceHandle> open(const std::string& path, uint16_t vid, uint16_t pid);
  virtual int read(std::shared_ptr<DeviceHandle>& handle, uint8_t* buf, size_t len);
  virtual void close(std::shared_ptr<DeviceHandle>& handle) noexcept;
};

}  // namespace spacemouse_driver
