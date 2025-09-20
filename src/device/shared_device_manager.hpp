#pragma once
#include <set>
#include <string>
#include <mutex>

#include "types/device_types.hpp"

namespace spacemouse_driver
{

class SharedDeviceManager
{
public:
  SharedDeviceManager()
  : _claimed_paths() {}

  bool claim_path(const std::string & path)
  {
    std::lock_guard<std::mutex> lock(_mutex);
    return _claimed_paths.insert(path).second;
  }

  void release_path(const std::string & path)
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _claimed_paths.erase(path);
  }

private:
  std::set<std::string> _claimed_paths;
  std::mutex _mutex;
};

}  // namespace spacemouse_driver
