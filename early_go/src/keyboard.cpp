#include "stdafx.hpp"
#include "keyboard.hpp"

using std::chrono::system_clock;
using std::find_if;
using std::shared_ptr;

namespace early_go
{
std::deque<keyboard::key_info> keyboard::key_deque_;
const int keyboard::SIMULTANEOUS_ALLOW_FRAME;

bool keyboard::update()
{
    key_info _key_info { };

    // Do not use GetKeyboardState function because GetKeyboardState function does not work when
    // using with cv::flip function.
    for (int i = 0; i < KEY_TABLE_LENGTH; ++i)
    {
        _key_info.key_table_.at(i) = GetAsyncKeyState(i);
    }
    key_deque_.push_front(_key_info);

    if (key_deque_.size() > KEY_DEQUE_MAX_SIZE)
    {
        key_deque_.erase(key_deque_.begin() + KEY_DEQUE_MAX_SIZE, key_deque_.end());
    }
    return true;
}

bool keyboard::is_down(const int keycode)
{
    if (key_deque_.size() <= 1)
    {
        return false;
    }
    if (!(key_deque_.at(1).key_table_[keycode] & 0x0001) &&
        (key_deque_.at(0).key_table_[keycode] & 0x0001))
    {
        return true;
    }
    return false;
}

bool keyboard::is_hold(const int keycode)
{
    if (key_deque_.size() <= 1)
    {
        return false;
    }
    if (key_deque_.at(0).key_table_[keycode] & 0x8000)
    {
        return true;
    }
    return false;
}

bool keyboard::is_up(const int keycode)
{
    if (key_deque_.size() <= 1)
    {
        return false;
    }
    if ((key_deque_.at(1).key_table_[keycode] & 0x0001) &&
        !(key_deque_.at(0).key_table_[keycode] & 0x0001))
    {
        return true;
    }
    return false;
}

bool keyboard::check_simultaneous(const int keycode)
{
    for (int i = 0; i < SIMULTANEOUS_ALLOW_FRAME; ++i)
    {
        if (i >= key_deque_.size())
        {
            break;
        }
        if (key_deque_.at(i).key_table_[keycode] & 0x0001)
        {
            return true;
        }
    }
    return false;
}
} // namespace early_go 
