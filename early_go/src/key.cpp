#include "stdafx.hpp"
#include "key.hpp"

using std::chrono::system_clock;
using std::find_if;

namespace early_go
{
std::deque<key_info> key::key_deque_;
const std::chrono::milliseconds key::SIMULTANEOUS_ALLOW_MILLI_SEC = std::chrono::milliseconds(50);

bool key::update()
{
    key_info _key_info = {};

    _key_info.time_point_ = system_clock::now();

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

bool key::is_down(const int &keycode)
{
    if (key_deque_.size() <= 1)
    {
        return false;
    }
    if (!(key_deque_.at(1).key_table_[keycode] & 0x8000) &&
        (key_deque_.at(0).key_table_[keycode] & 0x8000))
    {
        return true;
    }
    return false;
}

bool key::is_down(const system_clock::time_point &time_point, const int &keycode)
{
    auto it = find_if(
        std::begin(key_deque_), std::end(key_deque_),
        [&](const key_info& x)
        {
            return x.time_point_ == time_point;
        });

    if (!((it + 1)->key_table_[keycode] & 0x8000) && (it->key_table_[keycode] & 0x8000))
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
    if (key_deque_.at(0).key_table_[keycode] & 0x8000)
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
    if ((key_deque_.at(1).key_table_[keycode] & 0x8000) &&
        !(key_deque_.at(0).key_table_[keycode] & 0x8000))
    {
        return true;
    }
    return false;
}

bool key::check_simultaneous(const int& keycode)
{
    auto time_out_it = find_if(
        std::begin(key_deque_), std::end(key_deque_),
        [&](const key_info& x)
        {
            system_clock::time_point time_out{ key_deque_.at(0).time_point_ };
            time_out -= SIMULTANEOUS_ALLOW_MILLI_SEC;

            return time_out > x.time_point_;
        });

    auto keycode_it = find_if(
        std::begin(key_deque_), time_out_it,
        [&](const key_info &x)
        {
            return is_down(x.time_point_, keycode);
        });

    if (keycode_it != time_out_it)
    {
        return true;
    }
    return false;
}

} // namespace early_go 
