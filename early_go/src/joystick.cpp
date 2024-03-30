#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "joystick.hpp"

namespace early_go
{
BOOL CALLBACK device_find_callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
BOOL setup_gamepad_property(LPDIRECTINPUTDEVICE8 device);

LPDIRECTINPUT8 joystick::direct_input8_;
LPDIRECTINPUTDEVICE8 joystick::input_device8_;
joystick::button_state_type joystick::button_states_[button_kind_type::BUTTON_KIND_MAX];

bool joystick::initialize()
{
    // Create input interface.
    HRESULT result { DirectInput8Create(
        GetModuleHandle(nullptr),
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&direct_input8_,
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
    for (int i = 0; i < button_kind_type::BUTTON_KIND_MAX; i++)
    {
        button_states_[i] = button_state_type::BUTTON_STATE_NONE;
    }

    return true;
}

void joystick::finalize()
{
    // デバイスの解放
    if (input_device8_ != nullptr)
    {
        // 制御を停止
        input_device8_->Unacquire();
        input_device8_->Release();
        input_device8_ = nullptr;
    }

    // インターフェースの解放
    if (direct_input8_ != nullptr)
    {
        direct_input8_->Release();
        direct_input8_ = nullptr;
    }
}

//ゲームパッドデバイスの作成-デバイス列挙の結果を受け取る構造体
struct device_enum_param
{
    LPDIRECTINPUTDEVICE8* arg_input_device8_;
    int find_count_;
};


BOOL joystick::start_game_pad_control()
{
    // デバイスが生成されてない
    if (input_device8_ == nullptr)
    {
        return false;
    }

    // 制御開始
    if (FAILED(input_device8_->Acquire()))
    {
        return false;
    }

    DIDEVCAPS cap;
    input_device8_->GetCapabilities(&cap);
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
        if (FAILED(input_device8_->Poll()))
        {
            return false;
        }
    }

    return true;
}

BOOL CALLBACK device_find_callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
    device_enum_param* parameter { (device_enum_param*)pvRef };
    LPDIRECTINPUTDEVICE8 device { nullptr };

    // Finish if already found.
    if (parameter->find_count_ >= 1)
    {
        return DIENUM_STOP;
    }

    // デバイス生成
    HRESULT hresult { joystick::direct_input8_->CreateDevice(
        lpddi->guidInstance,
        parameter->arg_input_device8_,
        nullptr) };

    if (FAILED(hresult))
    {
        return DIENUM_STOP;
    }

    // 入力フォーマットの指定
    device = *parameter->arg_input_device8_;
    hresult = device->SetDataFormat(&c_dfDIJoystick);

    if (FAILED(hresult))
    {
        return DIENUM_STOP;
    }

    // プロパティの設定
    if (setup_gamepad_property(device) == false)
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
    if (input_device8_ == nullptr)
    {
        return;
    }

    DIJOYSTATE pad_data;

    // デバイス取得
    HRESULT hr = input_device8_->GetDeviceState(sizeof(DIJOYSTATE), &pad_data);
    if (FAILED(hr))
    {
        // 再度制御開始
        if (FAILED(input_device8_->Acquire()))
        {
            for (int i = 0; i < button_kind_type::BUTTON_KIND_MAX; i++)
            {
                button_states_[i] = button_state_type::BUTTON_STATE_NONE;
            }
            input_device8_->Poll();
        }
        return;
    }

    bool is_push[button_kind_type::BUTTON_KIND_MAX];
    // スティック判定
    int unresponsive_range = 200;
    if (pad_data.lX < -unresponsive_range)
    {
        is_push[button_kind_type::LEFT_BUTTON] = true;
    }
    else if (pad_data.lX > unresponsive_range)
    {
        is_push[button_kind_type::RIGHT_BUTTON] = true;
    }

    if (pad_data.lY < -unresponsive_range)
    {
        is_push[button_kind_type::UP_BUTTON] = true;
    }
    else if (pad_data.lY > unresponsive_range)
    {
        is_push[button_kind_type::DOWN_BUTTON] = true;
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
            is_push[button_kind_type::LEFT_BUTTON] = true;
        }
        else if (x > 0.01f)
        {
            is_push[button_kind_type::RIGHT_BUTTON] = true;
        }

        if (y > 0.01f)
        {
            is_push[button_kind_type::UP_BUTTON] = true;
        }
        else if (y < -0.01f)
        {
            is_push[button_kind_type::DOWN_BUTTON] = true;
        }
    }

    // Check button.
    for (int i = 0; i < 32; i++)
    {
        if (!(pad_data.rgbButtons[i] & 0x80))
        {
            continue;
        }

        switch (i)
        {
        case 0:
            is_push[button_kind_type::BUTTON_X] = true;
            break;
        case 1:
            is_push[button_kind_type::BUTTON_A] = true;
            break;
        case 2:
            is_push[button_kind_type::BUTTON_B] = true;
            break;
        case 3:
            is_push[button_kind_type::BUTTON_Y] = true;
            break;
        }
    }

    // 入力情報からボタンの状態を更新する
    for (int i = 0; i < button_kind_type::BUTTON_KIND_MAX; i++)
    {
        if (is_push[i] == true)
        {
            if (button_states_[i] == button_state_type::BUTTON_STATE_NONE)
            {
                button_states_[i] = button_state_type::BUTTON_STATE_DOWN;
            }
            else
            {
                button_states_[i] = button_state_type::BUTTON_STATE_PUSH;
            }
        }
        else
        {
            if (button_states_[i] == button_state_type::BUTTON_STATE_PUSH)
            {
                button_states_[i] = button_state_type::BUTTON_STATE_UP;
            }
            else
            {
                button_states_[i] = button_state_type::BUTTON_STATE_NONE;
            }
        }
    }
}

bool joystick::is_hold(button_kind_type button)
{
    if (button_states_[button] == button_state_type::BUTTON_STATE_PUSH)
    {
        return true;
    }

    return false;
}

bool joystick::is_up(button_kind_type button)
{
    if (button_states_[button] == button_state_type::BUTTON_STATE_UP)
    {
        return true;
    }

    return false;
}

bool joystick::is_down(button_kind_type button)
{
    if (button_states_[button] == button_state_type::BUTTON_STATE_DOWN)
    {
        return true;
    }

    return false;
}

bool joystick::create_device()
{
    device_enum_param parameter = { 0 };

    parameter.arg_input_device8_ = &input_device8_;

    // Search GAMEPAD
    direct_input8_->EnumDevices(
        DI8DEVTYPE_GAMEPAD,
        device_find_callback,
        &parameter,
        DIEDFL_ATTACHEDONLY
    );

    // Search JOYSTICK
    direct_input8_->EnumDevices(
        DI8DEVTYPE_JOYSTICK,
        device_find_callback,
        &parameter,
        DIEDFL_ATTACHEDONLY
    );

    if (parameter.find_count_ == 0)
    {
        return false;
    }

    int count = 0;
    // 制御開始
    while (start_game_pad_control() == false)
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

BOOL setup_gamepad_property(LPDIRECTINPUTDEVICE8 device)
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

}
