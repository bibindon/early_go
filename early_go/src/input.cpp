#include "stdafx.hpp"
#include "input.hpp"

namespace early_go {

::BYTE input::current_key_table_[256]{};
::BYTE input::previous_key_table_[256]{};

input::input() { }

bool input::update()
{
  std::copy(std::cbegin(current_key_table_), std::cend(current_key_table_),
            std::begin(previous_key_table_));

  return ::GetKeyboardState(current_key_table_);
}

bool input::is_down(const int& keycode)
{
  if (!(previous_key_table_[keycode] & 0x80) &&
      (current_key_table_[keycode] & 0x80)) {
    return true;
  }
  return false;
}

bool input::is_hold(const int& keycode)
{
  if (current_key_table_[keycode] & 0x80) {
    return true;
  }
  return false;
}

bool input::is_up(const int& keycode)
{
  if ((previous_key_table_[keycode] & 0x80) &&
      !(current_key_table_[keycode] & 0x80)) {
    return true;
  }
  return false;
}


} /* namespace early_go */
