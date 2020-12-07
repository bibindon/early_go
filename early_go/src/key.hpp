#ifndef KEY_HPP
#define KEY_HPP
#include "stdafx.hpp"

#include <chrono>

namespace early_go {

// Monostate pattern.
struct key final {
  static constexpr int KEY_TABLE_LENGTH{256};
  const static int KEY_DEQUE_MAX_SIZE;

  static bool update();
  static bool is_down(const int&);
  static bool is_down(const std::chrono::system_clock::time_point&, const int&);
  static bool is_hold(const int&);
  static bool is_up(const int&);

  static std::deque<
      std::pair<
          std::chrono::system_clock::time_point,
          std::array<int, key::KEY_TABLE_LENGTH>
      > 
  > key_deque_;
};
} /* namespace early_go */
#endif
