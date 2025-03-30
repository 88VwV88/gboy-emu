#include <print>
#include <stdexcept>
#include "core/cpu.hpp"

int main() {
  mpu::CPU cpu;
  try {
    cpu.run();
  } catch (std::exception &error) {
    std::println("{}", error.what());
  }
}