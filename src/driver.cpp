#include "spacemouse_driver/driver.hpp"

#include "driver/driver_context.hpp"
#include "connection/connection_manager.hpp"
#include "input/input_processor.hpp"
#include "input/callback_dispatcher.hpp"

namespace spacemouse_driver {

Driver::Driver(
  std::shared_ptr<DriverContext> context,
  std::shared_ptr<ConnectionMethod> conn_method)
: _context(context),
  _running(false) {
  _connection_manager = std::make_unique<ConnectionManager>(context, conn_method);
  _input_processor = std::make_unique<InputProcessor>(context);
  _callback_dispatcher = std::make_unique<CallbackDispatcher>(context);

  _connection_manager->set_state_change_callback(
    std::bind(
      &Driver::on_connection_state_change, this, std::placeholders::_1,
      std::placeholders::_2)
  );

  _input_processor->set_data_callback(
    std::bind(&Driver::on_new_input, this, std::placeholders::_1, std::placeholders::_2)
  );

  _context->logger->debug("Driver initialized successfully");
}

Driver::~Driver() {
  stop();
}

void Driver::run() {
  if (_running) {
    _context->logger->warning("Driver is already running.");
    return;
  }

  _running = true;

  _callback_dispatcher->start();
  _input_processor->start();
  _connection_manager->start();

  _context->logger->log("Driver started");
}

void Driver::stop() {
  if (!_running) {
    _context->logger->warning("Driver is not running.");
    return;
  }

  _running = false;

  _input_processor->stop();
  _callback_dispatcher->stop();

  if (_connection_manager->get_state() == ConnectionState::Connected) {
    _connection_manager->disconnect();
  }

  _context->logger->log("Driver stopped");
}

Input Driver::read_input() const {
  return _input_processor->get_latest_input();
}

void Driver::register_stick_callback(std::function<void(StickInput)> callback) {
  _callback_dispatcher->register_stick_callback(callback);
}

void Driver::register_button_callback(Button button, std::function<void(ButtonInput)> callback) {
  _callback_dispatcher->register_button_callback(button, callback);
}

void Driver::delete_stick_callback() {
  _callback_dispatcher->delete_stick_callback();
}

void Driver::delete_button_callback(Button button) {
  _callback_dispatcher->delete_button_callback(button);
}

void Driver::set_callback_interval(std::chrono::milliseconds interval) {
  _callback_dispatcher->set_callback_interval(interval);
}

void Driver::set_connection_retry_interval(std::chrono::milliseconds interval) {
  _connection_manager->set_connect_retry_interval(interval);
}

void Driver::set_instant_callbacks(bool enabled) {
  _callback_dispatcher->set_instant_callbacks(enabled);
}

ConnectionState Driver::get_connection_state() const {
  return _connection_manager->get_state();
}

void Driver::on_connection_state_change(ConnectionState state, std::shared_ptr<DeviceHandle> device) {
  if (state == ConnectionState::Connected) {
    _input_processor->set_device(device);
  } else if (state == ConnectionState::Disconnected) {
    _callback_dispatcher->process_input(Input{ });
    _input_processor->clear_device();
  }
}

void Driver::on_new_input(const Input& input, bool error) {
  // Input error = device disconnected
  if (error && _connection_manager->get_state() == ConnectionState::Connected) {
    _context->logger->debug("Failed to read input data from the device, disconnecting");
    _connection_manager->disconnect();
    return;
  }

  _callback_dispatcher->process_input(input);
}

}  // namespace spacemouse_driver
