#ifndef __CORE_MEMORY_HPP
#define __CORE_MEMORY_HPP

#include "common.hpp"
#include <array>
#include <cstddef>

namespace mpu {

/**
 * Memory Management Unit
 * @brief performs memory read/write operations
 */
struct mmu {
  // 64 KiB of memory
  std::array<u8, 0x4000> rom_bank0 {};   // 0x0000-3FFF
  std::array<u8, 0x4000> rom_bankn {};   // 0x4000-7FFF
  std::array<u8, 0x2000> vram {};        // 0x8000-9FFF
  std::array<u8, 0x2000> eram {};        // 0xA000-BFFF
  std::array<u8, 0x1000> wram0 {};       // 0xC000-CFFF
  std::array<u8, 0x1000> wram1 {};       // 0xD000-DFFF
  std::array<u8, 0xA0>   oam {};         // 0xFE00-FE9F
  std::array<u8, 0x80>   io_regs {};     // 0xFF00-FF7F
  std::array<u8, 0x7F>   hram {};        // 0xFF80-FFFE
  u8 interrupt_enable = 0;               // 0xFFFF

  // Read
  u8 at(u16 addr) const {
    if (addr < 0x4000) {
      return rom_bank0[addr];
    } else if (addr < 0x8000) {
      return rom_bankn[addr - 0x4000];
    } else if (addr < 0xA000) {
      return vram[addr - 0x8000];
    } else if (addr < 0xC000) {
      return eram[addr - 0xA000];
    } else if (addr < 0xD000) {
      return wram0[addr - 0xC000];
    } else if (addr < 0xE000) {
      return wram1[addr - 0xD000];
    } else if (addr < 0xFE00) {
      return at(addr - 0x2000);
    } else if (addr < 0xFEA0) {
      return oam[addr - 0xFE00];
    } else if (addr < 0xFF00) {
      return 0xFF;
    } else if (addr < 0xFF80) {
      return io_regs[addr - 0xFF00];
    } else if (addr < 0xFFFF) {
      return hram[addr - 0xFF80];
    } else if (addr == 0xFFFF) {
      return interrupt_enable;
    }
    return 0xFF;
  }

  // Write
  void set_u8(u16 addr, u8 value) {
    if (addr < 0x4000) {
      // ROM bank 1 not writable in gboy
    } else if (addr < 0x8000) {
      // ROM bank n not writable in gboy
    } else if (addr < 0xA000) {
      vram[addr - 0x8000] = value;
    } else if (addr < 0xC000) {
      eram[addr - 0xA000] = value;
    } else if (addr < 0xD000) {
      wram0[addr - 0xC000] = value;
    } else if (addr < 0xE000) {
      wram1[addr - 0xD000] = value;
    } else if (addr < 0xFE00) {
      // Echo RAM (0xE000-0xFDFF) mirrors 0xC000-0xDDFF
      // any changes in this reflect exact changes in wram
      set_u8(addr - 0x2000, value);
    } else if (addr < 0xFEA0) {
      oam[addr - 0xFE00] = value;
    } else if (addr < 0xFF00) {
      // Unusable memory area
    } else if (addr < 0xFF80) {
      io_regs[addr - 0xFF00] = value;
    } else if (addr < 0xFFFF) {
      hram[addr - 0xFF80] = value;
    } else if (addr == 0xFFFF) {
      interrupt_enable = value;
    }
  }

  // Write a 16-bit value
  void set_u16(u16 addr, u16 value) {
    set_u8(addr, static_cast<u8>(value & 0x00FF));
    set_u8(addr + 1, static_cast<u8>((value & 0xFF00) >> 8));
  }
};

// Sprite structure for OAM (Object Attribute Memory)
union sprite {
  u32 data;
  struct {
    u8 m_x;
    u8 m_y;
    u8 m_tile;
    bool m_flip;
  };
};

// PPU (Picture Processing Unit) OAM table
struct ppu {
  std::array<sprite, 40> m_oam;
};

} // namespace mpu

#endif
