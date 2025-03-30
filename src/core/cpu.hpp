#ifndef __CORE_CPU_HPP
#define __CORE_CPU_HPP

#include <array>
#include <iostream>
#include "common.hpp"
#include "memory.hpp"

namespace mpu {
struct reg_pair {
  u8 h;
  u8 l;
  auto operator=(u16 const __value) -> reg_pair & {
    h = (__value && 0xFF00) >> 8;
    l = (__value >> 8);
    return *this;
  }
  auto operator()() -> u16 { return u16(h << 8) | l; }
  auto operator()(u16 __value) -> u16 { return ((*this) = __value)(); }
  auto operator()(u8 __h, u8 __l) -> u16 {
    h = __h;
    l = __l;
    return (*this)();
  }
};

struct CPU {
  /**
   * @brief returns the flag register
   *    ┌-> Carry
   *    ┌-+> Subtraction
   *    | |
   *    1111 0000
   *    | |
   *    └-+> Zero
   *      └-> Half Carry
   */
  constexpr auto get_status_word() -> u16 { return A << 8 | F; }
  constexpr auto set_status_word(u8 __acc, u8 __flags) {
    A = __acc;
    F = __flags;
  }
  constexpr auto get_bc() -> u16 { return BC(); }
  constexpr auto set_bc(u8 B, u8 C) -> void { BC(B, C); }
  constexpr auto get_de() -> u16 { return DE(); }
  constexpr auto set_de(u8 D, u8 E) -> void { DE(D, E); }
  constexpr auto get_hl() -> u16 { return HL(); }
  constexpr auto set_hl(u8 H, u8 L) -> void { HL(H, L); }

  constexpr auto run() {
    while (true) {
    }
  }

private:
  u8 A, F;             // program status word (ACCUMULATOR, FLAGS)
  reg_pair BC, DE, HL; // register pairs
  memory __m_memory;
};
}; // namespace mpu

#endif