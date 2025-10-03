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

using DataCallback = std::function<void (const Input&, bool error)>;

class InputProcessor
{
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
  std::atomic<bool> _running;
  std::thread _process_thread;

  // Device and data
  std::mutex _device_mutex;
  std::shared_ptr<DeviceHandle> _device;
  DoubleBuffer<Input> _last_input;

  // Config
  std::atomic<std::chrono::milliseconds> _data_timeout;

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
