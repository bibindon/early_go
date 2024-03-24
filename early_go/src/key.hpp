#ifndef KEY_HPP
#define KEY_HPP
#include "stdafx.hpp"

#include <chrono>

namespace early_go
{
// Monostate pattern.
class key
{
public:
    static constexpr int KEY_TABLE_LENGTH{256};
    static constexpr int KEY_DEQUE_MAX_SIZE{300};
    static constexpr int SIMULTANEOUS_ALLOW_FRAME{3};

    static bool update();
    static bool is_down(const int &);
    static bool is_down(const std::chrono::system_clock::time_point &, const int &);
    static bool is_down(const int&, const int &);
    static bool is_hold(const int &);
    static bool is_up(const int &);
    static bool check_simultaneous(const int& keycode);

private:
    struct key_info;
    static std::deque<std::shared_ptr<key_info>> key_deque_;
};
} // namespace early_go 
#endif
