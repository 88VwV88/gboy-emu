#include "common.hpp"
#include "cpu.hpp"

namespace mpu {
auto CPU::__execute_instruction(u8 __opcode) -> void {
  [[maybe_unused]] u8 __value_u8;
  [[maybe_unused]] u16 __value_u16;

  switch ((__opcode & 0xF0) << 4) {
  case 0x0:
    switch ((__opcode & 0x0F)) {
    case 0x0: // 0x00 NOP
      break;
    case 0x1: // 0x01 LD BC, n16
      __value_u16 = (__fetch_next() << 8) | __fetch_next();
      set_bc(__value_u16);
      break;
    case 0x2: // 0x02 LD [BC], A
    {
      auto addr = get_bc();
      set_acc(_m__bus.at(addr));
    } break;
    case 0x3: // 0x03 INC BC
      set_bc(get_bc() + 1);
      break;
    case 0x4: // 0x04 INC B
    {
      auto bc = get_bc();
      auto b = (bc & 0x00FF) >> 2;
      auto c = bc & 0xFF00;
      set_bc(b + 1, c);

      if ((b + 1) & 0xFF00) // overflow occurred
        set_carry();
    } break;
    case 0x5: // 0x05 DEC B
    {
      auto bc = get_bc();
      auto b = (bc & 0xFF00) >> 2;
      auto c = bc & 0x00FF;
      if (b == 0)
        set_carry(); // borrow occurred
      set_bc(b ? b - 1 : 0xFF, c);
    } break;
    case 0x6: // 0x06 LD B, n8
    {
      __value_u8 = __fetch_next();
      set_bc(__value_u8, get_bc() & 0x00FF);
    } break;
    case 0x7: // 0x07 RCLA
      TODO("RCLA");
      break;
    case 0x8: // 0x08 LD [a16], SP
    {
      u16 addr = (__fetch_next() << 8) | __fetch_next();
      TODO("LD [a16], SP");
    } break;
    case 0x9: // 0x09 ADD HL, BC
    {
      auto sum = get_hl() + get_bc();
      set_hl(sum);
      if (sum & 0xF0000) // overflow occurred
        set_carry();
    } break;
    case 0xA: // 0x0A LD A, [BC]
    {
      auto addr = get_bc();
      auto acc = (get_psw() & 0xFF00) >> 8;
      _m__bus.set_u8(addr, acc);
    } break;
    case 0xB: // 0x0B DEC BC
      set_bc(get_bc() - 1);
      break;
    case 0xC: // 0x0C INC C
    {
      auto bc = get_bc();
      auto b = (bc & 0x00FF) >> 2;
      auto c = bc & 0xFF00;
      set_bc(b, c + 1);

      if ((c + 1) & 0xF00) // overflow occurred
        set_carry();
    } break;
    case 0xD: // 0x0D DEC C
    {
      auto bc = get_bc();
      auto b = (bc & 0xFF00) >> 2;
      auto c = bc & 0x00FF;
      if (c == 0)
        set_carry(); // borrow occurred
      set_bc(b, c ? c - 1 : 0xFF);
    } break;
    case 0xE: // 0x0E LD C, n8
      __value_u8 = __fetch_next();
      set_bc((get_bc() & 0xFF00) >> 2, __value_u8);
      break;
    case 0xF: // 0x0F RRCA
      TODO("RRCA");
      break;
    }
    break;
  case 0x1:
    switch ((__opcode & 0x0F)) {
    case 0x0: // 0x10 STOP n8
      TODO("STOP");
      break;
    case 0x1: // 0x11 LD DE, n16
      __value_u16 = (__fetch_next() << 8) | __fetch_next();
      set_de(__value_u16);
      break;
    case 0x2: // 0x12 LD [DE], A
    {
      auto addr = get_de();
      auto acc = (get_psw() & 0xFF00) >> 8;
      _m__bus.set_u8(addr, acc);
    } break;
    case 0x3: // 0x13 INC DE
      set_de(get_de() + 1);
      break;
    case 0x4: // 0x14 INC D
    {
      auto de = get_de();
      auto d = (de & 0x00FF) >> 2;
      auto e = de & 0xFF00;
      set_de(d + 1, e);

      if ((d + 1) & 0xFF00) // overflow occurred
        set_carry();
    } break;
    case 0x5: // 0x15 DEC D
    {
      auto de = get_de();
      auto d = (de & 0xFF00) >> 2;
      auto e = de & 0x00FF;
      if (d == 0)
        set_carry(); // borrow occurred
      set_de(d ? d - 1 : 0xFF, e);
    } break;
    case 0x6: // 0x16 LD D, n8
    {
      __value_u8 = __fetch_next();
      set_de(__value_u8, get_de() & 0x00FF);
    } break;
    case 0x7: // 0x17 RLA
      TODO("RLA");
      break;
    case 0x8: // 0x18 JR e8
      TODO("JR");
      break;
    case 0x9: // 0x19 ADD HL, DE
    {
      auto sum = get_hl() + get_de();
      set_hl(sum);
      if (sum & 0xF0000) // overflow occurred
        set_carry();
    } break;
    case 0xA: // 0x1A LD A, [DE]
    {
      auto addr = get_de();
      auto acc = (get_psw() & 0xFF00) >> 8;
      _m__bus.set_u8(addr, acc);
    } break;
    case 0xB: // 0x1B DEC DE
      set_de(get_de() - 1);
      break;
    case 0xC: // 0x1C INC E
    {
      auto de = get_de();
      auto d = (de & 0x00FF) >> 2;
      auto e = de & 0xFF00;
      set_de(d, e + 1);

      if ((e + 1) & 0xF00) // overflow occurred
        set_carry();
    } break;
    case 0xD: // 0x1D DEC E
    {
      auto de = get_de();
      auto d = (de & 0xFF00) >> 2;
      auto e = de & 0x00FF;
      if (e == 0)
        set_carry(); // borrow occurred
      set_de(d, e ? e - 1 : 0xFF);
    } break;
    case 0xE: // 0x1E LD E, u8
      __value_u8 = __fetch_next();
      set_de((get_de() & 0xFF00) >> 2, __value_u8);
      break;
    case 0xF: // 0x1F RRA
      TODO("RRA");
      break;
    }
    break;
  default:
    __throw("invalide opcode");
    break;
  }
}
}; // namespace mpu