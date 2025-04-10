#ifndef __CORE_CPU_HPP
#define __CORE_CPU_HPP

#include <array>
#include <chrono>
#include <iostream>
#include "common.hpp"
#include "memory.hpp"

namespace mpu {
using clk = std::chrono::steady_clock;

struct reg_pair {
  u8 h;
  u8 l;
  auto operator=(u16 const __value) -> reg_pair & {
    h = u8(__value && 0xFF00) >> 8;
    l = u8(__value >> 8);
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
  // MPU timers addresses
  const u16 TIMA = 0xFF05;
  const u16 TMA = 0xFF06;
  const u16 TMC = 0xFF07;

  constexpr auto get_af() -> reg_pair { return {A, F}; }
  constexpr auto set_acc(u8 __acc) { A = __acc; }
  constexpr auto set_flags(u8 __flags) { F = __flags; }

  constexpr auto get_bc() -> u16 { return BC(); }
  constexpr auto set_bc(u8 B, u8 C) -> void { BC(B, C); }
  constexpr auto set_bc(u16 __BC) -> void { BC(__BC); }

  constexpr auto get_de() -> u16 { return DE(); }
  constexpr auto set_de(u8 D, u8 E) -> void { DE(D, E); }
  constexpr auto set_de(u16 __DE) -> void { DE(__DE); }

  constexpr auto get_hl() -> u16 { return HL(); }
  constexpr auto set_hl(u8 H, u8 L) -> void { HL(H, L); }
  constexpr auto set_hl(u16 __HL) -> void { HL(__HL); }

  constexpr auto set_zero() -> void { set_flags(F | 0b1000'0000); }
  constexpr auto set_carry() -> void { set_flags(F | 0b0001'0000); }
  constexpr auto set_parity() -> void { set_flags(F | 0b0100'0000); }
  constexpr auto set_auxilary_carry() -> void { set_flags(F | 0b0010'0000); }

  constexpr auto run() {
    while (true) {
      __cycle();
    }
  }

  auto __execute_instruction(u8) -> void;

private:
  /**
   * @brief flag register
   * Z B AC C 0 0 0 0
   */
  u8 A, F;                         // program status word (ACCUMULATOR, FLAGS)
  reg_pair BC, DE, HL;             // register pairs
  u16 _m__pc;                      // program counter
  memory_bus _m__bus;              // 16b memory bus (64KiB)
  bool _m__ready = true;           // mpu ready state
  const u32 _m__speed = 3'000'000; // 3 MHz clock speed

  constexpr auto __cycle() -> void {
    for (u32 i = 0; i < _m__speed; ++i) {
      if (_m__ready) {
        u8 opcode = _m__bus.at(_m__pc);
        __execute_instruction(opcode);
      }
    }
    TODO("render onto screen");
  }
  constexpr auto __fetch_next() -> u8 { return _m__bus.at(_m__pc++); }
};
}; // namespace mpu

#endif