#pragma once

#include <memory>
#include <atomic>
#include <functional>
#include <chrono>

#include "spacemouse_driver/input_types.hpp"

namespace spacemouse_driver {

class ConnectionManager;
class InputProcessor;
class CallbackDispatcher;
class ConnectionMethod;
class DriverContext;
class DeviceHandle;
enum class ConnectionState;

class Driver {
public:
  Driver(std::shared_ptr<DriverContext> context, std::shared_ptr<ConnectionMethod> conn_method);
  ~Driver();

  // Main control methods
  void run();
  void stop();

  // Input access
  Input read_input() const;

  // Callback registration (forwarded to CallbackDispatcher)
  void register_stick_callback(std::function<void(StickInput)> callback);
  void register_button_callback(Button button, std::function<void(ButtonInput)> callback);
  void delete_stick_callback();
  void delete_button_callback(Button button);

  // Configuration
  void set_instant_callbacks(bool enabled);
  void set_callback_interval(std::chrono::milliseconds interval);
  void set_connection_retry_interval(std::chrono::milliseconds interval);

  // Status information
  ConnectionState get_connection_state() const;

private:
  std::shared_ptr<DriverContext> _context;
  std::atomic<bool> _running{ false };

  // Component modules
  std::unique_ptr<ConnectionManager> _connection_manager;
  std::unique_ptr<InputProcessor> _input_processor;
  std::unique_ptr<CallbackDispatcher> _callback_dispatcher;

  // Handle connection state changes
  void on_connection_state_change(ConnectionState state, std::shared_ptr<DeviceHandle> device);

  // Handle new input data
  void on_new_input(const Input& input, bool error);
};

}  // namespace spacemouse_driver
