#include "input/callback_dispatcher.hpp"

#include "driver/driver_context.hpp"

namespace spacemouse_driver {

CallbackDispatcher::CallbackDispatcher(std::shared_ptr<DriverContext> context)
: _context(context) {
  _context->logger->debug("CallbackDispatcher initialized");
}

CallbackDispatcher::~CallbackDispatcher() {
  stop();
}

void CallbackDispatcher::start() {
  if (_running) {
    _context->logger->warning("CallbackDispatcher is already running");
    return;
  }

  _running = true;
  _dispatch_thread = std::thread([this]() {
        dispatch_loop();
    });
  _context->logger->debug("CallbackDispatcher started");
}

void CallbackDispatcher::stop() {
  if (!_running) {
    return;
  }

  _running = false;
  {
    std::lock_guard<std::mutex> lock(_input_mutex);
    _new_input = true;
  }
  _input_cv.notify_all();

  if (_dispatch_thread.joinable()) {
    _dispatch_thread.join();
  }
  _context->logger->debug("CallbackDispatcher stopped");
}

void CallbackDispatcher::process_input(const Input& input) {
  {
    std::lock_guard<std::mutex> lock(_input_mutex);
    _current_input = input;
    _new_input = true;
  }
  if (_instant_callbacks) {
    _input_cv.notify_all();
  }
}

void CallbackDispatcher::register_stick_callback(std::function<void(StickInput)> callback) {
  std::lock_guard<std::mutex> lock(_callback_mutex);
  _stick_callback = callback;
}

void CallbackDispatcher::register_button_callback(Button button, std::function<void(ButtonInput)> callback) {
  std::lock_guard<std::mutex> lock(_callback_mutex);
  _button_callbacks[static_cast<size_t>(button)] = callback;
}

void CallbackDispatcher::delete_stick_callback() {
  std::lock_guard<std::mutex> lock(_callback_mutex);
  _stick_callback = nullptr;
}

void CallbackDispatcher::delete_button_callback(Button button) {
  std::lock_guard<std::mutex> lock(_callback_mutex);
  _button_callbacks[static_cast<size_t>(button)] = nullptr;
}

void CallbackDispatcher::set_callback_interval(std::chrono::milliseconds interval) {
  _callback_interval = interval;
}

void CallbackDispatcher::set_instant_callbacks(bool enabled) {
  _instant_callbacks = enabled;
}

void CallbackDispatcher::dispatch_loop() {
  while (_running) {
    Input input_to_process;

    {
      std::unique_lock<std::mutex> lock(_input_mutex);
      auto callback_interval = _callback_interval.load();
      _input_cv.wait_for(lock, callback_interval, [this] { return !_running || (_new_input && _instant_callbacks); });

      if (!_running) { break; }

      if (_new_input) {
        input_to_process = _current_input;
        _new_input = false;
      } else {
        continue;
      }
    }

    // Process button callbacks
    for (size_t i = 0; i < ButtonCount; ++i) {
      Button button = static_cast<Button>(i);
      if (input_to_process.buttons[i] != _prev_input.buttons[i]) {
        invoke_button_callback(button, input_to_process.buttons[i]);
      }
    }

    // Process stick callbacks
    if (input_to_process.stick == StickInput{ }) {
      if (!_zero_state_reported) {
        invoke_stick_callback(StickInput{ });
        _zero_state_reported = true;
      }
    } else {
      invoke_stick_callback(input_to_process.stick);
      _zero_state_reported = false;
    }

    _prev_input = input_to_process;
  }
}

void CallbackDispatcher::invoke_stick_callback(const StickInput& input) {
  std::function<void(StickInput)> callback;
  {
    std::lock_guard<std::mutex> lock(_callback_mutex);
    callback = _stick_callback;
  }

  if (callback) {
    callback(input);
  }
}

void CallbackDispatcher::invoke_button_callback(Button button, ButtonInput input) {
  std::function<void(ButtonInput)> callback;
  {
    std::lock_guard<std::mutex> lock(_callback_mutex);
    callback = _button_callbacks[static_cast<size_t>(button)];
  }

  if (callback) {
    callback(input);
  }
}

}  // namespace spacemouse_driver
