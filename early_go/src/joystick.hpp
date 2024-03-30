#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include "stdafx.hpp"

#include <dinput.h>

namespace early_go
{
// Monostate pattern.
enum class joystick_button_type
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    X,
    Y,
    A,
    B,
    UNKNOWN,
};
static constexpr int BUTTON_KIND_MAX { 8 };

enum class joystick_button_state
{
    NONE,
    DOWN,
    HOLD,
    UP,
    MAX,
};
class joystick
{
public:

    static bool initialize(HWND hwnd);
    static void finalize();
    static void update();
    static bool is_hold(joystick_button_type button);
    static bool is_up(joystick_button_type button);
    static bool is_down(joystick_button_type button);
    static bool check_simultaneous(joystick_button_type button);

    static std::shared_ptr<IDirectInput8> direct_input8_;
    static std::shared_ptr<IDirectInputDevice8> input_device8_;
    static std::unordered_map<joystick_button_type, joystick_button_state> button_states_;

private:
    static bool create_device(HWND hwnd);
    static BOOL start_game_pad_control();

};
} // namespace early_go 

