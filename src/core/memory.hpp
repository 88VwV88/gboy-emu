#ifndef __CORE_MEMORY_HPP
#define __CORE_MEMORY_HPP

#include <array>
#include "common.hpp"

namespace mpu {
struct memory {
  /* 64 KiB of memory*/
  std::array<u8, 64 * 1024> _m__memory;
  constexpr auto at(unsigned __pos) -> u8 { return _m__memory.at(__pos); }
};
}; // namespace mpu

#endif