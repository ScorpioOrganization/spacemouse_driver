# C++ SpaceMouse Driver Library

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![CMake](https://img.shields.io/badge/CMake-3.10+-blue.svg)](https://cmake.org/)

A modern C++ library for interfacing with 3Dconnexion SpaceMouse devices on Linux systems. This library provides a clean, event-driven API for reading input data from SpaceMouse devices.

## ✨ Features

- **Event-driven Architecture**: Utilizes callbacks for real-time input processing
- **Hot Plugging**: Automatically handles device connection and disconnection
- **Multi-device Support**: Can manage multiple SpaceMouse devices simultaneously
- **Flexible Connection**: Multiple connection methods, including automatic model detection and manual device path specification

## 🚀 Quick Start

### Basic Usage

```cpp
#include <spacemouse_driver/spacemouse_driver.hpp>
#include <thread>

using namespace spacemouse_driver;

int main() {
    // Create driver manager
    auto manager = std::make_unique<DriverManager>();
    
    // Create driver instance
    auto driver = manager->create_driver();
    
    // Register stick movement callback
    driver->register_stick_callback([](const StickInput& input) {
        // Handle stick input
    });
    
    // Register button callback
    driver->register_button_callback(Button::Button1, [](ButtonInput pressed) {
        // Handle button input
    });
    
    // Start the driver
    driver->run();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}
```

## 🛠️ Building and setup

### Prerequisites

- **Compiler**: GCC or Clang with C++17 support
- **CMake**: Version 3.10 or higher
- **Hidapi**: 
  - Ubuntu/Debian - `libhidapi-dev`

### Ubuntu/Debian

```bash
sudo apt update
sudo apt install build-essential cmake pkg-config libhidapi-dev
```

### Build and Install

```bash
# Clone the repository
git clone https://github.com/ScorpioOrganization/spacemouse_driver.git
cd spacemouse_driver

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
cmake --build .

# Install as system library
sudo cmake --install .

# Update shared library cache
sudo ldconfig
```

### Script setup

Inside the repository, you will find a `setup.sh` script that automates the configuration process, which is mandatory for proper operation of the library. Note that the script must be run with root priviliges.

#### The script will:
- Install udev rules that handle the device permissions, which is neccessary to access them without root permissions. It will also disable Linux's generic mouse driver for SpaceMouse devices
- Create and add current user to the `hidraw` group

### Manual setup

If you prefer to not run the script you can setup the environment yourself.

#### Install udev rules and reload them

```bash
sudo cp udev/60-spacemouse.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
sudo udevadm trigger
```

#### Create `hidraw` group and add yourself to it

```bash
sudo groupadd hidraw
sudo usermod -a -G hidraw $USER
# Log out and log back in for the group change to take effect
```

## 🔧 CMake Integration

To use the library in your CMake project:

```cmake
find_package(spacemouse_driver REQUIRED)
target_link_libraries(your_target spacemouse_driver)
```

## 🕹️ Supported Devices

| Model | Product ID | Features |
|-------|------------|----------|
| SpaceMouse Enterprise | 0xC633 | 6DOF + 31 buttons |
| SpaceMouse Wireless | 0xC63A / 0xC652 | 6DOF + 2 buttons |


## 📚 API Documentation

You can find API documentation in form of Doxygen comments within the public headers.
