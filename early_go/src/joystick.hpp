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
    enum class button_kind_type
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

    enum class button_state_type
    {
        NONE,
        DOWN,
        PUSH,
        UP,
        MAX,
    };

    static bool initialize(HWND hwnd);
    static void finalize();
    static void update();
    static bool is_hold(button_kind_type button);
    static bool is_up(button_kind_type button);
    static bool is_down(button_kind_type button);

    static std::shared_ptr<IDirectInput8> direct_input8_;
    //static LPDIRECTINPUTDEVICE8 input_device8_;
    static std::shared_ptr<IDirectInputDevice8> input_device8_;
    static std::unordered_map<button_kind_type, button_state_type> button_states_;

private:
    static bool create_device(HWND hwnd);
    static BOOL start_game_pad_control();

};
} // namespace early_go 

