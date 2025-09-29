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
