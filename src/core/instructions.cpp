#include "common.hpp"
#include "cpu.hpp"

mpu::mpu_runtime_error::mpu_runtime_error(std::string &&__message)
    : std::runtime_error(__message) {}

auto mpu::TODO(std::string &&__message) -> void {
  throw mpu::mpu_runtime_error(std::move(__message));
}

namespace mpu {

auto CPU::execute_instruction(u8 _opcode) -> void {
  [[maybe_unused]] u8 value_u8;
  [[maybe_unused]] u16 value_u16;
  [[maybe_unused]] u32 value_u32;
  [[maybe_unused]] u8 flags = get_psw().F;

  switch ((_opcode & 0xF0) >> 4) {
  case 0x0:
    switch ((_opcode & 0x0F)) {
    case 0x0: // 0x00 NOP
      TODO("NOP");
      break;

    case 0x1: // 0x01 LD BC, u16
      value_u16 = (__fetch_next() << 8) | __fetch_next();
      set_bc(value_u16);
      break;

    case 0x2: // 0x02 LD [BC], A
    {
      auto value_u16 = get_bc().BC;
      set_acc(bus.at(value_u16));
    } break;

    case 0x3: // 0x03 INC BC
      set_bc(get_bc().BC + 1);
      break;

    case 0x4: // 0x04 INC B
    {
      auto value_u8 = get_bc().B;
      set_b(value_u8 + 1);

      // carry flag is not changed

      // test for zero flag
      if (0 == value_u8 + 1)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // test for half carry
      if ((0 == (value_u8 & 0x10)) && (0 != (++value_u8 & 0x10)))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // test for subtract flag
      flags &= ~SUBTRACT_FLAG;
    } break;

    case 0x05: // DEC B
    {
      value_u8 = get_bc().B;
      u8 result = value_u8 - 1;

      // Zero flag
      if (result == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // Subtract flag (always set for DEC)
      flags |= SUBTRACT_FLAG;

      // Half-carry: borrow from bit 4
      if ((value_u8 & 0x0F) == 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      set_b(result);
    } break;

    case 0x6: // 0x06 LD B, u8
    {
      value_u8 = __fetch_next();
      set_b(value_u8);
    } break;

    case 0x7: // 0x07 RCLA
    {
      u8 result = get_psw().A;
      u8 carry = flags & CARRY_FLAG ? 0x01 : 0x00;

      // Carry flag
      if (result & 0x01)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;

      result = (result << 1) | carry;
      set_acc(result);
    } break;

    case 0x8: // 0x08 LD [a16], SP
    {
      u16 addr = (__fetch_next() << 8) | __fetch_next();
      TODO("LD [a16], SP");
    } break;

    case 0x09: {
      u16 hl = get_hl().HL;
      u16 bc = get_bc().BC;
      u16 result = hl + bc;

      set_hl(result);

      flags &= ~SUBTRACT_FLAG;

      // Carry if wrapped around (unsigned overflow)
      if ((result & 0xFFFF) < hl)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;

      // Half-carry
      if (((hl & 0x0FFF) + (bc & 0x0FFF)) > 0x0FFF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;
    } break;

    case 0xA: // 0x0A LD A, [BC]
    {
      value_u16 = get_bc().BC;
      set_acc(bus.at(value_u16));
    } break;

    case 0xB: // 0x0B DEC BC
      set_bc(get_bc().BC - 1);
      flags |= SUBTRACT_FLAG;
      break;

    case 0xC: // 0x0C INC C
    {
      auto value_u8 = get_bc().C;
      set_c(value_u8 + 1);

      // carry flag is not changed

      // test for zero flag
      if (0 == value_u8 + 1)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // test for half carry
      if ((0 == (value_u8 & 0x10)) && (0 != (++value_u8 & 0x10)))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // test for subtract flag
      flags &= ~SUBTRACT_FLAG;
    } break;

    case 0xD: // 0x0D DEC C
    {
      value_u8 = get_bc().C;
      u8 result = value_u8 - 1;

      // Zero flag
      if (result == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // Subtract flag (always set for DEC)
      flags |= SUBTRACT_FLAG;

      // Half-carry: borrow from bit 4
      if ((value_u8 & 0x0F) == 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      set_c(result);
    } break;

    case 0xE: // 0x0E LD C, u8
    {
      value_u8 = __fetch_next();
      set_c(value_u8);
    } break;

    case 0xF: // 0x0F RRCA
    {
      u8 result = get_psw().A;
      u8 carry = flags & CARRY_FLAG ? 0x01 : 0x00;

      // Carry flag
      if (result & 0x80)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;

      result = (result >> 1) | (carry << 7);
      set_acc(result);
    } break;
    }
    break;

  case 0x1:
    switch ((_opcode & 0x0F)) {
    case 0x0: // 0x10 STOP
      TODO("STOP");
      break;

    case 0x1: // 0x11 LD DE, u16
      value_u16 = (__fetch_next() << 8) | __fetch_next();
      set_de(value_u16);
      break;

    case 0x2: // 0x12 LD [DE], A
    {
      value_u16 = get_de().DE;
      bus.set_u8(value_u16, get_psw().A);
    } break;

    case 0x3: // 0x13 INC DE
      set_de(get_de().DE + 1);
      break;

    case 0x4: // 0x14 INC D
    {
      auto value_u8 = get_de().D;
      set_d(value_u8 + 1);

      // carry flag is not changed

      // test for zero flag
      if (0 == value_u8 + 1)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // test for half carry
      if ((0 == (value_u8 & 0x10)) && (0 != (++value_u8 & 0x10)))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // test for subtract flag
      flags &= ~SUBTRACT_FLAG;
    } break;

    case 0x5: // 0x15 DEC D
    {
      value_u8 = get_de().D;
      u8 result = value_u8 - 1;

      // Zero flag
      if (result == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // Subtract flag (always set for DEC)
      flags |= SUBTRACT_FLAG;
    } break;

    case 0x6: // 0x16 LD D, u8
      value_u8 = __fetch_next();
      set_d(value_u8);
      break;

    case 0x7: // 0x17 RLA Rotate left A through Carry
    {
      u8 result = get_psw().A;
      u8 carry = flags & CARRY_FLAG ? 0x01 : 0x00;

      // Carry flag
      if (result & 0x01)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;

      result = (result >> 1) | (carry << 7);
      set_acc(result);

      flags &= ~SUBTRACT_FLAG;

      flags &= ~HALF_FLAG;
    } break;

    case 0x8: // 0x18 JR e
    {
      int8_t offset = static_cast<int8_t>(__fetch_next());
      set_pc(get_pc() + offset);
    } break;

    case 0x9: // 0x19 ADD HL, DE
    {
      u16 hl = get_hl().HL;
      u16 de = get_de().DE;
      u16 result = hl + de;

      set_hl(result);

      flags &= ~SUBTRACT_FLAG;

      // Carry if wrapped around (unsigned overflow)
      if ((result & 0xFFFF) < hl)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;

      // Half-carry
      if (((hl & 0x0FFF) + (de & 0x0FFF)) > 0x0FFF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;
    } break;

    case 0xA: // 0x1A LD A, [DE]
      value_u16 = get_de().DE;
      set_acc(bus.at(value_u16));
      break;

    case 0xB: // 0x1B DEC DE
      set_de(get_de().DE - 1);
      break;

    case 0xC: // 0x1C INC E
    {
      auto value_u8 = get_de().E;
      set_e(value_u8 + 1);

      // carry flag is not changed

      // test for zero flag
      if (0 == value_u8 + 1)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // test for half carry
      if ((value_u8 & 0x0F) + 1 > 0x0F)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // test for subtract flag
      flags &= ~SUBTRACT_FLAG;
    } break;

    case 0xD: // 0x1D DEC E
    {
      value_u8 = get_de().E;
      u8 result = value_u8 - 1;

      // Zero flag
      if (result == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // Subtract flag (always set for DEC)
      flags |= SUBTRACT_FLAG;

      // Half-carry: borrow from bit 4
      if ((value_u8 & 0x0F) == 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      set_e(result);
    } break;

    case 0xE: // 0x1E LD E, u8
      value_u8 = __fetch_next();
      set_e(value_u8);
      break;

    case 0xF: // 0x1F RRA Rotate Right A through Carry
    {
      u8 result = get_psw().A;
      u8 carry = flags & CARRY_FLAG ? 0x01 : 0x00;

      // Carry flag
      if (result & 0x80)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;

      result = (result << 1) | carry;

      set_acc(result);

      flags &= ~SUBTRACT_FLAG;

      flags &= ~HALF_FLAG;
    } break;
    }

  case 0x2:
    switch ((_opcode & 0x0F)) {

    case 0x0: // 0x20 JR NZ, e
    {
      int8_t offset = static_cast<int8_t>(__fetch_next());
      if (!(flags & ZERO_FLAG))
        set_pc(get_pc() + offset);
    } break;

    case 0x1: // 0x21 LD HL, u16
    {
      value_u16 = (__fetch_next() << 8) | __fetch_next();
      set_hl(value_u16);
    } break;

    case 0x2: // 0x22 LD [a16], HL
    {
      u16 addr = (__fetch_next() << 8) | __fetch_next();
      bus.set_u16(addr, get_hl().HL);
    } break;

    case 0x3: // 0x23 INC HL
    {
      set_hl(get_hl().HL + 1);
    } break;

    case 0x4: // 0x24 INC H
    {
      value_u8 = get_hl().H;
      set_h(value_u8 + 1);

      // carry flag is not changed

      // test for zero flag
      if (0 == value_u8 + 1)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // test for half carry
      if ((value_u8 & 0x0F) + 1 > 0x0F)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // test for subtract flag
      flags &= ~SUBTRACT_FLAG;
    } break;

    case 0x5: // 0x25 DEC H
    {
      value_u8 = get_hl().H;
      u8 result = value_u8 - 1;
      set_h(result);

      // Zero flag
      if (result == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // Subtract flag (always set for DEC)
      flags |= SUBTRACT_FLAG;

      // Half-carry: borrow from bit 4
      if ((value_u8 & 0x0F) == 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;
    } break;

    case 0x6: // 0x26 LD H, u8
    {
      value_u8 = __fetch_next();
      set_h(value_u8);
    } break;

    case 0x7: // 0x27 DAA
    {
      u8 acc = get_psw().A;
      u8 correction = 0;
      bool carry = false;

      // If the last operation was addition (N flag = 0)
      if (!(flags & SUBTRACT_FLAG)) {
        // If the lower nibble > 9 or half-carry is set
        if ((acc & 0x0F) > 9 || (flags & HALF_FLAG)) {
          correction += 0x06;
        }
        // If the upper nibble > 9 or carry is set
        if ((acc & 0xF0) > 0x90 || (flags & CARRY_FLAG)) {
          correction += 0x60;
          carry = true;
        }
      } else {
        // If the last operation was subtraction (N flag = 1)
        if (flags & HALF_FLAG) {
          correction += (acc & 0x0F) <= 9 ? 0xFA : 0xA0;
        }
        if (flags & CARRY_FLAG) {
          correction += 0x9A;
          carry = true;
        }
      }

      acc += correction;
      set_acc(acc);

      // Update flags
      if (acc == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // Clear half-carry flag
      flags &= ~HALF_FLAG;

      // Update carry flag
      if (carry)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x8: // 0x28 JR Z, e
    {
      value_u8 = __fetch_next();
      if (flags & ZERO_FLAG)
        set_pc(get_pc() + value_u8);
    } break;

    case 0x9: // 0x29 ADD HL, HL
    {
      value_u16 = get_hl().HL;
      u16 result = value_u16 + value_u16;
      set_hl(result);

      flags &= ~SUBTRACT_FLAG;

      if ((result & 0xFFFF) < value_u16)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;

      if (((value_u16 & 0x0FFF) + (value_u16 & 0x0FFF)) > 0x0FFF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;
    } break;

    case 0xA: // 0x2A LD A, [HL+]
    {
      value_u16 = get_hl().HL;
      set_acc(bus.at(value_u16));
      set_hl(value_u16 + 1);
    } break;

    case 0xB: // 0x2B DEC HL
    {
      set_hl(get_hl().HL - 1);
    } break;

    case 0xC: // 0x2C INC L
    {
      value_u8 = get_hl().L;
      set_l(value_u8 + 1);

      // zero flag
      if (0 == value_u8 + 1)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry
      if ((value_u8 & 0x0F) + 1 > 0x0F)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // subtract flag
      flags &= ~SUBTRACT_FLAG;
    } break;

    case 0xD: // 0x2D DEC L
    {
      value_u8 = get_hl().L;
      set_l(value_u8 - 1);

      // subtract flag
      flags |= SUBTRACT_FLAG;

      // half carry
      if ((value_u8 & 0x0F) == 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // zero flag
      if (0 == value_u8 - 1)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;
    } break;

    case 0xE: // 0x2E LD L, u8
    {
      value_u8 = __fetch_next();
      set_l(value_u8);
    } break;

    case 0xF: // 0x2F CPL
    {
      u8 acc = get_psw().A;
      acc = ~acc;
      set_acc(acc);

      flags |= SUBTRACT_FLAG;

      flags |= HALF_FLAG;
    } break;
    }

  case 0x3:
    switch ((_opcode & 0x0F)) {
    case 0x0: // 0x30 JR NC, e8
    {
      value_u16 = get_pc();
      value_u8 = __fetch_next();

      if (!(flags & CARRY_FLAG)) {
        set_pc(value_u16 + value_u8);
      }
    } break;

    case 0x1: // 0x31 LD SP, u16
    {
      value_u16 = (__fetch_next() << 8) | __fetch_next();
      set_sp(value_u16);
    } break;

    case 0x2: // 0x32 LD [a16], A
    {
      u16 addr = (__fetch_next() << 8) | __fetch_next();
      bus.set_u8(addr, get_psw().A);
    } break;

    case 0x3: // 0x33 INC SP
    {
      set_sp(get_sp().SP + 1);
    } break;

    case 0x4: // 0x34 INC [HL]
    {
      u16 addr = get_hl().HL;
      bus.set_u8(addr, bus.at(addr) + 1);
    } break;

    case 0x5: // 0x35 DEC [HL]
    {
      u16 addr = get_hl().HL;
      bus.set_u8(addr, bus.at(addr) - 1);
    } break;

    case 0x6: // 0x36 LD [HL], u8
    {
      value_u8 = __fetch_next();
      bus.set_u8(get_hl().HL, value_u8);
    } break;

    case 0x7: // 0x37 SCF set carry flag
    {
      flags |= CARRY_FLAG;
      flags &= ~SUBTRACT_FLAG;
      flags &= ~HALF_FLAG;
    } break;

    case 0x8: // 0x38 JR C, e8
    {
      value_u8 = __fetch_next();
      if (flags & CARRY_FLAG)
        set_pc(get_pc() + value_u8);
    } break;

    case 0x9: // 0x39 ADD HL, SP
    {
      set_hl(get_hl().HL + get_sp().SP);

      flags &= ~SUBTRACT_FLAG;

      if ((get_hl().HL + get_sp().SP) & 0xFFFF < get_hl().HL)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;

      if (((get_hl().HL & 0x0FFF) + (get_sp().SP & 0x0FFF)) > 0x0FFF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;
    } break;

    case 0xA: // 0x3A LD A, [HL-]
    {
      value_u16 = get_hl().HL;
      set_acc(bus.at(value_u16));
      set_hl(value_u16 - 1);
    } break;

    case 0xB: // 0x3B DEC SP
    {
      set_sp(get_sp().SP - 1);
    } break;

    case 0xC: // 0x3C INC A
    {
      value_u8 = get_psw().A;
      set_acc(value_u8 + 1);

      // zero flag
      if (0 == value_u8 + 1)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry
      if ((value_u8 & 0x0F) + 1 > 0x0F)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // subtract flag
      flags &= ~SUBTRACT_FLAG;
    } break;

    case 0xD: // 0x3D DEC A
    {
      value_u8 = get_psw().A;
      set_acc(value_u8 - 1);

      // zero flag
      if (0 == value_u8 - 1)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // subtract flag
      flags |= SUBTRACT_FLAG;

      // half carry
      if ((value_u8 & 0x0F) == 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;
    } break;

    case 0xE: // 0x3E LD A, u8
    {
      value_u8 = __fetch_next();
      set_acc(value_u8);
    } break;

    case 0xF: // 0x3F CCF complement carry flag
    {
      flags ^= CARRY_FLAG;
      flags &= ~SUBTRACT_FLAG;
      flags &= ~HALF_FLAG;
    } break;
    }
    break;

  case 0x4:
    switch ((_opcode & 0x0F)) {
    case 0x0: // 0x40 LD B, B
    {
      TODO("NOP"); // NOP disguised as LD instruction
    } break;

    case 0x1: // 0x41 LD B, C
    {
      value_u8 = get_bc().C;
      set_b(value_u8);
    } break;

    case 0x2: // 0x42 LD B, D
    {
      value_u8 = get_de().D;
      set_b(value_u8);
    } break;

    case 0x3: // 0x43 LD B, E
    {
      value_u8 = get_de().E;
      set_b(value_u8);
    } break;

    case 0x4: // 0x44 LD B, H
    {
      value_u8 = get_hl().H;
      set_b(value_u8);
    } break;

    case 0x5: // 0x45 LD B, L
    {
      value_u8 = get_hl().L;
      set_b(value_u8);
    } break;

    case 0x6: // 0x46 LD B, [HL]
    {
      value_u16 = get_hl().HL;
      set_b(bus.at(value_u16));
    } break;

    case 0x7: // 0x47 LD B, A
    {
      value_u8 = get_psw().A;
      set_b(value_u8);
    } break;

    case 0x8: // 0x48 LD C, B
    {
      value_u8 = get_bc().B;
      set_c(value_u8);
    } break;

    case 0x9: // 0x49 LD C, C
    {
      TODO("NOP"); // NOP disguised as LD instruction
    } break;

    case 0xA: // 0x49 LD C, D
    {
      value_u8 = get_de().D;
      set_c(value_u8);
    } break;

    case 0xB: // 0x4A LD C, E
    {
      value_u8 = get_de().E;
      set_c(value_u8);
    } break;

    case 0xC: // 0x4C LD C, H
    {
      value_u8 = get_hl().H;
      set_c(value_u8);
    } break;

    case 0xD: // 0x4D LD C, L
    {
      value_u8 = get_hl().L;
      set_c(value_u8);
    } break;

    case 0xE: // 0x4E LD C, [HL]
    {
      value_u16 = get_hl().HL;
      set_c(bus.at(value_u16));
    } break;

    case 0xF: // 0x4F LD C, A
    {
      value_u8 = get_psw().A;
      set_c(value_u8);
    } break;
    }
    break;

  case 0x5:
    switch ((_opcode & 0x0F)) {
    case 0x0: // 0x50 LD D, B
    {
      value_u8 = get_bc().B;
      set_d(value_u8);
    } break;

    case 0x1: // 0x51 LD D, C
    {
      value_u8 = get_bc().C;
      set_d(value_u8);
    } break;

    case 0x2: // 0x52 LD D, D
    {
      TODO("NOP"); // NOP disguised as LD instruction
    } break;

    case 0x3: // 0x53 LD D, E
    {
      value_u8 = get_de().E;
      set_d(value_u8);
    } break;

    case 0x4: // 0x54 LD D, H
    {
      value_u8 = get_hl().H;
      set_d(value_u8);
    } break;

    case 0x5: // 0x55 LD D, L
    {
      value_u8 = get_hl().L;
      set_d(value_u8);
    } break;

    case 0x6: // 0x56 LD D, [HL]
    {
      value_u16 = get_hl().HL;
      set_d(bus.at(value_u16));
    } break;

    case 0x7: // 0x57 LD D, A
    {
      value_u8 = get_psw().A;
      set_d(value_u8);
    } break;

    case 0x8: // 0x58 LD E, B
    {
      value_u8 = get_bc().B;
      set_e(value_u8);
    } break;

    case 0x9: // 0x59 LD E, C
    {
      value_u8 = get_bc().C;
      set_e(value_u8);
    } break;

    case 0xA: // 0x5A LD E, D
    {
      value_u8 = get_de().D;
      set_e(value_u8);
    } break;

    case 0xB: // 0x5B LD E, E
    {
      TODO("NOP"); // NOP disguised as LD instruction
    } break;

    case 0xC: // 0x5C LD E, H
    {
      value_u8 = get_hl().H;
      set_e(value_u8);
    } break;

    case 0xD: // 0x5D LD E, L
    {
      value_u8 = get_hl().L;
      set_e(value_u8);
    } break;

    case 0xE: // 0x5E LD E, [HL]
    {
      value_u16 = get_hl().HL;
      set_e(bus.at(value_u16));
    } break;

    case 0xF: // 0x5F LD E, A
    {
      value_u8 = get_psw().A;
      set_e(value_u8);
    } break;
    }
    break;

  case 0x6:
    switch ((_opcode & 0x0F)) {
    case 0x0: // 0x60 LD H, B
    {
      value_u8 = get_bc().B;
      set_h(value_u8);
    } break;

    case 0x1: // 0x61 LD H, C
    {
      value_u8 = get_bc().C;
      set_h(value_u8);
    } break;

    case 0x2: // 0x62 LD H, D
    {
      value_u8 = get_de().D;
      set_h(value_u8);
    } break;

    case 0x3: // 0x63 LD H, E
    {
      value_u8 = get_de().E;
      set_h(value_u8);
    } break;

    case 0x4: // 0x64 LD H, H
    {
      TODO("NOP"); // NOP disguised as LD instruction
    } break;

    case 0x5: // 0x65 LD H, L
    {
      value_u8 = get_hl().L;
      set_h(value_u8);
    } break;

    case 0x6: // 0x66 LD H, [HL]
    {
      value_u16 = get_hl().HL;
      set_h(bus.at(value_u16));
    } break;

    case 0x7: // 0x67 LD H, A
    {
      value_u8 = get_psw().A;
      set_h(value_u8);
    } break;

    case 0x8: // 0x68 LD L, B
    {
      value_u8 = get_bc().B;
      set_l(value_u8);
    } break;

    case 0x9: // 0x69 LD L, C
    {
      value_u8 = get_bc().C;
      set_l(value_u8);
    } break;

    case 0xA: // 0x6A LD L, D
    {
      value_u8 = get_de().D;
      set_l(value_u8);
    } break;

    case 0xB: // 0x6B LD L, E
    {
      value_u8 = get_de().E;
      set_l(value_u8);
    } break;

    case 0xC: // 0x6C LD L, H
    {
      value_u8 = get_hl().H;
      set_l(value_u8);
    } break;

    case 0xD: // 0x6D LD L, L
    {
      TODO("NOP"); // NOP disguised as LD instruction
    } break;

    case 0xE: // 0x6E LD L, [HL]
    {
      value_u16 = get_hl().HL;
      set_l(bus.at(value_u16));
    } break;

    case 0xF: // 0x6F LD L, A
    {
      value_u8 = get_psw().A;
      set_l(value_u8);
    } break;
    }
    break;

  case 0x7:
    switch ((_opcode & 0x0F)) {
    case 0x0: // 0x70 LD [HL], B
    {
      value_u16 = get_hl().HL;
      bus.set_u8(value_u16, get_bc().B);
    } break;

    case 0x1: // 0x71 LD [HL], C
    {
      value_u16 = get_hl().HL;
      bus.set_u8(value_u16, get_bc().C);
    } break;

    case 0x2: // 0x72 LD [HL], D
    {
      value_u16 = get_hl().HL;
      bus.set_u8(value_u16, get_de().D);
    } break;

    case 0x3: // 0x73 LD [HL], E
    {
      value_u16 = get_hl().HL;
      bus.set_u8(value_u16, get_de().E);
    } break;

    case 0x4: // 0x74 LD [HL], H
    {
      value_u16 = get_hl().HL;
      bus.set_u8(value_u16, get_hl().H);
    } break;

    case 0x5: // 0x75 LD [HL], L
    {
      value_u16 = get_hl().HL;
      bus.set_u8(value_u16, get_hl().L);
    } break;

    case 0x6: // 0x76 HALT
    {
      // TODO: implement HALT
    } break;

    case 0x7: // 0x77 LD [HL], A
    {
      value_u16 = get_hl().HL;
      bus.set_u8(value_u16, get_psw().A);
    } break;

    case 0x8: // 0x78 LD A, B
    {
      value_u8 = get_bc().B;
      set_acc(value_u8);
    } break;

    case 0x9: // 0x79 LD A, C
    {
      value_u8 = get_bc().C;
      set_acc(value_u8);
    } break;

    case 0xA: // 0x7A LD A, D
    {
      value_u8 = get_de().D;
      set_acc(value_u8);
    } break;

    case 0xB: // 0x7B LD A, E
    {
      value_u8 = get_de().E;
      set_acc(value_u8);
    } break;

    case 0xC: // 0x7C LD A, H
    {
      value_u8 = get_hl().H;
      set_acc(value_u8);
    } break;

    case 0xD: // 0x7D LD A, L
    {
      value_u8 = get_hl().L;
      set_acc(value_u8);
    } break;

    case 0xE: // 0x7E LD A, [HL]
    {
      value_u16 = get_hl().HL;
      set_acc(bus.at(value_u16));
    } break;

    case 0xF: // 0x7F LD A, A
    {
      value_u8 = get_psw().A;
      set_acc(value_u8);
    } break;
    }
    break;

  case 0x8:
    switch ((_opcode & 0x0F)) {
    case 0x0: // 0x80 ADD A, B
    {
      value_u8 = get_bc().B;
      u8 acc = get_psw().A;
      value_u16 = acc + value_u8;
      set_acc(value_u16 & 0xFF);

      // set subtract flag to 0
      flags &= ~SUBTRACT_FLAG;

      // set zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // set half carry flag
      if ((acc & 0xF) + (value_u8 & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // set carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x1: // 0x81 ADD A, C
    {
      value_u8 = get_bc().C;
      u8 acc = get_psw().A;
      value_u16 = acc + value_u8;
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      if ((acc & 0xF) + (value_u8 & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x2: // 0x82 ADD A, D
    {
      value_u8 = get_de().D;
      u8 acc = get_psw().A;
      value_u16 = acc + value_u8;
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      if ((acc & 0xF) + (value_u8 & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x3: // 0x83 ADD A, E
    {
      value_u8 = get_de().E;
      u8 acc = get_psw().A;
      value_u16 = acc + value_u8;
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      if ((acc & 0xF) + (value_u8 & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x4: // 0x84 ADD A, H
    {
      value_u8 = get_hl().H;
      u8 acc = get_psw().A;
      value_u16 = acc + value_u8;
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      if ((acc & 0xF) + (value_u8 & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x5: // 0x85 ADD A, L
    {
      value_u8 = get_psw().A;
      value_u16 = get_psw().A + get_hl().L;
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      if ((value_u8 & 0xF) + (value_u8 & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x6: // 0x86 ADD A, [HL]
    {
      value_u16 = get_hl().HL;
      u8 acc = get_psw().A;
      value_u16 = acc + bus.at(value_u16);
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) + (bus.at(value_u16) & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x7: // 0x87 ADD A, A
    {
      u8 acc = get_psw().A;
      set_acc(acc + acc);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (acc + acc == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) + (acc & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (acc + acc > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x8: // 0x88 ADC A, B
    {
      value_u8 = get_bc().B;
      u8 acc = get_psw().A;
      value_u16 = acc + value_u8 + ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) + (value_u8 & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x9: // 0x89 ADC A, C
    {
      value_u8 = get_bc().C;
      u8 acc = get_psw().A;
      value_u16 = acc + value_u8 + ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) + (value_u8 & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xA: // 0x8A ADC A, D
    {
      value_u8 = get_de().D;
      u8 acc = get_psw().A;
      value_u16 = acc + value_u8 + ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) + (value_u8 & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xB: // 0x8B ADC A, E
    {
      value_u8 = get_de().E;
      u8 acc = get_psw().A;
      value_u16 = acc + value_u8 + ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) + (value_u8 & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xC: // 0x8C ADC A, H
    {
      value_u8 = get_hl().H;
      u8 acc = get_psw().A;
      value_u16 = acc + value_u8 + ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) + (value_u8 & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xD: // 0x8D ADC A, L
    {
      value_u8 = get_hl().L;
      u8 acc = get_psw().A;
      value_u16 = acc + value_u8 + ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) + (value_u8 & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xE: // 0x8E ADC A, [HL]
    {
      value_u16 = get_hl().HL;
      u8 acc = get_psw().A;
      value_u16 = acc + bus.at(value_u16) + ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) + (bus.at(value_u16) & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xF: // 0x8F ADC A, A
    {
      u8 acc = get_psw().A;
      value_u16 = acc + acc + ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) + (acc & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;
    }
    break;

  case 0x9:
    switch ((_opcode & 0x0F)) {
    case 0x0: // 0x90 SUB A, B
    {
      value_u8 = get_bc().B;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8;
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) - (value_u8 & 0xF) < 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x1: // 0x91 SUB A, C
    {
      value_u8 = get_bc().C;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8;
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) - (value_u8 & 0xF) < 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x2: // 0x92 SUB A, D
    {
      value_u8 = get_de().D;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8;
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) - (value_u8 & 0xF) < 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x3: // 0x93 SUB A, E
    {
      value_u8 = get_de().E;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8;
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) - (value_u8 & 0xF) < 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x4: // 0x94 SUB A, H
    {
      value_u8 = get_hl().H;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8;
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) - (value_u8 & 0xF) < 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x5: // 0x95 SUB A, L
    {
      value_u8 = get_hl().L;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8;
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) - (value_u8 & 0xF) < 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x6: // 0x96 SUB A, [HL]
    {
      value_u16 = get_hl().HL;
      u8 acc = get_psw().A;
      value_u16 = acc - bus.at(value_u16);
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) - (bus.at(value_u16) & 0xF) < 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x7: // 0x97 SUB A, A
    {
      u8 acc = get_psw().A;
      value_u16 = acc - acc;
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) - (acc & 0xF) < 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x8: // 0x98 SBC A, B
    {
      value_u8 = get_bc().B;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8 - ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) - (value_u8 & 0xF) - ((flags & CARRY_FLAG) ? 1 : 0) < 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x9: // 0x99 SBC A, C
    {
      value_u8 = get_bc().C;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8 - ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) - (value_u8 & 0xF) - ((flags & CARRY_FLAG) ? 1 : 0) < 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xA: // 0x9A SBC A, D
    {
      value_u8 = get_de().D;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8 - ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // half carry flag
      if ((acc & 0xF) - (value_u8 & 0xF) - ((flags & CARRY_FLAG) ? 1 : 0) < 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;
    } break;

    case 0xB: // 0x9B SBC A, E
    {
      value_u8 = get_de().E;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8 - ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // half carry flag
      if ((acc & 0xF) - (value_u8 & 0xF) - ((flags & CARRY_FLAG) ? 1 : 0) < 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;
    } break;

    case 0xC: // 0x9C SBC A, H
    {
      value_u8 = get_hl().H;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8 - ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // half carry flag
      if ((acc & 0xF) - (value_u8 & 0xF) - ((flags & CARRY_FLAG) ? 1 : 0) < 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;
    } break;

    case 0xD: // 0x9D SBC A, L
    {
      value_u8 = get_hl().L;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8 - ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // half carry flag
      if ((acc & 0xF) - (value_u8 & 0xF) - ((flags & CARRY_FLAG) ? 1 : 0) < 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;
    } break;

    case 0xE: // 0x9E SBC A, [HL]
    {
      value_u16 = get_hl().HL;
      u8 acc = get_psw().A;
      value_u16 = acc - bus.at(value_u16) - ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // half carry flag
      if ((acc & 0xF) - (bus.at(value_u16) & 0xF) -
              ((flags & CARRY_FLAG) ? 1 : 0) <
          0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;
    } break;

    case 0xF: // 0x9F SBC A, A
    {
      u8 acc = get_psw().A;
      value_u16 = acc - acc - ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      flags |= SUBTRACT_FLAG;

      // half carry flag
      if ((acc & 0xF) - (acc & 0xF) - ((flags & CARRY_FLAG) ? 1 : 0) < 0)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (value_u16 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;
    } break;
    }
    break;

  case 0xA:
    switch (_opcode & 0xF) {
    case 0x0: // 0xAA AND A, B
    {
      value_u8 = get_bc().B;
      u8 acc = get_psw().A;
      value_u16 = acc & value_u8;
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      flags |= HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;

    case 0x1: // 0xAB AND A, C
    {
      value_u8 = get_bc().C;
      u8 acc = get_psw().A;
      value_u16 = acc & value_u8;
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      flags |= HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;

    case 0x2: // 0xAC AND A, D
    {
      value_u8 = get_de().D;
      u8 acc = get_psw().A;
      value_u16 = acc & value_u8;
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      flags |= HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;

    case 0x3: // 0xAD AND A, E
    {
      value_u8 = get_de().E;
      u8 acc = get_psw().A;
      value_u16 = acc & value_u8;
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      flags |= HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;

    case 0x4: // 0xAE AND A, H
    {
      value_u8 = get_hl().H;
      u8 acc = get_psw().A;
      value_u16 = acc & value_u8;
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      flags |= HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;

    case 0x5: // 0xAF AND A, L
    {
      value_u8 = get_hl().L;
      u8 acc = get_psw().A;
      value_u16 = acc & value_u8;
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      flags |= HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;

    case 0x6: // 0xB6 AND A, [HL]
    {
      value_u16 = get_hl().HL;
      u8 acc = get_psw().A;
      value_u16 = acc & bus.at(value_u16);
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      flags |= HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;

    case 0x7: // 0xB7 AND A, A
    {
      // only flags affected
      flags &= ~SUBTRACT_FLAG;

      if (get_psw().A & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      flags |= HALF_FLAG;
      flags &= ~CARRY_FLAG;
    } break;

    case 0x8: // 0xB8 XOR A, B
    {
      value_u8 = get_bc().B;
      u8 acc = get_psw().A;
      value_u16 = acc ^ value_u8;
      set_acc(value_u16 & 0xFF);

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // all other flags are cleared
      flags &= ~SUBTRACT_FLAG;
      flags &= ~HALF_FLAG;
      flags &= ~CARRY_FLAG;
    } break;

    case 0x9: // 0xB9 XOR A, C
    {
      value_u8 = get_bc().C;
      u8 acc = get_psw().A;
      value_u16 = acc ^ value_u8;
      set_acc(value_u16 & 0xFF);

      // zero flag

      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // all other flags are cleared
      flags &= ~SUBTRACT_FLAG;
      flags &= ~HALF_FLAG;
      flags &= ~CARRY_FLAG;
    } break;

    case 0xA: // 0xBA XOR A, D
    {
      value_u8 = get_de().D;
      u8 acc = get_psw().A;
      value_u16 = acc ^ value_u8;
      set_acc(value_u16 & 0xFF);

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // all other flags are cleared
      flags &= ~SUBTRACT_FLAG;
      flags &= ~HALF_FLAG;
      flags &= ~CARRY_FLAG;
    } break;

    case 0xB: // 0xBB XOR A, E
    {
      value_u8 = get_de().E;
      u8 acc = get_psw().A;
      value_u16 = acc ^ value_u8;
      set_acc(value_u16 & 0xFF);

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // all other flags are cleared
      flags &= ~SUBTRACT_FLAG;
      flags &= ~HALF_FLAG;
      flags &= ~CARRY_FLAG;
    } break;

    case 0xC: // 0xBC XOR A, H
    {
      value_u8 = get_hl().H;
      u8 acc = get_psw().A;
      value_u16 = acc ^ value_u8;
      set_acc(value_u16 & 0xFF);

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // all other flags are cleared
      flags &= ~SUBTRACT_FLAG;
      flags &= ~HALF_FLAG;
      flags &= ~CARRY_FLAG;
    } break;

    case 0xD: // 0xBD XOR A, L
    {
      value_u8 = get_hl().L;
      u8 acc = get_psw().A;
      value_u16 = acc ^ value_u8;
      set_acc(value_u16 & 0xFF);

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // all other flags are cleared
      flags &= ~SUBTRACT_FLAG;
      flags &= ~HALF_FLAG;
      flags &= ~CARRY_FLAG;
    } break;

    case 0xE: // 0xBE XOR A, [HL]
    {
      value_u16 = get_hl().HL;
      u8 acc = get_psw().A;
      value_u16 = acc ^ bus.at(value_u16);
      set_acc(value_u16 & 0xFF);

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // all other flags are cleared
      flags &= ~SUBTRACT_FLAG;
      flags &= ~HALF_FLAG;
      flags &= ~CARRY_FLAG;
    } break;

    case 0xF: // 0xBF XOR A, A
    {
      u8 acc = get_psw().A;
      value_u16 = acc ^ acc;
      set_acc(value_u16 & 0xFF);

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // all other flags are cleared
      flags &= ~SUBTRACT_FLAG;
      flags &= ~HALF_FLAG;
      flags &= ~CARRY_FLAG;
    } break;
    }
    break;

  case 0xB:
    switch (_opcode & 0xF) {
    case 0x0: // 0xB0 OR A, B
    {
      value_u8 = get_bc().B;
      u8 acc = get_psw().A;
      value_u16 = acc | value_u8;
      set_acc(value_u16 & 0xFF);

      // subtract flag
      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      flags &= ~HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;

    case 0x1: // 0xB1 OR A, C
    {
      value_u8 = get_bc().C;
      u8 acc = get_psw().A;
      value_u16 = acc | value_u8;
      set_acc(value_u16 & 0xFF);

      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      flags &= ~HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;

    case 0x2: // 0xB2 OR A, D
    {
      value_u8 = get_de().D;
      u8 acc = get_psw().A;
      value_u16 = acc | value_u8;
      set_acc(value_u16 & 0xFF);

      // subtract flag
      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      flags &= ~HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;

    case 0x3: // 0xB3 OR A, E
    {
      value_u8 = get_de().E;
      u8 acc = get_psw().A;
      value_u16 = acc | value_u8;
      set_acc(value_u16 & 0xFF);

      // subtract flag
      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      flags &= ~HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;

    case 0x4: // 0xB4 OR A, H
    {
      value_u8 = get_hl().H;
      u8 acc = get_psw().A;
      value_u16 = acc | value_u8;
      set_acc(value_u16 & 0xFF);

      // subtract flag
      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      flags &= ~HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;

    case 0x5: // 0xB5 OR A, L
    {
      value_u8 = get_hl().L;
      u8 acc = get_psw().A;
      value_u16 = acc | value_u8;
      set_acc(value_u16 & 0xFF);

      // subtract flag
      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      flags &= ~HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;

    case 0x6: // 0xB6 OR A, [HL]
    {
      value_u16 = get_hl().HL;
      u8 acc = get_psw().A;
      value_u16 = acc | bus.at(value_u16);
      set_acc(value_u16 & 0xFF);

      // subtract flag
      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      flags &= ~HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;

    case 0x7: // 0xB7 OR A, A
    {
      u8 acc = get_psw().A;
      value_u16 = acc | acc;
      set_acc(value_u16 & 0xFF);

      // subtract flag
      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      flags &= ~HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;

    case 0x8: // 0xB8 CP A, B
    {
      value_u8 = get_bc().B;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8;

      // subtract flag
      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) < (value_u8 & 0xF))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (acc < (value_u8 & 0xFF))
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x9: // 0xB9 CP A, C
    {
      value_u8 = get_bc().C;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8;

      // subtract flag
      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) < (value_u8 & 0xF))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (acc < (value_u8 & 0xFF))
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xA: // 0xBA CP A, D
    {
      value_u8 = get_de().D;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8;

      // subtract flag
      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) < (value_u8 & 0xF))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (acc < (value_u8 & 0xFF))
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xB: // 0xBB CP A, E
    {
      value_u8 = get_de().E;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8;

      // subtract flag
      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) < (value_u8 & 0xF))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (acc < (value_u8 & 0xFF))
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xC: // 0xBC CP A, H
    {
      value_u8 = get_hl().H;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8;

      // subtract flag
      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) < (value_u8 & 0xF))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (acc < (value_u8 & 0xFF))
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xD: // 0xBD CP A, L
    {
      value_u8 = get_hl().L;
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8;

      // subtract flag
      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) < (value_u8 & 0xF))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (acc < (value_u8 & 0xFF))
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xE: // 0xBE CP A, [HL]
    {
      value_u16 = get_hl().HL;
      u8 acc = get_psw().A;
      value_u16 = acc - bus.at(value_u16);

      // subtract flag
      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) < (bus.at(value_u16) & 0xF))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (acc < (bus.at(value_u16) & 0xFF))
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xF: // 0xBF CP A, A
    {
      // redundant instruction, only flags are set
      // subtract flag
      flags |= SUBTRACT_FLAG;

      // zero flag
      flags |= ZERO_FLAG;

      // half carry flag
      flags &= ~HALF_FLAG;

      // carry flag
      flags &= ~CARRY_FLAG;
    } break;
    }
    break;

  case 0xC:
    switch (_opcode & 0xF) {
    case 0x0: // RET NZ
    {
      if (!(flags & ZERO_FLAG)) {
        value_u16 = bus.at(get_sp().SP) | (bus.at(get_sp().SP + 1) << 8);
        set_sp(get_sp().SP + 2);
        set_pc(value_u16);
      }
    } break;

    case 0x1: // POP BC
    {
      set_c(bus.at(get_sp().SP));
      set_b(bus.at(get_sp().SP + 1));
      set_sp(get_sp().SP + 2);
    } break;

    case 0x2: // JP NZ, nn
    {
      if (!(flags & ZERO_FLAG)) {
        value_u16 = __fetch_next() << 8 | __fetch_next();
        set_pc(value_u16);
      } else
        set_pc(get_pc() + 3);
    } break;

    case 0x3: // JP nn
    {
      value_u16 = __fetch_next() | __fetch_next() << 8;
      set_pc(value_u16);
    } break;

    case 0x4: // CALL NZ, nn
    {
      if (!(flags & ZERO_FLAG)) {
        value_u16 = __fetch_next() | __fetch_next() << 8;
        set_sp(get_sp().SP - 2);
        bus.set_u8(get_sp().SP, get_pc() & 0xFF);
        bus.set_u8(get_sp().SP + 1, (get_pc() >> 8) & 0xFF);
        set_pc(value_u16);
      } else
        set_pc(get_pc() + 3);
    } break;

    case 0x5: // PUSH BC
    {
      set_sp(get_sp().SP - 2);
      bus.set_u8(get_sp().SP, get_bc().C);
      bus.set_u8(get_sp().SP + 1, get_bc().B);
    } break;

    case 0x6: // ADD A, n
    {
      value_u8 = __fetch_next();
      u8 acc = get_psw().A;
      value_u16 = acc + value_u8;
      set_acc(value_u16 & 0xFF);

      // subtract flag
      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) + (value_u8 & 0xF) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (acc + value_u8 > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x7: // RST 00
    {
      set_sp(get_sp().SP - 2);
      value_u16 = get_pc() + 1;
      bus.set_u8(get_sp().SP, value_u16 & 0xFF);
      bus.set_u8(get_sp().SP + 1, (value_u16 >> 8) & 0xFF);
      set_pc(0x00);
    } break;

    case 0x8: // RET Z
    {
      if (flags & ZERO_FLAG) {
        value_u16 = bus.at(get_sp().SP) | (bus.at(get_sp().SP + 1) << 8);
        set_sp(get_sp().SP + 2);
        set_pc(value_u16);
      }
    } break;

    case 0x9: // RET
    {
      value_u16 = bus.at(get_sp().SP) | (bus.at(get_sp().SP + 1) << 8);
      set_sp(get_sp().SP + 2);
      set_pc(value_u16);
    } break;

    case 0xA: // JP Z, nn
    {
      if (flags & ZERO_FLAG) {
        value_u16 = __fetch_next() << 8 | __fetch_next();
        set_pc(value_u16);
      }
    } break;

    case 0xB: // PREFIX
    {
      TODO("PREFIX");
    } break;

    case 0xC: // CALL Z, nn
    {
      if (flags & ZERO_FLAG) {
        value_u16 = __fetch_next() | __fetch_next() << 8;
        set_sp(get_sp().SP - 2);
        bus.set_u8(get_sp().SP, get_pc() & 0xFF);
        bus.set_u8(get_sp().SP + 1, (get_pc() >> 8) & 0xFF);
        set_pc(value_u16);
      }
    } break;

    case 0xD: // CALL nn
    {
      value_u16 = __fetch_next() | __fetch_next() << 8;
      set_sp(get_sp().SP - 2);
      bus.set_u8(get_sp().SP, get_pc() & 0xFF);
      bus.set_u8(get_sp().SP + 1, (get_pc() >> 8) & 0xFF);
      set_pc(value_u16);
    } break;

    case 0xE: // ADC A, n8
    {
      value_u8 = __fetch_next();
      u8 acc = get_psw().A;
      value_u16 = acc + value_u8 + ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      // subtract flag
      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) + (value_u8 & 0xF) + ((flags & CARRY_FLAG) ? 1 : 0) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (acc + value_u8 + ((flags & CARRY_FLAG) ? 1 : 0) > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xF: // RST 08
    {
      set_sp(get_sp().SP - 2);
      value_u16 = get_pc() + 1;
      bus.set_u8(get_sp().SP, (value_u16 >> 8) & 0xFF);
      bus.set_u8(get_sp().SP + 1, value_u16 & 0xFF);
      set_pc(0x08);
    } break;
    }
    break;

  case 0xD:
    switch (_opcode & 0xF) {
    case 0x0: // RET NC
    {
      if (!(flags & CARRY_FLAG)) {
        value_u16 = bus.at(get_sp().SP) | (bus.at(get_sp().SP + 1) << 8);
        set_sp(get_sp().SP + 2);
        set_pc(value_u16);
      }
    } break;

    case 0x1: // POP DE
    {
      set_d(bus.at(get_sp().SP));
      set_e(bus.at(get_sp().SP + 1));
      set_sp(get_sp().SP + 2);
    } break;

    case 0x2: // JP NC, nn
    {
      if (!(flags & CARRY_FLAG)) {
        value_u16 = __fetch_next() << 8 | __fetch_next();
        set_pc(value_u16);
      }
    } break;

    case 0x4: // CALL NC, nn
    {
      if (!(flags & CARRY_FLAG)) {
        value_u16 = __fetch_next() | __fetch_next() << 8;
        set_sp(get_sp().SP - 2);
        bus.set_u8(get_sp().SP, get_pc() & 0xFF);
        bus.set_u8(get_sp().SP + 1, (get_pc() >> 8) & 0xFF);
        set_pc(value_u16);
      }
    } break;

    case 0x5: // PUSH DE
    {
      set_sp(get_sp().SP - 2);
      bus.set_u8(get_sp().SP, get_de().D);
      bus.set_u8(get_sp().SP + 1, get_de().E);
    } break;

    case 0x6: // SUB A, n8
    {
      value_u8 = __fetch_next();
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8;
      set_acc(value_u16 & 0xFF);

      // subtract flag
      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) < (value_u8 & 0xF))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (acc < value_u8)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x7: // RST 10
    {
      set_sp(get_sp().SP - 2);
      value_u16 = get_pc() + 1;
      bus.set_u8(get_sp().SP, value_u16 & 0xFF);
      bus.set_u8(get_sp().SP + 1, (value_u16 >> 8) & 0xFF);
      set_pc(0x10);
    } break;

    case 0x8: // RET C
    {
      if (flags & CARRY_FLAG) {
        value_u16 = bus.at(get_sp().SP) | (bus.at(get_sp().SP + 1) << 8);
        set_sp(get_sp().SP + 2);
        set_pc(value_u16);
      }
    } break;

    case 0x9: // RETI
    {
      value_u16 = bus.at(get_sp().SP) | (bus.at(get_sp().SP + 1) << 8);
      set_sp(get_sp().SP + 2);
      set_pc(value_u16);
      INTERRUPT_ENABLE = true;
    } break;

    case 0xA: // JP C, nn
    {
      value_u16 = __fetch_next() | __fetch_next() << 8;

      if (flags & CARRY_FLAG) {
        set_pc(value_u16);
      }
    } break;

    case 0xC: // CALL C, nn
    {
      if (flags & CARRY_FLAG) {
        value_u16 = __fetch_next() | __fetch_next() << 8;
        set_sp(get_sp().SP - 2);
        bus.set_u8(get_sp().SP, get_pc() & 0xFF);
        bus.set_u8(get_sp().SP + 1, (get_pc() >> 8) & 0xFF);
        set_pc(value_u16);
      }
    } break;

    case 0xE: // SBC A, n8
    {
      value_u8 = __fetch_next();
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8 - ((flags & CARRY_FLAG) ? 1 : 0);
      set_acc(value_u16 & 0xFF);

      // subtract flag
      flags |= SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) < (value_u8 & 0xF) + ((flags & CARRY_FLAG) ? 1 : 0))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (acc < value_u8 + ((flags & CARRY_FLAG) ? 1 : 0))
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xF: // RST 18
    {
      set_sp(get_sp().SP - 2);
      value_u16 = get_pc() + 1;
      bus.set_u8(get_sp().SP, (value_u16 >> 8) & 0xFF);
      bus.set_u8(get_sp().SP + 1, value_u16 & 0xFF);
      set_pc(0x18);
    } break;
    }
    break;

  case 0xE:
    switch (_opcode & 0xF) {
    case 0x0: // LDH (a8), A
    {
      value_u8 = __fetch_next();
      bus.set_u8(0xFF00 + value_u8, get_psw().A);
    } break;

    case 0x1: // POP HL
    {
      set_hl(bus.at(get_sp().SP) | (bus.at(get_sp().SP + 1) << 8));
      set_sp(get_sp().SP + 2);
    } break;

    case 0x2: // LD (C), A
    {
      bus.set_u8(0xFF00 + get_bc().C, get_psw().A);
    } break;

    case 0x5: // PUSH HL
    {
      set_sp(get_sp().SP - 2);
      bus.set_u8(get_sp().SP, get_hl().H);
      bus.set_u8(get_sp().SP + 1, get_hl().L);
    } break;

    case 0x6: // AND A, n8
    {
      value_u8 = __fetch_next();
      u8 acc = get_psw().A;
      value_u16 = acc & value_u8;
      set_acc(value_u16 & 0xFF);

      // subtract flag
      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) & (value_u8 & 0xF))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if ((acc & 0xFF) & (value_u8 & 0xFF))
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x7: // RST 20
    {
      set_sp(get_sp().SP - 2);
      value_u16 = get_pc();
      bus.set_u8(get_sp().SP, value_u16 & 0xFF);
      bus.set_u8(get_sp().SP + 1, (value_u16 >> 8) & 0xFF);
      set_pc(0x20);
    } break;

    case 0x8: // ADD SP, n8 (opcode 0xE8)
    {
      u8 offset = static_cast<int8_t>(__fetch_next());
      u16 sp = get_sp().SP;
      u16 result = sp + offset;

      // Clear Zero and Subtract flags
      flags &= ~(ZERO_FLAG | SUBTRACT_FLAG);

      if (((sp & 0xF) + (offset & 0xF)) > 0xF)
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      if (((sp & 0xFF) + (offset & 0xFF)) > 0xFF)
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;

      set_sp(result);
    } break;

    case 0x9: // 0xE9 JP HL
    {
      set_pc(get_hl().HL);
    } break;

    case 0xA: // 0xEA LD [a16], A
    {
      value_u16 = __fetch_next() | __fetch_next() << 8;
      bus.set_u8(value_u16, get_psw().A);
    } break;

    case 0xE: // 0xEE XOR A, n8
    {
      value_u8 = __fetch_next();
      u8 acc = get_psw().A;
      value_u16 = acc ^ value_u8;
      set_acc(value_u16 & 0xFF);

      // subtract flag
      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) ^ (value_u8 & 0xF))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if ((acc & 0xFF) ^ (value_u8 & 0xFF))
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xF: // 0xEF RST 28
    {
      set_sp(get_sp().SP - 2);
      value_u16 = get_pc() + 1;
      bus.set_u8(get_sp().SP, (value_u16 >> 8) & 0xFF);
      bus.set_u8(get_sp().SP + 1, value_u16 & 0xFF);
      set_pc(0x28);
    } break;
    }
    break;

  case 0xF:
    switch (_opcode & 0xF) {
    case 0x0: // LDH A, (a8)
    {
      value_u8 = __fetch_next();
      set_acc(bus.at(0xFF00 + value_u8));
    } break;

    case 0x1: // POP AF
    {
      set_acc(bus.at(get_sp().SP) | (bus.at(get_sp().SP + 1) << 8));
      set_sp(get_sp().SP + 2);
    } break;

    case 0x2: // LD A, (C)
    {
      set_acc(bus.at(0xFF00 + get_bc().C));
    } break;

    case 0x3: // DI
    {
      INTERRUPT_ENABLE = false;
    } break;

    case 0x5: // PUSH AF
    {
      set_sp(get_sp().SP - 2);
      bus.set_u8(get_sp().SP, get_psw().A & 0xFF);
      bus.set_u8(get_sp().SP + 1, (get_psw().A >> 8) & 0xFF);
    } break;

    case 0x6: // OR A, n8
    {
      value_u8 = __fetch_next();
      u8 acc = get_psw().A;
      value_u16 = acc | value_u8;
      set_acc(value_u16 & 0xFF);

      // subtract flag
      flags &= ~SUBTRACT_FLAG;

      // zero flag
      if (value_u16 & 0xFF == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) | (value_u8 & 0xF))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if ((acc & 0xFF) | (value_u8 & 0xFF))
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0x7: // RST 30
    {
      set_sp(get_sp().SP - 2);
      value_u16 = get_pc() + 1;
      bus.set_u8(get_sp().SP, (value_u16 >> 8) & 0xFF);
      bus.set_u8(get_sp().SP + 1, value_u16 & 0xFF);
      set_pc(0x30);
    } break;

    case 0x8: // LD HL, SP+n8
    {
      value_u8 = __fetch_next();
      u16 sp = get_sp().SP;
      u16 result = sp + value_u8;
      set_hl(result);

      flags &= ~ZERO_FLAG;
      flags &= ~SUBTRACT_FLAG;

      flags = (flags & ~(HALF_FLAG | CARRY_FLAG)) |
              (((sp & 0xF) + (value_u8 & 0xF)) > 0xF ? HALF_FLAG : 0) |
              (((sp & 0xFF) + (value_u8 & 0xFF)) > 0xFF ? CARRY_FLAG : 0);
    } break;

    case 0x9: // LD SP, HL
    {
      set_sp(get_hl().HL);
    } break;

    case 0xA: // LD A, (a16)
    {
      value_u16 = __fetch_next() | __fetch_next() << 8;
      set_acc(bus.at(value_u16));
    } break;

    case 0xB: // EI
    {
      INTERRUPT_ENABLE = true;
    } break;

    case 0xE: // CP A, n8
    {
      value_u8 = __fetch_next();
      u8 acc = get_psw().A;
      value_u16 = acc - value_u8;

      // subtract flag
      flags |= SUBTRACT_FLAG;

      // zero flag
      if ((value_u16 & 0xFF) == 0)
        flags |= ZERO_FLAG;
      else
        flags &= ~ZERO_FLAG;

      // half carry flag
      if ((acc & 0xF) < (value_u8 & 0xF))
        flags |= HALF_FLAG;
      else
        flags &= ~HALF_FLAG;

      // carry flag
      if (acc < (value_u8 & 0xFF))
        flags |= CARRY_FLAG;
      else
        flags &= ~CARRY_FLAG;
    } break;

    case 0xF: // 0xFF RST 38
    {
      set_sp(get_sp().SP - 2);
      value_u16 = get_pc() + 1;
      bus.set_u8(get_sp().SP, (value_u16 >> 8) & 0xFF);
      bus.set_u8(get_sp().SP + 1, value_u16 & 0xFF);
      set_pc(0x38);
    } break;
    }
    break;
  }
  set_flags(flags);
}

}; // namespace mpu