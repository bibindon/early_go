#ifndef KEY_HPP
#define KEY_HPP
#include "stdafx.hpp"

#include <chrono>

#include <gtest/gtest_prod.h>

namespace early_go
{
static constexpr int KEY_TABLE_LENGTH { 256 };
static constexpr int KEY_DEQUE_MAX_SIZE { 300 };
static constexpr int SIMULTANEOUS_ALLOW_FRAME { 3 };
// Monostate pattern.
class keyboard
{
public:
    static bool update();
    static bool is_down(const int);
    static bool is_hold(const int);
    static bool is_up(const int);
    static bool check_simultaneous(const int keycode);

private:
    // key_info must be defined in this header file, not in a cpp file because the key_info must
    // be visible from the operation_test.
    struct key_info
    {
        std::array<SHORT, KEY_TABLE_LENGTH> key_table_;
    };

    static std::deque<key_info> key_deque_;

    FRIEND_TEST(mesh_test, mesh_ctor_test);
    FRIEND_TEST(operation_test, operation_test01);
    FRIEND_TEST(operation_test, operation_test02);
    FRIEND_TEST(operation_test, operation_test03);
    FRIEND_TEST(operation_test, operation_test04);
    FRIEND_TEST(operation_test, operation_test05);
    FRIEND_TEST(operation_test, operation_test06);
    FRIEND_TEST(operation_test, operation_test07);
    FRIEND_TEST(operation_test, operation_test08);
    FRIEND_TEST(operation_test, operation_test09);
    FRIEND_TEST(operation_test, operation_test10);
};
} // namespace early_go 
#endif
