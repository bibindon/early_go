#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "joystick.hpp"

using std::unordered_map;
using std::shared_ptr;

namespace early_go
{
BOOL CALLBACK device_find_callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
BOOL setup_gamepad_property(LPDIRECTINPUTDEVICE8 device);

shared_ptr<IDirectInput8> joystick::direct_input8_;
shared_ptr<IDirectInputDevice8> joystick::input_device8_;
unordered_map<joystick_button_type, joystick_button_state> joystick::button_states_
{
    { joystick_button_type::UP, joystick_button_state::NONE },
    { joystick_button_type::DOWN, joystick_button_state::NONE },
    { joystick_button_type::LEFT, joystick_button_state::NONE },
    { joystick_button_type::RIGHT, joystick_button_state::NONE },
    { joystick_button_type::X, joystick_button_state::NONE },
    { joystick_button_type::Y, joystick_button_state::NONE },
    { joystick_button_type::A, joystick_button_state::NONE },
    { joystick_button_type::B, joystick_button_state::NONE },
};

bool joystick::initialize(HWND hwnd)
{
    // Create input interface.
    LPDIRECTINPUT8 temp_direct_input { nullptr };
    HRESULT result { DirectInput8Create(
        GetModuleHandle(nullptr),
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&temp_direct_input,
        nullptr) };

    if (FAILED(result))
    {
        THROW_WITH_TRACE("Failed to create direct input.");
        return false;
    }
    direct_input8_.reset(temp_direct_input, custom_deleter { });

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
    if (input_device8_.get() != nullptr)
    {
        input_device8_->Unacquire();
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
    if (input_device8_.get() == nullptr)
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
    if (input_device8_.get() == nullptr)
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
                x.second = joystick_button_state::NONE;
            }
            input_device8_->Poll();
        }
        return;
    }

    unordered_map<joystick_button_type, bool> is_push_map {
        { joystick_button_type::UP, false },
        { joystick_button_type::DOWN, false },
        { joystick_button_type::LEFT, false },
        { joystick_button_type::RIGHT, false },
        { joystick_button_type::X, false },
        { joystick_button_type::Y, false },
        { joystick_button_type::A, false },
        { joystick_button_type::B, false },
    };

    // Check stick.
    int unresponsive_range { 200 };
    if (pad_data.lX < -unresponsive_range)
    {
        is_push_map.at(joystick_button_type::LEFT) = true;
    }
    else if (pad_data.lX > unresponsive_range)
    {
        is_push_map.at(joystick_button_type::RIGHT) = true;
    }

    if (pad_data.lY < -unresponsive_range)
    {
        is_push_map.at(joystick_button_type::UP) = true;
    }
    else if (pad_data.lY > unresponsive_range)
    {
        is_push_map.at(joystick_button_type::DOWN) = true;
    }

    // Check D-pad
    if (pad_data.rgdwPOV[0] != 0xFFFFFFFF)
    {
        float rad { D3DXToRadian(pad_data.rgdwPOV[0] / 100.0f) };
        float x { sinf(rad) };
        float y { cosf(rad) };

        if (x < -0.01f)
        {
            is_push_map.at(joystick_button_type::LEFT) = true;
        }
        else if (x > 0.01f)
        {
            is_push_map.at(joystick_button_type::RIGHT) = true;
        }

        if (y > 0.01f)
        {
            is_push_map.at(joystick_button_type::UP) = true;
        }
        else if (y < -0.01f)
        {
            is_push_map.at(joystick_button_type::DOWN) = true;
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
            is_push_map.at(joystick_button_type::X) = true;
            break;
        case 1:
            is_push_map.at(joystick_button_type::A) = true;
            break;
        case 2:
            is_push_map.at(joystick_button_type::B) = true;
            break;
        case 3:
            is_push_map.at(joystick_button_type::Y) = true;
            break;
        }
    }

    // Update button status.
    for (auto& _is_push : is_push_map)
    {
        if (_is_push.second == true)
        {
            if (button_states_.at(_is_push.first) == joystick_button_state::NONE)
            {
                button_states_.at(_is_push.first) = joystick_button_state::DOWN;
            }
            else
            {
                button_states_.at(_is_push.first) = joystick_button_state::HOLD;
            }
        }
        else
        {
            if (button_states_.at(_is_push.first) == joystick_button_state::HOLD)
            {
                button_states_.at(_is_push.first) = joystick_button_state::UP;
            }
            else
            {
                button_states_.at(_is_push.first) = joystick_button_state::NONE;
            }
        }
    }
}

bool joystick::is_hold(joystick_button_type button)
{
    if (button_states_.at(button) == joystick_button_state::HOLD)
    {
        return true;
    }
    return false;
}

bool joystick::is_up(joystick_button_type button)
{
    if (button_states_.at(button) == joystick_button_state::UP)
    {
        return true;
    }
    return false;
}

bool joystick::is_down(joystick_button_type button)
{
    if (button_states_.at(button) == joystick_button_state::DOWN)
    {
        return true;
    }
    return false;
}

bool joystick::check_simultaneous(joystick_button_type button)
{
    return false;
}

bool joystick::create_device(HWND hwnd)
{
    device_enum_param parameter = { 0 };

    LPDIRECTINPUTDEVICE8 temp_device { nullptr };
    parameter.arg_input_device8_ = &temp_device;
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

    input_device8_.reset(temp_device, custom_deleter { });

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
