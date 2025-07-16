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

  switch ((_opcode & 0xF0) << 4) {
  case 0x0:
    switch ((_opcode & 0x0F)) {
    case 0x0: // 0x00 NOP
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
      // test for carry
      if (0 == value_u8) {
        if (0 != ++value_u8)
          flags |= CARRY_FLAG;
      } else
        ++value_u8;
      set_b(value_u8);

      // test for half carry
      if (value_u8 & 0x10)
        flags |= HALF_FLAG;
      // test parity
      if (0 == value_u8)
        if (value_u8 & 0x01)
          flags |= PARITY_FLAG;
    } break;
    case 0x5: // 0x05 DEC B
    {
      auto value_u8 = get_bc().B;

      // test for borrow
      if (value_u8 & 0x80)
        flags |= CARRY_FLAG;
      // test for half carry
      if (value_u8 & 0x08)
        flags |= HALF_FLAG;
      // test parity
      if (value_u8 & 0x01)
        flags |= PARITY_FLAG;
    } break;
    case 0x6: // 0x06 LD B, u8
    {
      value_u8 = __fetch_next();
      set_b(value_u8);
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
      value_u32 = get_hl().HL + get_bc().BC;
      set_hl(value_u32 & 0xFFFF);
    } break;
    case 0xA: // 0x0A LD A, [BC]
    {
      value_u16 = get_bc().BC;
      set_acc(bus.at(value_u16));
    } break;
    case 0xB: // 0x0B DEC BC
      set_bc(get_bc().BC - 1);
      break;
    case 0xC: // 0x0C INC C
    {
      value_u8 = get_bc().C;
      if (0 == value_u8) {
        if (0 != ++value_u8)
          flags |= CARRY_FLAG;
      } else
        ++value_u8;
      set_c(value_u8);

      if (0 == value_u8)
        flags |= ZERO_FLAG;
      if (value_u8 & 0x01)
        flags |= PARITY_FLAG;
    } break;
    case 0xD: // 0x0D DEC C
    {
      value_u8 = get_bc().C;
      set_b(--value_u8);

      if (0 == value_u8)
        flags |= ZERO_FLAG;
      if (value_u8 & 0x01)
        flags |= PARITY_FLAG;
    } break;
    case 0xE: // 0x0E LD C, u8
      TODO("LD C,u8");
      break;
    case 0xF: // 0x0F RRCA
      TODO("RRCA");
      break;
    }
    break;
  default:
    throw mpu_runtime_error("invalide opcode");
    break;
  }
}
}; // namespace mpu