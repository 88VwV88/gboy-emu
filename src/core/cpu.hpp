#ifndef _CORE_CPU_HPP
#define _CORE_CPU_HPP

#include "common.hpp"
#include "memory.hpp"
#include <array>
#include <chrono>
#include <iostream>

namespace mpu {
using clk = std::chrono::steady_clock;

struct CPU {
  // MPU timers addresses
  const u16 TIMA = 0xFF05;
  const u16 TMA = 0xFF06;
  const u16 TMC = 0xFF07;

private:
  /**
   * @brief flag register
   * Z B HC C 0 0 0 0
   */

  // Program Status Word (PSW)
  union p_PSW {
    u16 PSW;
    struct {
      u8 A;
      u8 F;
    };
  } PSW;
  // BC register pair
  union p_BC {
    u16 BC;
    struct {
      u8 B;
      u8 C;
    };
  } BC;
  // DE register pair
  union p_DE {
    u16 DE;
    struct {
      u8 D;
      u8 E;
    };
  } DE;
  // HL register pair
  union p_HL {
    u16 HL;
    struct {
      u8 H;
      u8 L;
    };
  } HL;

  // SP register
  union p_SP {
    u16 SP;
    p_SP() : SP(0xFFFE) {} // Initialize SP to 0xFFFE
  } SP;

public:
  p_PSW get_psw() { return PSW; }
  void set_acc(const u8 _acc) { PSW.A = _acc; }
  void set_flags(const u8 _flags) { PSW.F = _flags; }

  p_BC get_bc() const { return BC; }
  void set_b(const u8 _B) { BC.B = _B; }
  void set_c(const u8 _C) { BC.C = _C; }
  void set_bc(const u16 _BC) { BC.BC = _BC; }

  p_DE get_de() const { return DE; }
  void set_d(const u8 _D) { DE.D = _D; }
  void set_e(const u8 _E) { DE.E = _E; }
  void set_de(const u16 _DE) { DE.DE = _DE; }

  p_HL get_hl() const { return HL; }
  void set_h(const u8 _H) { HL.H = _H; }
  void set_l(const u8 _L) { HL.L = _L; }
  void set_hl(const u8 _HL) { HL.HL = _HL; }

  p_SP get_sp() const { return SP; }
  void set_sp(const u16 _SP) { SP.SP = _SP; }

  constexpr static u8 ZERO_FLAG = 0x80;
  constexpr static u8 SUBTRACT_FLAG = 0x40;
  constexpr static u8 HALF_FLAG = 0x20;
  constexpr static u8 CARRY_FLAG = 0x10;
  inline static bool INTERRUPT_ENABLE = false;

  void run() {
    while (true) {
      __cycle();
    }
  }

  auto execute_instruction(u8) -> void;

private:
  u16 pc;                        // program counter
  mmu bus;                       // 16b memory bus (64KiB)
  bool m_ready = true;           // mpu ready state
  const u32 m_speed = 3'000'000; // 3 MHz clock speed

  auto __cycle() -> void {
    for (u32 i = 0; i < m_speed; ++i) {
      if (m_ready) {
        u8 opcode = bus.at(pc);
        execute_instruction(opcode);
      }
    }
    TODO("render onto screen and update cycles");
  }
  auto __fetch_next() -> u8 { return bus.at(pc++); }

public:
  // getter and setter for program counter
  u16 get_pc() const { return pc; }
  void set_pc(const u16 _pc) { pc = _pc; }
};
}; // namespace mpu

#endif