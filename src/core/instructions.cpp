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
    case 0x1: // LD BC,a16
      __value_u16 = (__fetch_next() << 8) | __fetch_next();
      set_bc(__value_u16);
      break;
    case 0x2: // LD [BC],A
      TODO("LD [BC], A");
      break;
    case 0x3: // INC BC
      set_bc(get_bc() + 1);
      break;
    case 0x4: // INC B
    {
      auto bc = get_bc();
      auto b = (bc & 0x00FF) >> 2;
      auto c = bc & 0xFF00;
      set_bc(b + 1, c);

      if ((b + 1) & 0xFF00) // overflow occurred
        set_carry();
    } break;
    case 0x5: // DEC B
    {
      auto bc = get_bc();
      auto b = (bc & 0xFF00) >> 2;
      auto c = bc & 0x00FF;
      if (b == 0)
        set_carry(); // borrow occurred
      set_bc(b ? b - 1 : 0xFF, c);
    } break;
    case 0x6: // LD B, u8
      set_bc(__value_u8, get_bc() & 0x00FF);
      break;
    case 0x7: // RCLA
      TODO("RCLA");
      break;
    case 0x8: // LD [a16], SP
      TODO("LD [a16], SP");
      break;
    case 0x9: // ADD HL, BC
    {
      auto sum = get_hl() + get_bc();
      set_hl(sum);
      if (sum & 0xF0000) // overflow occurred
        set_carry();
    } break;
    case 0xA: // LD A, [BC]
      TODO("LD A, [BC]");
      break;
    case 0xB: // DEC BC
      set_bc(get_bc() - 1);
      break;
    case 0xC: // INC C
    {
      auto bc = get_bc();
      auto b = (bc & 0x00FF) >> 2;
      auto c = bc & 0xFF00;
      set_bc(b, c + 1);

      if ((c + 1) & 0xF00) // overflow occurred
        set_carry();
    } break;
    case 0xD: // DEC C
    {
      auto bc = get_bc();
      auto b = (bc & 0xFF00) >> 2;
      auto c = bc & 0x00FF;
      if (c == 0)
        set_carry(); // borrow occurred
      set_bc(b, c ? c - 1 : 0xFF);
    } break;
    case 0xE: // LD C, a8
      set_bc((get_bc() & 0xFF00) >> 2, __value_u8);
      break;
    case 0xF: // RRCA
      TODO("RRCA");
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