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
    [[maybe_unused]] u8 flags;

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
                }
                break;

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
                }
                break;

                case 0x05: // DEC B
                {
                    value_u8 = get_bc().B;
                    u8 result = value - 1;

                    // Zero flag
                    if (result == 0)
                        flags |= ZERO_FLAG;
                    else
                        flags &= ~ZERO_FLAG;

                    // Subtract flag (always set for DEC)
                    flags |= SUBTRACT_FLAG;

                    // Half-carry: borrow from bit 4
                    if ((value & 0x0F) == 0)
                        flags |= HALF_FLAG;
                    else
                        flags &= ~HALF_FLAG;

                    set_b(result);
                }
                break;

                case 0x6: // 0x06 LD B, u8
                {
                    value_u8 = __fetch_next();
                    set_b(value_u8);
                }
                break;

                case 0x7: // 0x07 RCLA
                    TODO("RCLA");
                    break;

                case 0x8: // 0x08 LD [a16], SP
                {
                    u16 addr = (__fetch_next() << 8) | __fetch_next();
                    TODO("LD [a16], SP");
                }
                break;

                case 0x09:
                {
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
                }
                break;


                case 0xA: // 0x0A LD A, [BC]
                {
                    value_u16 = get_bc().BC;
                    set_acc(bus.at(value_u16));
                }
                break;

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
                }
                break;

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
                    if ((value & 0x0F) == 0)
                        flags |= HALF_FLAG;
                    else
                        flags &= ~HALF_FLAG;

                    set_c(result);
                }
                break;

                case 0xE: // 0x0E LD C, u8
                    TODO("LD C,u8");
                    break;

                case 0xF: // 0x0F RRCA
                    TODO("RRCA");
                    break;
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
                    bus.at(value_u16) = get_acc();
                }
                break;

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
                }
                break;

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
                }
                break;

                case 0x6: // 0x16 LD D, u8
                    value_u8 = __fetch_next();
                    set_d(value_u8);
                    break;

                case 0x7: // 0x17 RLA Rotate left A through Carry
                {
                    u8 result = get_acc();
                    u8 carry = flags & CARRY_FLAG ? 0x01 : 0x00;

                    // Carry flag
                    if(result & 0x01)
                        flags |= CARRY_FLAG;
                    else
                        flags &= ~CARRY_FLAG;

                    result = (result >> 1) | (carry << 7);
                    set_acc(result);

                    flags &= ~SUBTRACT_FLAG;

                    flags &= ~HALF_FLAG;
                }
                break;

                case 0x8: // 0x18 JR e
                    TODO("JR e");
                    break;

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
                }
                break;

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
                }
                break;

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
                  if ((value & 0x0F) == 0)
                      flags |= HALF_FLAG;
                  else
                      flags &= ~HALF_FLAG;

                  set_e(result);
                }
                break;

                case 0xE: // 0x1E LD E, u8
                    value_u8 = __fetch_next();
                    set_e(value_u8);
                    break;

                case 0xF: // 0x1F RRA Rotate Right A through Carry
                {
                  u8 result = get_acc();
                  u8 carry = flags & CARRY_FLAG ? 0x01 : 0x00;

                  // Carry flag
                  if(result & 0x80)
                    flags |= CARRY_FLAG;
                  else
                    flags &= ~CARRY_FLAG;

                  result = (result << 1) | carry;

                  set_acc(result);

                    flags &= ~SUBTRACT_FLAG;

                    flags &= ~HALF_FLAG;
                }
                break;
            }

            case 0x2:{
              switch((_opcode & 0x0F)){

                case 0x0: // 0x20 JR NZ, e
                  if(flags & ZERO_FLAG)
                    TODO("JR NZ, e");
                  else
                    TODO("JR NZ, e");
                  break;

                case 0x1: // 0x21 LD HL, u16
                {
                  value_u16 = (__fetch_next() << 8) | __fetch_next();
                  set_hl(value_u16);
                }
                break;

                case 0x2: // 0x22 LD [a16], HL
                {
                  u16 addr = (__fetch_next() << 8) | __fetch_next();
                  bus.at(addr) = get_hl().HL;
                }
                break;

                case 0x3: // 0x23 INC HL
                {
                  set_hl(get_hl().HL + 1);
                }
                break;

                case 0x4: // 0x24 INC H
                {
                  value_u8 = get_hl().H;
                  set_h(value_u8 + 1);

                  // carry flag is not changed

                  // test for zero flag
                  if(0 == value_u8 + 1)
                    flags |= ZERO_FLAG;
                  else
                    flags &= ~ZERO_FLAG;

                  // test for half carry
                  if((value_u8 & 0x0F) + 1 > 0x0F)
                    flags |= HALF_FLAG;
                  else
                    flags &= ~HALF_FLAG;

                  // test for subtract flag
                  flags &= ~SUBTRACT_FLAG;
                }
                break;

                case 0x5: // 0x25 DEC H
                {
                  value_u8 = get_hl().H;
                  u8 result = value_u8 - 1;
                  set_h(result);

                  // Zero flag
                  if(result == 0)
                    flags |= ZERO_FLAG;
                  else
                    flags &= ~ZERO_FLAG;

                  // Subtract flag (always set for DEC)
                  flags |= SUBTRACT_FLAG;

                  // Half-carry: borrow from bit 4
                  if((value_u8 & 0x0F) == 0)
                    flags |= HALF_FLAG;
                  else
                    flags &= ~HALF_FLAG;
                }
                break;

                case 0x6: // 0x26 LD H, u8
                {
                  value_u8 = __fetch_next();
                }
              }

              
            }
            break;

                

        default:
            throw mpu_runtime_error("invalide opcode");
            break;
    }
}

}; // namespace mpu