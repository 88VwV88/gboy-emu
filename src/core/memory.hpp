#ifndef __CORE_MEMORY_HPP
#define __CORE_MEMORY_HPP

#include "common.hpp"
#include <array>

namespace mpu {
u8 mem_read(u16 mem_addr);
u8 mem_write(u16 mem_addr, u8 value);
/**
 * Memory Management Unit
 * @brief performs memory read/write operations
 */
struct mmu {
  /* 64 KiB of memory*/
  std::array<u8, 64 * 1024> _m__memory;
  auto at(u16 __pos) -> u8 { return _m__memory.at(__pos); }
  auto set_u8(u16 __pos, u8 __value) -> void { _m__memory.at(__pos) = __value; }
  auto set_u16(u16 __pos, u16 __value) -> void {
    _m__memory.at(__pos) = as<u8>(__value & 0x00FF);
    _m__memory.at(__pos + 1) = as<u8>((__value & 0xFF00) >> 8);
  }
};
union sprite {
  u32 data;
  struct {
    u8 m_x;
    u8 m_y;
    u8 m_tile;
    bool m_flip;
  };
};
struct ppu {
  std::array<sprite, 40> m_oam;
};
}; // namespace mpu

#endif