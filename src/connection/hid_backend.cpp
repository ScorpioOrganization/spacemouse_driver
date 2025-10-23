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

#include "connection/hid_backend.hpp"

#include <stdexcept>
#include <vector>
#include <string>
#include <memory>

#include "device/device_registry.hpp"

namespace spacemouse_driver {

HidBackend::HidBackend(std::shared_ptr<SharedDeviceManager> shared_device_manager)
: _shared_device_manager(shared_device_manager) {
  if (hid_init()) {
    throw std::runtime_error("Failed to initialize hidapi.");
  }
}

HidBackend::~HidBackend() {
  try {
    hid_exit();
  } catch (...) {
  }
}

std::vector<DeviceInfo> HidBackend::enumerate() {
  hid_device_info* devs = hid_enumerate(0x0, 0x0);
  std::vector<DeviceInfo> devices;
  for (hid_device_info* dev = devs; dev; dev = dev->next) {
    devices.push_back({ dev->path, dev->vendor_id, dev->product_id, dev->interface_number });
  }
  hid_free_enumeration(devs);
  return devices;
}

std::shared_ptr<DeviceHandle> HidBackend::open(const std::string& path, uint16_t vid, uint16_t pid) {
  bool available = _shared_device_manager->claim_path(path);
  if (!available) {
    return nullptr;
  }
  auto hid_device = hid_open_path(path.c_str());
  if (!hid_device) {
    _shared_device_manager->release_path(path);
    return nullptr;
  }
  auto config = DeviceRegistry::get(vid, pid);
  if (!config) {
    hid_close(hid_device);
    _shared_device_manager->release_path(path);
    return nullptr;
  }
  return std::make_shared<DeviceHandle>(hid_device, *config, path);
}

int HidBackend::read(std::shared_ptr<DeviceHandle>& handle, uint8_t* buf, size_t len) {
  return hid_read_timeout(handle->hid_handle, buf, len, 100);
}

void HidBackend::close(std::shared_ptr<DeviceHandle>& handle) noexcept{
  if (handle->hid_handle) {
    hid_close(handle->hid_handle);
    _shared_device_manager->release_path(handle->path);
    handle.reset();
  }
}

}  // namespace spacemouse_driver
