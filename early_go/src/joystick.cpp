#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "joystick.hpp"

using std::unordered_map;

namespace early_go
{
BOOL CALLBACK device_find_callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
BOOL setup_gamepad_property(LPDIRECTINPUTDEVICE8 device);

LPDIRECTINPUT8 joystick::direct_input8_;
LPDIRECTINPUTDEVICE8 joystick::input_device8_;
unordered_map<joystick::button_kind_type, joystick::button_state_type> joystick::button_states_
{
    { joystick::button_kind_type::UP, joystick::button_state_type::NONE },
    { joystick::button_kind_type::DOWN, joystick::button_state_type::NONE },
    { joystick::button_kind_type::LEFT, joystick::button_state_type::NONE },
    { joystick::button_kind_type::RIGHT, joystick::button_state_type::NONE },
    { joystick::button_kind_type::X, joystick::button_state_type::NONE },
    { joystick::button_kind_type::Y, joystick::button_state_type::NONE },
    { joystick::button_kind_type::A, joystick::button_state_type::NONE },
    { joystick::button_kind_type::B, joystick::button_state_type::NONE },
};

bool joystick::initialize(HWND hwnd)
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
    if (!create_device(hwnd))
    {
        finalize();
        THROW_WITH_TRACE("Failed to create direct input device.");
        return false;
    }
    return true;
}

void joystick::finalize()
{
    if (input_device8_ != nullptr)
    {
        input_device8_->Unacquire();
        input_device8_->Release();
        input_device8_ = nullptr;
    }

    if (direct_input8_ != nullptr)
    {
        direct_input8_->Release();
        direct_input8_ = nullptr;
    }
}

// A struct that receives device enumeration result.
struct device_enum_param
{
    LPDIRECTINPUTDEVICE8* arg_input_device8_;
    int find_count_;
    HWND hwnd_;
};

BOOL joystick::start_game_pad_control()
{
    if (input_device8_ == nullptr)
    {
        return false;
    }

    if (FAILED(input_device8_->Acquire()))
    {
        return false;
    }

    DIDEVCAPS cap = { 0 };
    input_device8_->GetCapabilities(&cap);
    // Check polling
    if (cap.dwFlags & DIDC_POLLEDDATAFORMAT)
    {
        DWORD error = GetLastError();
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

    // Create device.
    HRESULT hresult { joystick::direct_input8_->CreateDevice(
        lpddi->guidInstance,
        parameter->arg_input_device8_,
        nullptr) };

    if (FAILED(hresult))
    {
        return DIENUM_STOP;
    }

    // Set format.
    device = *parameter->arg_input_device8_;
    hresult = device->SetDataFormat(&c_dfDIJoystick);

    if (FAILED(hresult))
    {
        return DIENUM_STOP;
    }

    if (setup_gamepad_property(device) == false)
    {
        return DIENUM_STOP;
    }

    // Set Cooperative level
    if (FAILED( device->SetCooperativeLevel(
        parameter->hwnd_,
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

    DIJOYSTATE pad_data { };

    // Get device.
    HRESULT hr { input_device8_->GetDeviceState(sizeof(DIJOYSTATE), &pad_data) };
    if (FAILED(hr))
    {
        // Start control again.
        if (FAILED(input_device8_->Acquire()))
        {
            for (auto& x : button_states_)
            {
                x.second = button_state_type::NONE;
            }
            input_device8_->Poll();
        }
        return;
    }

    unordered_map<button_kind_type, bool> is_push_map {
        { button_kind_type::UP, false },
        { button_kind_type::DOWN, false },
        { button_kind_type::LEFT, false },
        { button_kind_type::RIGHT, false },
        { button_kind_type::X, false },
        { button_kind_type::Y, false },
        { button_kind_type::A, false },
        { button_kind_type::B, false },
    };

    // スティック判定
    int unresponsive_range = 200;
    if (pad_data.lX < -unresponsive_range)
    {
        is_push_map.at(button_kind_type::LEFT) = true;
    }
    else if (pad_data.lX > unresponsive_range)
    {
        is_push_map.at(button_kind_type::RIGHT) = true;
    }

    if (pad_data.lY < -unresponsive_range)
    {
        is_push_map.at(button_kind_type::UP) = true;
    }
    else if (pad_data.lY > unresponsive_range)
    {
        is_push_map.at(button_kind_type::DOWN) = true;
    }

    // Check D-pad
    if (pad_data.rgdwPOV[0] != 0xFFFFFFFF)
    {
        float rad { D3DXToRadian(pad_data.rgdwPOV[0] / 100.0f) };
        float x { sinf(rad) };
        float y { cosf(rad) };

        if (x < -0.01f)
        {
            is_push_map.at(button_kind_type::LEFT) = true;
        }
        else if (x > 0.01f)
        {
            is_push_map.at(button_kind_type::RIGHT) = true;
        }

        if (y > 0.01f)
        {
            is_push_map.at(button_kind_type::UP) = true;
        }
        else if (y < -0.01f)
        {
            is_push_map.at(button_kind_type::DOWN) = true;
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
            is_push_map.at(button_kind_type::X) = true;
            break;
        case 1:
            is_push_map.at(button_kind_type::A) = true;
            break;
        case 2:
            is_push_map.at(button_kind_type::B) = true;
            break;
        case 3:
            is_push_map.at(button_kind_type::Y) = true;
            break;
        }
    }

    // Update button status.
    for (auto& _is_push : is_push_map)
    {
        if (_is_push.second == true)
        {
            if (button_states_.at(_is_push.first) == button_state_type::NONE)
            {
                button_states_.at(_is_push.first) = button_state_type::DOWN;
            }
            else
            {
                button_states_.at(_is_push.first) = button_state_type::PUSH;
            }
        }
        else
        {
            if (button_states_.at(_is_push.first) == button_state_type::PUSH)
            {
                button_states_.at(_is_push.first) = button_state_type::UP;
            }
            else
            {
                button_states_.at(_is_push.first) = button_state_type::NONE;
            }
        }
    }
}

bool joystick::is_hold(button_kind_type button)
{
    if (button_states_.at(button) == button_state_type::PUSH)
    {
        return true;
    }
    return false;
}

bool joystick::is_up(button_kind_type button)
{
    if (button_states_.at(button) == button_state_type::UP)
    {
        return true;
    }
    return false;
}

bool joystick::is_down(button_kind_type button)
{
    if (button_states_.at(button) == button_state_type::DOWN)
    {
        return true;
    }
    return false;
}

bool joystick::create_device(HWND hwnd)
{
    device_enum_param parameter = { 0 };

    parameter.arg_input_device8_ = &input_device8_;
    parameter.hwnd_ = hwnd;

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

    int count { 0 };
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
    // Set coordinate type to abusolute value mode.
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

    // Set X ranges
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

    // Set Y ranges.
    diprg.diph.dwObj = DIJOFS_Y;
    if (FAILED(device->SetProperty(DIPROP_RANGE, &diprg.diph)))
    {
        return false;
    }
    return true;
}
}
