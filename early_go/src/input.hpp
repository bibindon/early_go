#ifndef INPUT_HPP
#define INPUT_HPP
#include "stdafx.hpp"
#include "input.hpp"

namespace early_go {

// Monostate pattern.
class input final {
public:
  input();
  static bool update();
  static bool is_down(const int&);
  static bool is_hold(const int&);
  static bool is_up(const int&);

private:
  static ::BYTE current_key_table_[256];
  static ::BYTE previous_key_table_[256];
};
} /* namespace early_go */
#endif
