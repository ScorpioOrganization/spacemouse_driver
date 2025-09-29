#pragma once

#include <memory>
#include <atomic>
#include <functional>
#include <chrono>

#include "spacemouse_driver/input_types.hpp"
#include "spacemouse_driver/connection_state.hpp"
#include "spacemouse_driver/device_model.hpp"

namespace spacemouse_driver {

class ConnectionManager;
class InputProcessor;
class CallbackDispatcher;
class ConnectionMethod;
class DriverContext;
class DeviceHandle;

/**
 * @brief Main driver class for controlling SpaceMouse devices
 *
 * The Driver class provides the primary interface for interacting with SpaceMouse devices.
 * It manages device connections, processes input data, and handles callback registration
 * for real-time input processing.
 * One driver instance can connect to a single SpaceMouse device at a time.
 *
 */
class Driver
{
public:
  Driver(std::shared_ptr<DriverContext> context, std::shared_ptr<ConnectionMethod> conn_method);
  ~Driver();

  // Main control methods

  /**
   * @brief Starts the driver and begins device communication
   *
   * This method initializes the underlying components and starts the connection process.
   * It will attempt to connect to the specified device and start processing input data.
   */
  void run();

  /**
   * @brief Stops the driver and terminates device communication
   *
   * This method gracefully shuts down all driver components and disconnects from the device.
   * It can be called from any thread to stop a running driver.
   */
  void stop();

  // Input access

  /**
   * @brief Reads the current input state from the device
   *
   * Returns the most recent input data
   *
   * @return Current input state including stick position and button states
   */
  Input read_input() const;

  // Callback registration

  /**
   * @brief Registers a callback function for stick input events
   *
   * @param callback Function to call
   * @note Only one stick callback can be registered at a time. Calling it again overrides the previous one.
   */
  void register_stick_callback(std::function<void(StickInput)> callback);

  /**
   * @brief Registers a callback function for specific button events
   *
   * @param button The button to monitor for events
   * @param callback Function to call when the button state changes
   */
  void register_button_callback(Button button, std::function<void(ButtonInput)> callback);

  /**
   * @brief Removes the currently registered stick callback
   */
  void delete_stick_callback();

  /**
   * @brief Removes the callback for a specific button
   *
   * @param button The button to stop monitoring
   */
  void delete_button_callback(Button button);

  // Configuration

  /**
   * @brief Enables or disables instant callback execution
   *
   * When enabled, callbacks are executed immediately upon new input.
   * When disabled, callbacks are executed at intervals specified with set_callback_interval().
   *
   * @param enabled True to enable instant callbacks, false for interval-based
   */
  void set_instant_callbacks(bool enabled);

  /**
   * @brief Sets the interval for callback execution when instant callbacks are disabled
   *
   * @param interval Time interval between callback executions
   */
  void set_callback_interval(std::chrono::milliseconds interval);

  /**
   * @brief Sets the retry interval for connection attempts
   *
   * @param interval Time to wait between connection retry attempts
   * @note Default value is 1000 milliseconds
   */
  void set_connection_retry_interval(std::chrono::milliseconds interval);

  // Status information

  /**
   * @brief Gets the current connection state
   *
   * @return Current state of the device connection
   */
  ConnectionState get_connection_state() const;

  /**
   * @brief Gets the model of the currently connected device
   *
   * @return Model of the connected SpaceMouse device
   */
  Model get_connected_model() const;

private:
  std::shared_ptr<DriverContext> _context;
  std::atomic<bool> _running;

  // Component modules
  std::unique_ptr<ConnectionManager> _connection_manager;
  std::unique_ptr<InputProcessor> _input_processor;
  std::unique_ptr<CallbackDispatcher> _callback_dispatcher;

  void on_connection_state_change(ConnectionState state, std::shared_ptr<DeviceHandle> device);
  void on_new_input(const Input& input, bool error);
};

}  // namespace spacemouse_driver
