#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <mutex>

namespace spacemouse_driver {

/**
 * @brief Enumeration of logging levels
 *
 * Defines the severity levels for log messages. Lower numeric values
 * represent higher priority messages.
 */
enum class LogLevel
{
  Error = 0,
  Warning = 1,
  Info = 2,
  Debug = 3
};

/**
 * @brief Abstract base class for logging implementations
 *
 * Provides the interface for logging operations within the SpaceMouse driver.
 * Implementations should be thread-safe as they may be called from multiple threads.
 */
class Logger {
protected:
  std::mutex _log_mutex;
  LogLevel _log_level = LogLevel::Info;

public:
  virtual ~Logger() = default;

  /**
   * @brief Sets the minimum logging level
   *
   * Only messages at or above this level will be logged.
   *
   * @param level New minimum logging level
   */
  void set_log_level(LogLevel level) { _log_level = level; }

  LogLevel get_log_level() const { return _log_level; }

  /**
   * @brief Logs a message with specified level
   *
   * @param message Message to log
   * @param level Severity level of the message
   */
  virtual void log(const std::string& message, LogLevel level = LogLevel::Info) = 0;

  /**
   * @brief Logs a warning message
   *
   * @param message Warning message to log
   */
  virtual void warning(const std::string& message) = 0;

  /**
   * @brief Logs an error message
   *
   * @param message Error message to log
   */
  virtual void error(const std::string& message) = 0;

  /**
   * @brief Logs a debug message
   *
   * @param message Debug message to log
   */
  virtual void debug(const std::string& message) = 0;
};

/**
 * @brief Console-based logger implementation
 *
 * Outputs log messages to standard output (Info, Debug) and standard error (Warning, Error).
 */
class ConsoleLogger : public Logger {
public:
  ConsoleLogger() = default;

  /**
   * @brief Logs a message to console with appropriate formatting
   *
   * @param message Message to log
   * @param level Severity level of the message
   */
  void log(const std::string& message, LogLevel level = LogLevel::Info) override {
    std::lock_guard<std::mutex> lock(_log_mutex);
    if (level <= _log_level) {
      switch (level) {
        case LogLevel::Error:
          std::cerr << "[ERROR] : " << message << std::endl;
          break;
        case LogLevel::Warning:
          std::cerr << "[WARNING] : " << message << std::endl;
          break;
        case LogLevel::Info:
          std::cout << "[INFO] : " << message << std::endl;
          break;
        case LogLevel::Debug:
          std::cout << "[DEBUG] : " << message << std::endl;
          break;
      }
    }
  }

  void warning(const std::string& message) override {
    log(message, LogLevel::Warning);
  }

  void error(const std::string& message) override {
    log(message, LogLevel::Error);
  }

  void debug(const std::string& message) override {
    log(message, LogLevel::Debug);
  }
};

}  // namespace spacemouse_driver
