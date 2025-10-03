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

#pragma once

#include <set>
#include <string>
#include <mutex>

#include "types/device_types.hpp"

namespace spacemouse_driver {

class SharedDeviceManager
{
public:
  SharedDeviceManager()
  : _claimed_paths() { }

  bool claim_path(const std::string& path) {
    std::lock_guard<std::mutex> lock(_mutex);
    return _claimed_paths.insert(path).second;
  }

  void release_path(const std::string& path) {
    std::lock_guard<std::mutex> lock(_mutex);
    _claimed_paths.erase(path);
  }

private:
  std::set<std::string> _claimed_paths;
  std::mutex _mutex;
};

}  // namespace spacemouse_driver
