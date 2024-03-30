#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include "stdafx.hpp"

#include <dinput.h>

namespace early_go
{
// Monostate pattern.
class joystick
{
public:
    enum button_kind_type
    {
        UP_BUTTON,
        DOWN_BUTTON,
        LEFT_BUTTON,
        RIGHT_BUTTON,
        BUTTON_X,
        BUTTON_Y,
        BUTTON_A,
        BUTTON_B,
        BUTTON_KIND_MAX,
    };

    enum button_state_type
    {
        BUTTON_STATE_NONE,
        BUTTON_STATE_DOWN,
        BUTTON_STATE_PUSH,
        BUTTON_STATE_UP,
        BUTTON_STATE_MAX,
    };

    static bool initialize();
    static void finalize();
    static void update();
    static bool is_hold(button_kind_type button);
    static bool is_up(button_kind_type button);
    static bool is_down(button_kind_type button);

    static LPDIRECTINPUT8 direct_input8_;
    static LPDIRECTINPUTDEVICE8 input_device8_;
    static button_state_type button_states_[button_kind_type::BUTTON_KIND_MAX];

private:
    static bool create_device();
    static BOOL start_game_pad_control();

};
} // namespace early_go 

