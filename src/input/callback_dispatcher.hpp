#pragma once

#include <memory>
#include <atomic>
#include <thread>
#include <functional>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <array>

#include "spacemouse_driver/input_types.hpp"

namespace spacemouse_driver {

class DriverContext;

class CallbackDispatcher {
public:
  explicit CallbackDispatcher(std::shared_ptr<DriverContext> context);
  ~CallbackDispatcher();

  // Thread control
  void start();
  void stop();

  // Process new input data
  void process_input(const Input& input);

  // Callback registration
  void register_stick_callback(std::function<void(StickInput)> callback);
  void register_button_callback(Button button, std::function<void(ButtonInput)> callback);
  void delete_stick_callback();
  void delete_button_callback(Button button);

  // Config
  void set_callback_interval(std::chrono::milliseconds interval);
  void set_instant_callbacks(bool enabled);

private:
  std::shared_ptr<DriverContext> _context;
  std::atomic<bool> _running{ false };
  std::thread _dispatch_thread;

  // Callback handling
  std::mutex _callback_mutex;
  std::function<void(StickInput)> _stick_callback;
  std::array<std::function<void(ButtonInput)>, ButtonCount> _button_callbacks;

  // Input data
  std::mutex _input_mutex;
  Input _current_input;
  Input _prev_input;
  std::condition_variable _input_cv;
  bool _new_input{ false };
  bool _zero_state_reported{ false };

  // Config
  std::atomic<std::chrono::milliseconds> _callback_interval{ std::chrono::milliseconds(20) };
  std::atomic_bool _instant_callbacks{ false };

  // Main dispatch loop
  void dispatch_loop();

  // Helpers
  void invoke_stick_callback(const StickInput& input);
  void invoke_button_callback(Button button, ButtonInput input);
};

}  // namespace spacemouse_driver
