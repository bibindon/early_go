#ifndef KEY_HPP
#define KEY_HPP
#include "stdafx.hpp"

#include <chrono>

#include "gtest/gtest_prod.h"

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
    static bool is_down(const int&, const int &);
    static bool is_hold(const int &);
    static bool is_up(const int &);
    static bool check_simultaneous(const int& keycode);

private:
    // key_info must be defined in this header file, not in a cpp file because the key_info must
    // be visible from the operation_test.
    struct key_info
    {
        std::array<SHORT, key::KEY_TABLE_LENGTH> key_table_;
    };

    static std::deque<std::shared_ptr<key_info>> key_deque_;

    FRIEND_TEST(operation_test, operation_test);
};
} // namespace early_go 
#endif
