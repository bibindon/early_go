#include "stdafx.hpp"
#include "key.hpp"

namespace early_go {

const int key::KEY_DEQUE_MAX_SIZE = 300;
std::deque<
    std::pair<
        std::chrono::system_clock::time_point,
        std::array<int, key::KEY_TABLE_LENGTH>
    > 
> key::key_deque_;

bool key::update()
{
  ::BYTE temp_key_table[KEY_TABLE_LENGTH];

  if (!::GetKeyboardState(temp_key_table)) {
    return false;
  }
  std::array<int, KEY_TABLE_LENGTH> current_key_table;
  std::copy(temp_key_table, temp_key_table + KEY_TABLE_LENGTH,
            std::begin(current_key_table));

  std::chrono::system_clock::time_point update_time{
      std::chrono::system_clock::now()};
  key_deque_.push_front(std::make_pair(update_time, current_key_table));

  if (key_deque_.size() > KEY_DEQUE_MAX_SIZE) {
    key_deque_.erase(key_deque_.begin() + KEY_DEQUE_MAX_SIZE - 1,
                     key_deque_.end());
  }

  return true;
}

bool key::is_down(const int& keycode)
{
  if (key_deque_.size() < 2) {
    return false;
  }
  if (!(key_deque_.at(1).second[keycode] & 0x80) &&
      (key_deque_.at(0).second[keycode] & 0x80)) {
    return true;
  }
  return false;
}

bool key::is_down(const std::chrono::system_clock::time_point& time_point,
                  const int& keycode)
{
  auto it = std::find_if(std::begin(key_deque_), std::end(key_deque_),
      [&](const std::pair<std::chrono::system_clock::time_point,
                          std::array<int, key::KEY_TABLE_LENGTH> >& x) {
          return x.first == time_point;
  });

  if (!((it+1)->second[keycode] & 0x80) &&
      (it->second[keycode] & 0x80)) {
    return true;
  }
  return false;
}

bool key::is_hold(const int& keycode)
{
  if (key_deque_.size() < 2) {
    return false;
  }
  if (key_deque_.at(0).second[keycode] & 0x80) {
    return true;
  }
  return false;
}

bool key::is_up(const int& keycode)
{
  if (key_deque_.size() < 2) {
    return false;
  }
  if ((key_deque_.at(1).second[keycode] & 0x80) &&
      !(key_deque_.at(0).second[keycode] & 0x80)) {
    return true;
  }
  return false;
}


} /* namespace early_go */
