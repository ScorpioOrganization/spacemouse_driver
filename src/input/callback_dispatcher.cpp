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

#include "input/callback_dispatcher.hpp"

#include "driver/driver_context.hpp"

namespace spacemouse_driver {

CallbackDispatcher::CallbackDispatcher(std::shared_ptr<DriverContext> context)
: _context(context),
  _running(false),
  _new_input(false),
  _zero_state_reported(false),
  _instant_callbacks(false) {
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
  _dispatch_thread = std::thread(
    [this]() {
      dispatch_loop();
    });
  _context->logger->debug("CallbackDispatcher started");
}

void CallbackDispatcher::stop() {
  if (!_running) {
    return;
  }

  _running = false;
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

void CallbackDispatcher::register_button_callback(
  Button button,
  std::function<void(ButtonInput)> callback) {
  std::lock_guard<std::mutex> lock(_callback_mutex);
  _button_callbacks[*magic_enum::enum_index(button)] = callback;
}

void CallbackDispatcher::delete_stick_callback() {
  std::lock_guard<std::mutex> lock(_callback_mutex);
  _stick_callback = nullptr;
}

void CallbackDispatcher::delete_button_callback(Button button) {
  std::lock_guard<std::mutex> lock(_callback_mutex);
  _button_callbacks[*magic_enum::enum_index(button)] = nullptr;
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
      _input_cv.wait_for(
        lock, callback_interval, [this] {
          return !_running || (_new_input && _instant_callbacks);
        });

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
      Button button = magic_enum::enum_value<Button>(i);
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
    callback = _button_callbacks[*magic_enum::enum_index(button)];
  }

  if (callback) {
    callback(input);
  }
}

}  // namespace spacemouse_driver
