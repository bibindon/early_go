#ifndef KEY_HPP
#define KEY_HPP
#include "stdafx.hpp"

#include <chrono>

namespace early_go
{
struct key_info;

// Monostate pattern.
class key
{
public:
    static constexpr int KEY_TABLE_LENGTH{256};
    static constexpr int KEY_DEQUE_MAX_SIZE{300};
    const static std::chrono::milliseconds SIMULTANEOUS_ALLOW_MILLI_SEC;

    static std::deque<key_info> key_deque_;

    static bool update();
    static bool is_down(const int &);
    static bool is_down(const std::chrono::system_clock::time_point &, const int &);
    static bool is_hold(const int &);
    static bool is_up(const int &);

    static bool check_simultaneous(const int& keycode);
};
struct key_info
{
    std::chrono::system_clock::time_point time_point_;
    std::array<SHORT, key::KEY_TABLE_LENGTH> key_table_;
};
} // namespace early_go 
#endif
