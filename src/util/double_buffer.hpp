#pragma once

#include <array>
#include <atomic>
#include <cstddef>

namespace spacemouse_driver
{

template<typename T>
class DoubleBuffer
{
public:
  DoubleBuffer()
  : _active_idx(0) {}

  void write(const T & value)
  {
    size_t next_idx = 1 - _active_idx.load(std::memory_order_relaxed);
    _buffers[next_idx] = value;
    _active_idx.store(next_idx, std::memory_order_release);
  }

  T read() const
  {
    size_t cur_idx = _active_idx.load(std::memory_order_acquire);
    return _buffers[cur_idx];
  }

private:
  std::array<T, 2> _buffers;
  std::atomic<size_t> _active_idx;
};

}  // namespace spacemouse_driver
