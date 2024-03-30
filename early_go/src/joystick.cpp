#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "joystick.hpp"

namespace early_go
{
BOOL CALLBACK DeviceFindCallBack(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
BOOL SetUpGamePadProperty(LPDIRECTINPUTDEVICE8 device);

LPDIRECTINPUT8 joystick::g_InputInterface;
LPDIRECTINPUTDEVICE8 joystick::g_GamePadDevice;
joystick::ButtonState joystick::g_ButtonStates[ButtonKind::ButtonKindMax];

bool joystick::initialize()
{
    // Create input interface.
    HRESULT result { DirectInput8Create(
        GetModuleHandle(nullptr),
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&g_InputInterface,
        nullptr) };

    if (FAILED(result))
    {
        THROW_WITH_TRACE("Failed to create direct input.");
        return false;
    }

    // Create device.
    if (!create_device())
    {
        finalize();
        THROW_WITH_TRACE("Failed to create direct input device.");
        return false;
    }

    // 入力情報の初期化
    for (int i = 0; i < ButtonKind::ButtonKindMax; i++)
    {
        g_ButtonStates[i] = ButtonState::ButtonStateNone;
    }

    return true;
}

void joystick::finalize()
{
    // デバイスの解放
    if (g_GamePadDevice != nullptr)
    {
        // 制御を停止
        g_GamePadDevice->Unacquire();
        g_GamePadDevice->Release();
        g_GamePadDevice = nullptr;
    }

    // インターフェースの解放
    if (g_InputInterface != nullptr)
    {
        g_InputInterface->Release();
        g_InputInterface = nullptr;
    }
}

//ゲームパッドデバイスの作成-デバイス列挙の結果を受け取る構造体
struct DeviceEnumParameter
{
    LPDIRECTINPUTDEVICE8* input_device8_;
    int find_count_;
};

BOOL joystick::StartGamePadControl()
{
    // デバイスが生成されてない
    if (g_GamePadDevice == nullptr)
    {
        return false;
    }

    // 制御開始
    if (FAILED(g_GamePadDevice->Acquire()))
    {
        return false;
    }

    DIDEVCAPS cap;
    g_GamePadDevice->GetCapabilities(&cap);
    // ポーリング判定
    if (cap.dwFlags & DIDC_POLLEDDATAFORMAT)
    {
        DWORD error = GetLastError();
        // ポーリング開始
        /*
            PollはAcquireの前に行うとされていたが、
            Acquireの前で実行すると失敗したので
            後で実行するようにした
        */
        if (FAILED(g_GamePadDevice->Poll()))
        {
            return false;
        }
    }

    return true;
}

BOOL SetUpGamePadProperty(LPDIRECTINPUTDEVICE8 device)
{
    // 軸モードを絶対値モードとして設定
    DIPROPDWORD diprop;
    ZeroMemory(&diprop, sizeof(diprop));
    diprop.diph.dwSize = sizeof(diprop);
    diprop.diph.dwHeaderSize = sizeof(diprop.diph);
    diprop.diph.dwHow = DIPH_DEVICE;
    diprop.diph.dwObj = 0;
    diprop.dwData = DIPROPAXISMODE_ABS;
    if (FAILED(device->SetProperty(DIPROP_AXISMODE, &diprop.diph)))
    {
        return false;
    }

    // X軸の値の範囲設定
    DIPROPRANGE diprg;
    ZeroMemory(&diprg, sizeof(diprg));
    diprg.diph.dwSize = sizeof(diprg);
    diprg.diph.dwHeaderSize = sizeof(diprg.diph);
    diprg.diph.dwHow = DIPH_BYOFFSET;
    diprg.diph.dwObj = DIJOFS_X;
    diprg.lMin = -1000;
    diprg.lMax = 1000;
    if (FAILED(device->SetProperty(DIPROP_RANGE, &diprg.diph)))
    {
        return false;
    }

    // Y軸の値の範囲設定
    diprg.diph.dwObj = DIJOFS_Y;
    if (FAILED(device->SetProperty(DIPROP_RANGE, &diprg.diph)))
    {
        return false;
    }

    return true;
}

BOOL CALLBACK DeviceFindCallBack(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
    DeviceEnumParameter* parameter { (DeviceEnumParameter*)pvRef };
    LPDIRECTINPUTDEVICE8 device { nullptr };

    // Finish if already found.
    if (parameter->find_count_ >= 1)
    {
        return DIENUM_STOP;
    }

    // デバイス生成
    HRESULT hresult { joystick::g_InputInterface->CreateDevice(
        lpddi->guidInstance,
        parameter->input_device8_,
        nullptr) };

    if (FAILED(hresult))
    {
        return DIENUM_STOP;
    }

    // 入力フォーマットの指定
    device = *parameter->input_device8_;
    hresult = device->SetDataFormat(&c_dfDIJoystick);

    if (FAILED(hresult))
    {
        return DIENUM_STOP;
    }

    // プロパティの設定
    if (SetUpGamePadProperty(device) == false)
    {
        return DIENUM_STOP;
    }

    // TODO hwnd
    // Set Cooperative level
    if (FAILED( device->SetCooperativeLevel(
        FindWindow(constants::APP_NAME.c_str(), nullptr),
        DISCL_EXCLUSIVE | DISCL_FOREGROUND)))
    {
        return DIENUM_STOP;
    }

    ++parameter->find_count_;

    return DIENUM_CONTINUE;
}



void joystick::update()
{
    if (g_GamePadDevice == nullptr)
    {
        return;
    }

    DIJOYSTATE pad_data;

    // デバイス取得
    HRESULT hr = g_GamePadDevice->GetDeviceState(sizeof(DIJOYSTATE), &pad_data);
    if (FAILED(hr))
    {
        // 再度制御開始
        if (FAILED(g_GamePadDevice->Acquire()))
        {
            for (int i = 0; i < ButtonKind::ButtonKindMax; i++)
            {
                g_ButtonStates[i] = ButtonState::ButtonStateNone;
            }
            g_GamePadDevice->Poll();
        }
        return;
    }

    bool is_push[ButtonKind::ButtonKindMax];
    // スティック判定
    int unresponsive_range = 200;
    if (pad_data.lX < -unresponsive_range)
    {
        is_push[ButtonKind::LeftButton] = true;
    }
    else if (pad_data.lX > unresponsive_range)
    {
        is_push[ButtonKind::RightButton] = true;
    }

    if (pad_data.lY < -unresponsive_range)
    {
        is_push[ButtonKind::UpButton] = true;
    }
    else if (pad_data.lY > unresponsive_range)
    {
        is_push[ButtonKind::DownButton] = true;
    }

    // 十字キー判定
    if (pad_data.rgdwPOV[0] != 0xFFFFFFFF)
    {
        float rad = D3DXToRadian((pad_data.rgdwPOV[0] / 100.0f));
        // 本来はxがcos、yがsinだけど、rgdwPOVは0が上から始まるので、
        // cosとsinを逆にした方が都合がいい
        float x = sinf(rad);
        float y = cosf(rad);

        if (x < -0.01f)
        {
            is_push[ButtonKind::LeftButton] = true;
        }
        else if (x > 0.01f)
        {
            is_push[ButtonKind::RightButton] = true;
        }

        if (y > 0.01f)
        {
            is_push[ButtonKind::UpButton] = true;
        }
        else if (y < -0.01f)
        {
            is_push[ButtonKind::DownButton] = true;
        }
    }

    // ボタン判定
    for (int i = 0; i < 32; i++)
    {
        if (!(pad_data.rgbButtons[i] & 0x80))
        {
            continue;
        }

        switch (i)
        {
        case 0:
            is_push[ButtonKind::Button01] = true;
            break;
        case 1:
            is_push[ButtonKind::Button02] = true;
            break;
        }
    }

    // 入力情報からボタンの状態を更新する
    for (int i = 0; i < ButtonKind::ButtonKindMax; i++)
    {
        if (is_push[i] == true)
        {
            if (g_ButtonStates[i] == ButtonState::ButtonStateNone)
            {
                g_ButtonStates[i] = ButtonState::ButtonStateDown;
            }
            else
            {
                g_ButtonStates[i] = ButtonState::ButtonStatePush;
            }
        }
        else
        {
            if (g_ButtonStates[i] == ButtonState::ButtonStatePush)
            {
                g_ButtonStates[i] = ButtonState::ButtonStateUp;
            }
            else
            {
                g_ButtonStates[i] = ButtonState::ButtonStateNone;
            }
        }
    }
}

bool joystick::is_hold(ButtonKind button)
{
    if (g_ButtonStates[button] == ButtonState::ButtonStatePush)
    {
        return true;
    }

    return false;
}

bool joystick::is_up(ButtonKind button)
{
    if (g_ButtonStates[button] == ButtonState::ButtonStateUp)
    {
        return true;
    }

    return false;
}

bool joystick::is_down(ButtonKind button)
{
    if (g_ButtonStates[button] == ButtonState::ButtonStateDown)
    {
        return true;
    }

    return false;
}

bool joystick::create_device()
{
    DeviceEnumParameter parameter = { 0 };

    parameter.input_device8_ = &g_GamePadDevice;

    // Search GAMEPAD
    g_InputInterface->EnumDevices(
        DI8DEVTYPE_GAMEPAD,
        DeviceFindCallBack,
        &parameter,
        DIEDFL_ATTACHEDONLY
    );

    // Search JOYSTICK
    g_InputInterface->EnumDevices(
        DI8DEVTYPE_JOYSTICK,
        DeviceFindCallBack,
        &parameter,
        DIEDFL_ATTACHEDONLY
    );

    if (parameter.find_count_ == 0)
    {
        return false;
    }

    int count = 0;
    // 制御開始
    while (StartGamePadControl() == false)
    {
        Sleep(100);
        count++;
        if (count >= 5)
        {
            break;
        }
    }

    return true;
}

}
