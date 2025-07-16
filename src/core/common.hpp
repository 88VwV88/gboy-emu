#ifndef __CORE_COMMON_HPP
#define __CORE_COMMON_HPP

#include <format>
#include <stdexcept>

namespace mpu {
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;

struct mpu_runtime_error : std::runtime_error {
  mpu_runtime_error(std::string &&message);
};
auto TODO(std::string &&message) -> void;
template <typename AsType> inline auto as(auto &&__value) -> u8 {
  return static_cast<AsType>(__value);
}
}; // namespace mpu

#endif