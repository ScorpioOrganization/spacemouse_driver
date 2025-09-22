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
  return hid_read(handle->hid_handle, buf, len);
}

void HidBackend::close(std::shared_ptr<DeviceHandle>& handle) noexcept{
  if (handle->hid_handle) {
    hid_close(handle->hid_handle);
    _shared_device_manager->release_path(handle->path);
    handle.reset();
  }
}

}  // namespace spacemouse_driver
