#ifndef _CORE_CPU_HPP
#define _CORE_CPU_HPP

#include <array>
#include <chrono>
#include <iostream>
#include "common.hpp"
#include "memory.hpp"

namespace mpu {
using clk = std::chrono::steady_clock;

struct CPU {
  // MPU timers addresses
  const u16 TIMA = 0xFF05;
  const u16 TMA = 0xFF06;
  const u16 TMC = 0xFF07;

  constexpr auto get_af() -> u16 { return as<u16>((A << 8) | F); }
  constexpr auto set_acc(u8 _acc) { A = _acc; }
  constexpr auto set_flags(u8 _flags) { F = _flags; }

  constexpr auto get_bc() -> u16 { return as<u16>(B << 8 | C); }
  constexpr auto set_bc(u8 _B, u8 _C) -> void { B = _B, C = _C; }
  constexpr auto set_bc(u16 _BC) -> void {
    set_bc(as<u8>(_BC & 0x00FF), as<u8>((_BC & 0xFF00) >> 8));
  }

  constexpr auto get_de() -> u16 { return as<u16>((D << 8) | E); }
  constexpr auto set_de(u8 _D, u8 _E) -> void { D = _D, E = _E; }
  constexpr auto set_de(u16 _DE) -> void {
    set_de(as<u8>(_DE & 0x00FF), (_DE & 0xFF00) >> 8);
  }

  constexpr auto get_hl() -> u16 { return as<u16>((H << 8) | L); }
  constexpr auto set_hl(u8 _H, u8 _L) -> void { H = _H, L = _L; }
  constexpr auto set_hl(u16 _HL) -> void {
    set_de(as<u8>(_HL & 0x00FF), (_HL & 0xFF00) >> 8);
  }

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
  u8 B, C, D, E, H, L;             // register pairs
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
    TODO("render onto screen and update cycles");
  }
  constexpr auto __fetch_next() -> u8 { return _m__bus.at(_m__pc++); }
};
}; // namespace mpu

#endif