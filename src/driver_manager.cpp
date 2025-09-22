#include "spacemouse_driver/driver_manager.hpp"

#include <string>
#include <vector>

#include "spacemouse_driver/driver.hpp"
#include "connection/connection_method.hpp"
#include "device/device_registry.hpp"
#include "driver/driver_context.hpp"

namespace spacemouse_driver {

DriverManager::DriverManager(std::unique_ptr<Logger> logger, LogLevel log_level)
: _context(
    std::make_shared<DriverContext>(
      std::make_unique<HidBackend>(std::make_shared<SharedDeviceManager>()),
      std::move(logger)
    )
),
  _drivers() {
  if (!_context->logger) {
    throw std::invalid_argument("Logger instance cannot be null.");
  }

  set_log_level(log_level);
}

DriverManager::DriverManager()
: DriverManager(std::make_unique<ConsoleLogger>(), LogLevel::Warning) { }

DriverManager::~DriverManager() {
  for (auto& driver : _drivers) {
    driver->stop();
  }
}

std::shared_ptr<Driver> DriverManager::create_driver(const std::vector<Model>& model_list) {
  std::vector<Model> model_list_cpy = model_list;
  for (const auto& model : model_list_cpy) {
    if (!DeviceRegistry::is_supported(model)) {
      _context->logger->error(
        "Unsupported device model specified: " +
        std::string(magic_enum::enum_name(model)));
      return nullptr;
    }
  }
  if (model_list_cpy.empty()) {
    auto all_models = magic_enum::enum_values<Model>();
    model_list_cpy = std::vector<Model>(all_models.begin(), all_models.end());
  }
  auto conn_method = std::make_shared<ModelListConnectionMethod>(model_list_cpy);
  auto driver = make_driver(conn_method);
  return driver;
}

std::shared_ptr<Driver> DriverManager::create_driver() {
  auto conn_method = std::make_shared<AnyModelConnectionMethod>();
  auto driver = make_driver(conn_method);
  return driver;
}

std::shared_ptr<Driver> DriverManager::create_driver(Model device_model) {
  return create_driver(std::vector<Model>{ device_model });
}

std::shared_ptr<Driver> DriverManager::create_driver(const std::string& device_path) {
  if (device_path.empty()) {
    _context->logger->error("Device path cannot be empty.");
    return nullptr;
  }
  auto conn_method = std::make_shared<PathConnectionMethod>(device_path);
  auto driver = make_driver(conn_method);
  return driver;
}

void DriverManager::set_log_level(LogLevel level) {
  _context->logger->set_log_level(level);
}

std::shared_ptr<Driver> DriverManager::make_driver(
  const std::shared_ptr<ConnectionMethod>& conn_method) {
  auto driver = std::make_shared<Driver>(_context, conn_method);
  _drivers.push_back(driver);
  return driver;
}

}  // namespace spacemouse_driver
