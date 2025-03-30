#include <print>
#include <stdexcept>
#include "core/cpu.hpp"

int main() {
  mpu::CPU cpu;
  try {
    cpu.run();
  } catch (std::runtime_error &error) {
    std::println("{}", error.what());
  }
}