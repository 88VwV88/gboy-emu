#ifndef __CORE_COMMON_HPP
#define __CORE_COMMON_HPP

#include <stdexcept>
#include <format>

namespace mpu {
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;

struct mpu_runtime_error : std::runtime_error {
  mpu_runtime_error(std::string &&__message) : std::runtime_error(__message) {}
};
auto __throw(std::string &&__message) -> void {
  throw mpu_runtime_error(std::move(__message));
}
inline auto TODO(std::string &&__code) -> void {
  throw std::runtime_error(std::format("UNIMPLEMENTED: {}", __code));
}
template <typename AsType> inline auto as(auto &&__value) -> u8 {
  return static_cast<AsType>(__value);
}
}; // namespace mpu

#endif