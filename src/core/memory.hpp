#ifndef __CORE_MEMORY_HPP
#define __CORE_MEMORY_HPP

#include <array>
#include "common.hpp"

namespace mpu {
struct memory_bus {
  /* 64 KiB of memory*/
  std::array<u8, 64 * 1024> _m__memory;
  constexpr auto at(u16 __pos) -> u8 { return _m__memory.at(__pos); }
  auto set_u8(u16 __pos, u8 __value) -> void { _m__memory.at(__pos) = __value; }
  auto set_u16(u16 __pos, u16 __value) -> void {
    _m__memory.at(__pos) = as<u8>(__value & 0x00FF);
    _m__memory.at(__pos + 1) = as<u8>((__value & 0xFF00) >> 8);
  }
};
}; // namespace mpu

#endif