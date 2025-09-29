#pragma once

#include <memory>
#include <utility>

#include "connection/hid_backend.hpp"
#include "spacemouse_driver/logger.hpp"

namespace spacemouse_driver {

struct DriverContext {
  std::unique_ptr<HidBackend> hid_backend;
  std::unique_ptr<Logger> logger;

  DriverContext(
    std::unique_ptr<HidBackend> hid_backend,
    std::unique_ptr<Logger> logger)
  : hid_backend(std::move(hid_backend)),
    logger(std::move(logger)) { }
};

}  // namespace spacemouse_driver
