#pragma once

#include <memory>
#include <atomic>
#include <thread>
#include <functional>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "util/double_buffer.hpp"
#include "spacemouse_driver/input_types.hpp"
#include "driver/driver_context.hpp"

namespace spacemouse_driver {

using DataCallback = std::function<void(const Input&, bool error)>;

class InputProcessor {
public:
  explicit InputProcessor(std::shared_ptr<DriverContext> context);
  ~InputProcessor();

  // Thread control
  void start();
  void stop();

  // Device management
  void set_device(std::shared_ptr<DeviceHandle> device);
  void clear_device();

  // Data access
  Input get_latest_input() const;

  // Callback for new data
  void set_data_callback(DataCallback callback);

private:
  std::shared_ptr<DriverContext> _context;
  std::atomic<bool> _running{ false };
  std::thread _process_thread;

  // Device and data
  std::mutex _device_mutex;
  std::shared_ptr<DeviceHandle> _device;
  DoubleBuffer<Input> _last_input;
  std::chrono::steady_clock::time_point _last_data_time;

  // Config
  std::atomic<std::chrono::milliseconds> _data_timeout{ std::chrono::milliseconds(1000) };

  // Callback for new data
  std::mutex _callback_mutex;
  DataCallback _data_callback;

  // Buffer for read operations
  static constexpr size_t BUFFER_SIZE = 64;

  // Processing function
  void process_loop();

  // Input parsing
  Input parse(const uint8_t* data, size_t length, const DeviceConfig& config) const;
};

}  // namespace spacemouse_driver
