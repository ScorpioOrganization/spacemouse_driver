#include "input/input_processor.hpp"

#include "driver/driver_context.hpp"

namespace spacemouse_driver
{

InputProcessor::InputProcessor(std::shared_ptr<DriverContext> context)
: _context(context),
  _last_data_time(std::chrono::steady_clock::now())
{
  _context->logger->debug("InputProcessor initialized");
}

InputProcessor::~InputProcessor()
{
  stop();
}

void InputProcessor::start()
{
  if (_running) {
    _context->logger->warning("InputProcessor is already running");
    return;
  }

  _running = true;
  _process_thread = std::thread(
    [this]() {
      process_loop();
    });
  _context->logger->debug("InputProcessor started");
}

void InputProcessor::stop()
{
  if (!_running) {
    return;
  }

  _running = false;
  if (_process_thread.joinable()) {
    _process_thread.join();
  }
  _context->logger->debug("InputProcessor stopped");
}

void InputProcessor::set_device(std::shared_ptr<DeviceHandle> device)
{
  std::lock_guard<std::mutex> lock(_device_mutex);
  _device = device;
  _last_data_time = std::chrono::steady_clock::now();
}

void InputProcessor::clear_device()
{
  std::lock_guard<std::mutex> lock(_device_mutex);
  _device = nullptr;
  _last_input.write(Input{});
}

Input InputProcessor::get_latest_input() const
{
  return _last_input.read();
}

void InputProcessor::set_data_callback(DataCallback callback)
{
  std::lock_guard<std::mutex> lock(_callback_mutex);
  _data_callback = callback;
}

void InputProcessor::process_loop()
{
  uint8_t buf[BUFFER_SIZE];

  while (_running) {
    std::shared_ptr<DeviceHandle> current_device;
    {
      std::lock_guard<std::mutex> lock(_device_mutex);
      current_device = _device;
    }

    if (!current_device) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }

    int res = _context->hid_backend->read(current_device, buf, BUFFER_SIZE);

    if (res < 0) {
      // Read error = disconnected
      _context->logger->debug("Read error from device");

      DataCallback callback;
      {
        std::lock_guard<std::mutex> lock(_callback_mutex);
        callback = _data_callback;
      }

      if (callback) {
        Input error_input;
        bool error = true;
        callback(error_input, error);
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }

    if (res == 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue;
    }

    _last_data_time = std::chrono::steady_clock::now();

    Input curr_input = parse(buf, static_cast<size_t>(res), current_device->config);
    _last_input.write(curr_input);

    DataCallback callback;
    {
      std::lock_guard<std::mutex> lock(_callback_mutex);
      callback = _data_callback;
    }

    if (callback) {
      callback(curr_input, false);
    }
  }
}

Input InputProcessor::parse(const uint8_t * data, size_t length, const DeviceConfig & config) const
{
  Input input;

  // Parse axis data
  for (size_t i = 0; i < AxisCount; ++i) {
    auto mapping = config.get_axis_mapping(static_cast<Axis>(i));
    auto raw_data = mapping.parse(data, length);
    if (!raw_data) {continue;}
    double normalized_value = static_cast<double>(raw_data.value()) / config.axis_div;
    input.stick.axis[i] = normalized_value;
  }

  // Parse button data
  Input last_input = _last_input.read();
  for (size_t i = 0; i < ButtonCount; ++i) {
    auto mapping = config.get_button_mapping(static_cast<Button>(i));
    if (!mapping) {
      input.buttons[i] = false;
      continue;
    }
    auto is_pressed = std::visit(
      [&](const auto & mapping) {
        return mapping.parse(data, length);
      }, *mapping);

    if (!is_pressed.has_value()) {
      input.buttons[i] = last_input.buttons[i];
      continue;
    }
    input.buttons[i] = is_pressed.value();
  }

  return input;
}

}  // namespace spacemouse_driver
