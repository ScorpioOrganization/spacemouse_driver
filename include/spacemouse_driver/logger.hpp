#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <mutex>

namespace spacemouse_driver {

enum class LogLevel
{
  Error = 0,
  Warning = 1,
  Info = 2,
  Debug = 3
};

class Logger {
protected:
  std::mutex _log_mutex;
  LogLevel _log_level = LogLevel::Info;

public:
  virtual ~Logger() = default;
  void set_log_level(LogLevel level) { _log_level = level; }
  LogLevel get_log_level() const { return _log_level; }
  virtual void log(const std::string& message, LogLevel level = LogLevel::Info) = 0;
  virtual void warning(const std::string& message) = 0;
  virtual void error(const std::string& message) = 0;
  virtual void debug(const std::string& message) = 0;
};

class ConsoleLogger : public Logger {
public:
  ConsoleLogger() = default;

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
