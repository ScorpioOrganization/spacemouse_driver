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

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <iterator>

#include "types/device_types.hpp"
#include "driver/driver_context.hpp"

namespace spacemouse_driver {

class ConnectionMethod
{
public:
  virtual std::shared_ptr<DeviceHandle> connect(std::shared_ptr<DriverContext> context) = 0;
  virtual ~ConnectionMethod() = default;
};

class ModelListConnectionMethod : public ConnectionMethod
{
public:
  explicit ModelListConnectionMethod(const std::vector<Model>& model_list);
  std::shared_ptr<DeviceHandle> connect(std::shared_ptr<DriverContext> context) override;

private:
  std::vector<Model> _model_list;
};

class PathConnectionMethod : public ConnectionMethod
{
public:
  explicit PathConnectionMethod(const std::string& path);
  std::shared_ptr<DeviceHandle> connect(std::shared_ptr<DriverContext> context) override;

private:
  std::string _path;
};

class AnyModelConnectionMethod : public ConnectionMethod
{
public:
  AnyModelConnectionMethod();
  std::shared_ptr<DeviceHandle> connect(std::shared_ptr<DriverContext> context) override;
};

}  // namespace spacemouse_driver
