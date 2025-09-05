#pragma once

#include <hidapi/hidapi.h>

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

#include "types/device_types.hpp"
#include "device/shared_device_manager.hpp"

namespace spacemouse_driver {

class HidBackend {
private:
  std::shared_ptr<SharedDeviceManager> _shared_device_manager;

public:
  explicit HidBackend(std::shared_ptr<SharedDeviceManager> shared_device_manager);
  virtual ~HidBackend();
  virtual std::vector<DeviceInfo> enumerate();
  virtual std::shared_ptr<DeviceHandle> open(const std::string& path, uint16_t vid, uint16_t pid);
  virtual int read(std::shared_ptr<DeviceHandle> handle, uint8_t* buf, size_t len);
  virtual void close(std::shared_ptr<DeviceHandle> handle) noexcept;
};

}  // namespace spacemouse_driver
