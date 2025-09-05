#pragma once

#include <memory>
#include <atomic>
#include <functional>
#include <chrono>
#include <thread>

#include "driver/driver_context.hpp"
#include "connection/connection_method.hpp"

namespace spacemouse_driver {

enum class ConnectionState
{
  Disconnected,
  Connected,
};

class ConnectionManager {
public:
  ConnectionManager(
    std::shared_ptr<DriverContext> context,
    std::shared_ptr<ConnectionMethod> conn_method);
  ~ConnectionManager();

  // Connection management
  void disconnect();
  ConnectionState get_state() const;
  std::shared_ptr<DeviceHandle> get_device();

  // Connection thread management
  void start();
  void stop();

  // Connection state notification
  void set_state_change_callback(std::function<void(ConnectionState, std::shared_ptr<DeviceHandle>)> callback);

  // Config
  void set_connect_retry_interval(std::chrono::milliseconds interval);

private:
  std::shared_ptr<DriverContext> _context;
  std::shared_ptr<ConnectionMethod> _conn_method;
  std::shared_ptr<DeviceHandle> _device;
  std::atomic<ConnectionState> _state{ ConnectionState::Disconnected };
  std::mutex _mutex;

  // Connection management
  bool try_connect();

  // Connection thread management
  std::atomic_bool _running{ false };
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
