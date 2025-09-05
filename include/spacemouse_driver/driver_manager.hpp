#pragma once

#include <vector>
#include <memory>
#include <string>

#include "spacemouse_driver/logger.hpp"

namespace spacemouse_driver {

class SharedDeviceManager;
class Driver;
class DriverContext;
class ConnectionMethod;
class Logger;
enum class Model;

class DriverManager {
public:
  DriverManager();
  explicit DriverManager(std::unique_ptr<Logger> logger, LogLevel log_level = LogLevel::Warning);
  ~DriverManager();

  void set_log_level(LogLevel level);

  std::shared_ptr<Driver> create_driver();
  std::shared_ptr<Driver> create_driver(const std::vector<Model>& model_list);
  std::shared_ptr<Driver> create_driver(Model device_model);
  std::shared_ptr<Driver> create_driver(const std::string& device_path);

private:
  std::shared_ptr<DriverContext> _context;
  std::vector<std::shared_ptr<Driver>> _drivers;

  std::shared_ptr<Driver> make_driver(const std::shared_ptr<ConnectionMethod>& conn_method);
};

}  // namespace spacemouse_driver
