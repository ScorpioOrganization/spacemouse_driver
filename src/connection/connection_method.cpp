#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include <utility>

#include "connection/connection_method.hpp"
#include "driver/driver_context.hpp"
#include "device/device_registry.hpp"

namespace spacemouse_driver {

ModelListConnectionMethod::ModelListConnectionMethod(const std::vector<Model>& model_list)
: _model_list(model_list) { }

std::shared_ptr<DeviceHandle> ModelListConnectionMethod::connect(std::shared_ptr<DriverContext> context) {
  if (_model_list.empty()) {
    context->logger->error("No preferred models specified for device connection.");
    return nullptr;
  }
  auto devs = context->hid_backend->enumerate();
  std::vector<std::pair<DeviceInfo, int>> candidates;
  for (const auto& dev : devs) {
    auto device = DeviceRegistry::get(dev.vid, dev.pid);
    if (!device) { continue; }
    if (device->interface && device->interface != dev.interface) { continue; }

    auto it = std::find(_model_list.begin(), _model_list.end(), device->model);
    if (it == _model_list.end()) { continue; }

    int priority = static_cast<int>(std::distance(_model_list.begin(), it));
    candidates.emplace_back(dev, priority);
  }
  if (candidates.empty()) {
    context->logger->log("No listed SpaceMouse devices found.");
    return nullptr;
  }
  std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) { return a.second < b.second; });
  std::shared_ptr<DeviceHandle> result = nullptr;
  for (const auto& [dev, priority] : candidates) {
    if (!result) {
      result = context->hid_backend->open(dev.path, dev.vid, dev.pid);
      if (result) {
        continue;
      }
    }
  }
  return result;
}

PathConnectionMethod::PathConnectionMethod(const std::string& path)
: _path(path) { }

std::shared_ptr<DeviceHandle> PathConnectionMethod::connect(std::shared_ptr<DriverContext> context) {
  auto devs = context->hid_backend->enumerate();
  for (const auto& dev : devs) {
    if (dev.path == _path) {
      auto device = DeviceRegistry::get(dev.vid, dev.pid);
      if (!device) {
        context->logger->debug("Device at path " + _path + " is not a supported SpaceMouse device.");
        return nullptr;
      }
      auto device_handle = context->hid_backend->open(dev.path, dev.vid, dev.pid);
      if (!device_handle) {
        context->logger->error("Failed to open device at path: " + _path);
        return nullptr;
      }
      return device_handle;
    }
  }
  context->logger->debug("No device found at path: " + _path);
  return nullptr;
}

AnyModelConnectionMethod::AnyModelConnectionMethod() = default;

std::shared_ptr<DeviceHandle> AnyModelConnectionMethod::connect(std::shared_ptr<DriverContext> context) {
  auto devs = context->hid_backend->enumerate();
  for (const auto& dev : devs) {
    auto device = DeviceRegistry::get(dev.vid, dev.pid);
    if (!device) { continue; }
    if (device->interface && device->interface != dev.interface) { continue; }
    auto device_handle = context->hid_backend->open(dev.path, dev.vid, dev.pid);
    if (device_handle) {
      return device_handle;
    }
  }
  context->logger->debug("No SpaceMouse devices found.");
  return nullptr;
}

}  // namespace spacemouse_driver
