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
#include <functional>
#include <chrono>
#include <thread>

#include "driver/driver_context.hpp"
#include "connection/connection_method.hpp"
#include "spacemouse_driver/connection_state.hpp"

namespace spacemouse_driver {

class ConnectionManager
{
public:
  ConnectionManager(
    std::shared_ptr<DriverContext> context,
    std::shared_ptr<ConnectionMethod> conn_method);
  ~ConnectionManager();

  // Connection management
  void disconnect();
  ConnectionState get_state() const;
  std::optional<Model> get_connected_model() const;
  std::shared_ptr<DeviceHandle> get_device() const;

  // Connection thread management
  void start();
  void stop();

  // Connection state notification
  void set_state_change_callback(
    std::function<void(ConnectionState,
    std::shared_ptr<DeviceHandle>)> callback);

  // Config
  void set_connect_retry_interval(std::chrono::milliseconds interval);

private:
  std::shared_ptr<DriverContext> _context;
  std::shared_ptr<ConnectionMethod> _conn_method;
  std::shared_ptr<DeviceHandle> _device;
  std::atomic<ConnectionState> _state;
  mutable std::mutex _mutex;

  // Connection management
  bool try_connect();

  // Connection thread management
  std::atomic_bool _running;
  std::thread _connect_thread;
  void connect_loop();

  // Config
  std::atomic<std::chrono::milliseconds> _connect_retry_interval{ std::chrono::milliseconds(1000) };

  // Notification callback
  std::function<void(ConnectionState, std::shared_ptr<DeviceHandle>)> _state_change_callback;

  // State changes
  void change_state(ConnectionState new_state);
  void notify_state_change();
};

}  // namespace spacemouse_driver
