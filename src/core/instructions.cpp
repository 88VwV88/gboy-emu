#include "common.hpp"
#include "cpu.hpp"

namespace mpu {
auto CPU::__execute_instruction(u8 __opcode) -> void {
  [[maybe_unused]] u8 __value_u8;
  [[maybe_unused]] u16 __value_u16;
  switch ((__opcode & 0xF0) << 4) {
  case 0x0:
    switch ((__opcode & 0x0F)) {
    case 0x0: // NOP
      break;
    case 0x1: // LD BC,u16
      __value_u16 = (__fetch_next() << 8) | __fetch_next();
      set_bc(__value_u16);
      break;
    case 0x2: // LD (BC),A
      break;
    }
    break;
  case 0x1:
    break;
  case 0x2:
    break;
  case 0x3:
    break;
  case 0x4:
    break;
  case 0x5:
    break;
  case 0x6:
    break;
  case 0x7:
    break;
  case 0x8:
    break;
  case 0xA:
    break;
  case 0xB:
    break;
  case 0xC:
    break;
  case 0xD:
    break;
  case 0xE:
    break;
  case 0xF:
    break;
  default:
    __throw("invalide opcode");
    break;
  }
}
}; // namespace mpu