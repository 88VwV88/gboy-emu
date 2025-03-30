#ifndef __GUI_PIXELS_HPP
#define __GUI_PIXELS_HPP

#include <array>
#include "common.hpp"

namespace gui {
const unsigned WIDTH = 160;
const unsigned HEIGHT = 144;

struct pixels {
  std::array<color, WIDTH * HEIGHT> __screen;
};
}; // namespace gui

#endif