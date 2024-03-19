#include "stdafx.hpp"
#include "key.hpp"

using std::chrono::system_clock;

namespace early_go
{
const int key::KEY_DEQUE_MAX_SIZE = 300;
std::deque<std::pair<
    system_clock::time_point, std::array<SHORT, key::KEY_TABLE_LENGTH> > > key::key_deque_;

bool key::update()
{
    std::array<SHORT, KEY_TABLE_LENGTH> current_key_table;
    // Do not use GetKeyboardState function because GetKeyboardState function does not work when
    // using with cv::flip function.
    for (int i = 0; i < KEY_TABLE_LENGTH; ++i)
    {
        current_key_table.at(i) = GetAsyncKeyState(i);
    }

    system_clock::time_point update_time{system_clock::now()};
    key_deque_.push_front(std::make_pair(update_time, current_key_table));

    if (key_deque_.size() > KEY_DEQUE_MAX_SIZE)
    {
        key_deque_.erase(key_deque_.begin() + KEY_DEQUE_MAX_SIZE, key_deque_.end());
    }

    return true;
}

bool key::is_down(const int &keycode)
{
    if (key_deque_.size() <= 1)
    {
        return false;
    }
    if (!(key_deque_.at(1).second[keycode] & 0x8000) &&
        (key_deque_.at(0).second[keycode] & 0x8000))
    {
        return true;
    }
    return false;
}

bool key::is_down(const system_clock::time_point &time_point,
                  const int &keycode)
{
    auto it = std::find_if(
        std::begin(key_deque_), std::end(key_deque_),
        [&](const std::pair<system_clock::time_point, std::array<SHORT, key::KEY_TABLE_LENGTH>> &x)
        { return x.first == time_point; });

    if (!((it + 1)->second[keycode] & 0x8000) && (it->second[keycode] & 0x8000))
    {
        return true;
    }
    return false;
}

bool key::is_hold(const int &keycode)
{
    if (key_deque_.size() < 2)
    {
        return false;
    }
    if (key_deque_.at(0).second[keycode] & 0x8000)
    {
        return true;
    }
    return false;
}

bool key::is_up(const int &keycode)
{
    if (key_deque_.size() < 2)
    {
        return false;
    }
    if ((key_deque_.at(1).second[keycode] & 0x8000) &&
        !(key_deque_.at(0).second[keycode] & 0x8000))
    {
        return true;
    }
    return false;
}

} // namespace early_go 
