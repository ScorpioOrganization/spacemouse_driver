#pragma once

#include <vector>
#include <memory>
#include <string>

#include "spacemouse_driver/logger.hpp"

namespace spacemouse_driver
{

class SharedDeviceManager;
class Driver;
class DriverContext;
class ConnectionMethod;
class Logger;
enum class Model;

/**
 * @brief Factory class for creating and managing SpaceMouse drivers
 *
 * The DriverManager provides a high-level interface for creating Driver instances
 * with various configuration options. It handles the underlying setup of device
 * detection, connection methods, and shared resources.
 *
 * Example
 * ```cpp
 * // Create a driver manager with console logging
 * auto manager = std::make_unique<DriverManager>();
 *
 * // Create a driver for any available SpaceMouse device
 * auto driver = manager->create_driver();
 *
 * // Start the driver
 * driver->run();
 * ```
 */
class DriverManager
{
public:
  /**
   * @brief Constructs a DriverManager with default console logging and Warning log level
   */
  DriverManager();

  ~DriverManager();

  /**
   * @brief Constructs a DriverManager with custom logger
   *
   * @param logger Custom logger implementation to use for driver operations
   * @param log_level Initial logging level to set
   */
  explicit DriverManager(std::unique_ptr<Logger> logger, LogLevel log_level = LogLevel::Warning);

  /**
   * @brief Sets the logging level for all driver operations
   *
   * @param level New logging level to apply
   */
  void set_log_level(LogLevel level);

  /**
   * @brief Creates a driver for any available SpaceMouse device
   *
   * Automatically detects and connects to the first available SpaceMouse device.
   *
   * @return Shared pointer to the created driver
   */
  std::shared_ptr<Driver> create_driver();

  /**
   * @brief Creates a driver for devices matching specific models
   *
   * Searches for devices matching any of the specified models, in the provided order.
   * The first matching device is used.
   *
   * @param model_list List of device models to search for
   * @return Shared pointer to the created driver
   */
  std::shared_ptr<Driver> create_driver(const std::vector<Model> & model_list);

  /**
   * @brief Creates a driver for a specific device model
   *
   * Searches for any device matching the specified model.
   *
   * @param device_model Specific device model to search for
   * @return Shared pointer to the created driver
   */
  std::shared_ptr<Driver> create_driver(Model device_model);

  /**
   * @brief Creates a driver for a device at a specific path
   *
   * Directly connects to a device using its system hidraw path (e.g., /dev/hidraw0).
   *
   * @param device_path System path to the device
   * @return Shared pointer to the created driver
   */
  std::shared_ptr<Driver> create_driver(const std::string & device_path);

private:
  std::shared_ptr<DriverContext> _context;
  std::vector<std::shared_ptr<Driver>> _drivers;

  std::shared_ptr<Driver> make_driver(const std::shared_ptr<ConnectionMethod> & conn_method);
};

}  // namespace spacemouse_driver
