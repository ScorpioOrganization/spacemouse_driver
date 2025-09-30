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

#include "connection/connection_manager.hpp"

#include <future>
#include <thread>

namespace spacemouse_driver {

ConnectionManager::ConnectionManager(
  std::shared_ptr<DriverContext> context,
  std::shared_ptr<ConnectionMethod> conn_method)
: _context(context),
  _conn_method(conn_method),
  _state(ConnectionState::Disconnected),
  _running(false) {
  _context->logger->debug("ConnectionManager initialized");
}

ConnectionManager::~ConnectionManager() {
  stop();
  if (_state == ConnectionState::Connected) {
    disconnect();
  }
}

bool ConnectionManager::try_connect() {
  if (_state == ConnectionState::Connected) {
    _context->logger->warning("Already connected to a device");
    return true;
  }

  auto device = _conn_method->connect(_context);
  if (!device) {
    change_state(ConnectionState::Disconnected);
    return false;
  }

  {
    std::lock_guard<std::mutex> lock(_mutex);
    _device = device;
  }
  change_state(ConnectionState::Connected);
  _context->logger->log("Connected to SpaceMouse device: " + device->get_name());
  return true;
}

void ConnectionManager::start() {
  if (_running) {
    _context->logger->warning("ConnectionManager is already running");
    return;
  }
  _running = true;
  _connect_thread = std::thread(&ConnectionManager::connect_loop, this);
  _context->logger->debug("ConnectionManager started");
}

void ConnectionManager::stop() {
  if (!_running) {
    return;
  }
  _running = false;
  if (_connect_thread.joinable()) {
    _connect_thread.join();
  }

  _context->logger->debug("ConnectionManager stopped");
}

void ConnectionManager::connect_loop() {
  while (_running) {
    std::this_thread::sleep_for(_connect_retry_interval.load());
    if (_state == ConnectionState::Disconnected) {
      try_connect();
    }
  }
}

void ConnectionManager::disconnect() {
  if (_state != ConnectionState::Connected) {
    _context->logger->warning("Not connected to any device");
    return;
  }

  {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_device) {
      _context->logger->log("Disconnecting from SpaceMouse device: " + _device->get_name());
      _context->hid_backend->close(_device);
    }
    _device = nullptr;
  }
  change_state(ConnectionState::Disconnected);
}

ConnectionState ConnectionManager::get_state() const {
  return _state;
}

std::optional<Model> ConnectionManager::get_connected_model() const {
  std::lock_guard<std::mutex> lock(_mutex);
  if (_device) {
    return _device->config.model;
  }
  return std::nullopt;
}

std::shared_ptr<DeviceHandle> ConnectionManager::get_device() const {
  std::lock_guard<std::mutex> lock(_mutex);
  return _device;
}

void ConnectionManager::set_state_change_callback(
  std::function<void(ConnectionState, std::shared_ptr<DeviceHandle>)> callback) {
  std::lock_guard<std::mutex> lock(_mutex);
  _state_change_callback = callback;
}

void ConnectionManager::set_connect_retry_interval(std::chrono::milliseconds interval) {
  _connect_retry_interval = interval;
}

void ConnectionManager::change_state(ConnectionState new_state) {
  if (_state == new_state) {
    return;
  }
  _state = new_state;
  notify_state_change();
}

void ConnectionManager::notify_state_change() {
  std::shared_ptr<DeviceHandle> device_copy;
  std::function<void(ConnectionState, std::shared_ptr<DeviceHandle>)> callback_copy;
  {
    std::lock_guard<std::mutex> lock(_mutex);
    device_copy = _device;
    callback_copy = _state_change_callback;
  }
  if (callback_copy) {
    callback_copy(_state, device_copy);
  }
}

}  // namespace spacemouse_driver
