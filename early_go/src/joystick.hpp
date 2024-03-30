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
    enum ButtonKind
    {
        UpButton,
        DownButton,
        LeftButton,
        RightButton,
        Button01,
        Button02,
        // TODO Add Button03, Button04
        ButtonKindMax,
    };

    enum ButtonState
    {
        ButtonStateNone,
        ButtonStateDown,
        ButtonStatePush,
        ButtonStateUp,
        ButtonStateMax,
    };

    bool initialize();
    void finalize();
    void update();
    bool is_hold(ButtonKind button);
    bool is_up(ButtonKind button);
    bool is_down(ButtonKind button);

    static LPDIRECTINPUT8 g_InputInterface;
    static LPDIRECTINPUTDEVICE8 g_GamePadDevice;
    static ButtonState g_ButtonStates[ButtonKind::ButtonKindMax];
private:

// ゲームパッドデバイスの作成
bool create_device();

BOOL StartGamePadControl();

};
} // namespace early_go 

