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

    static bool initialize();
    static bool update();

    bool InitInput();
    void ReleaseInput();
    void UpdateInput();
    bool IsButtonPush(ButtonKind button);
    bool IsButtonUp(ButtonKind button);
    bool IsButtonDown(ButtonKind button);

    static LPDIRECTINPUT8 g_InputInterface;
    static LPDIRECTINPUTDEVICE8 g_GamePadDevice;
    static ButtonState g_ButtonStates[ButtonKind::ButtonKindMax];
private:



// 入力インターフェースの作成
bool CreateInputInterface();

// ゲームパッドデバイスの作成
bool CreateGamePadDevice();

// ゲームパッドの更新
void UpdateGamePad();

BOOL StartGamePadControl();

};
} // namespace early_go 

