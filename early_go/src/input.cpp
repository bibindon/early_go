#include "input.hpp"

#include "keyboard.hpp"
#include "joystick.hpp"

namespace early_go
{
bool input::update()
{
    keyboard::update();
    joystick::update();
    return true;
}

bool input::is_down(const input_type _input_type)
{
    bool is_down_keyboard { false };
    bool is_down_joystick { false };

    if (_input_type == input_type::UP)
    {
        is_down_keyboard = keyboard::is_down('W');
        is_down_joystick = joystick::is_down(joystick_button_type::UP);
    }
    else if (_input_type == input_type::LEFT)
    {
        is_down_keyboard = keyboard::is_down('A');
        is_down_joystick = joystick::is_down(joystick_button_type::LEFT);
    }
    else if (_input_type == input_type::DOWN)
    {
        is_down_keyboard = keyboard::is_down('S');
        is_down_joystick = joystick::is_down(joystick_button_type::DOWN);
    }
    else if (_input_type == input_type::RIGHT)
    {
        is_down_keyboard = keyboard::is_down('D');
        is_down_joystick = joystick::is_down(joystick_button_type::RIGHT);
    }
    else if (_input_type == input_type::Y)
    {
        is_down_keyboard = keyboard::is_down('I');
        is_down_joystick = joystick::is_down(joystick_button_type::Y);
    }
    else if (_input_type == input_type::X)
    {
        is_down_keyboard = keyboard::is_down('J');
        is_down_joystick = joystick::is_down(joystick_button_type::X);
    }
    else if (_input_type == input_type::A)
    {
        is_down_keyboard = keyboard::is_down('K');
        is_down_joystick = joystick::is_down(joystick_button_type::A);
    }
    else if (_input_type == input_type::B)
    {
        is_down_keyboard = keyboard::is_down('L');
        is_down_joystick = joystick::is_down(joystick_button_type::B);
    }

    return is_down_keyboard || is_down_joystick;
}

bool input::is_hold(const input_type _input_type)
{
    bool is_hold_keyboard { false };
    bool is_hold_joystick { false };

    if (_input_type == input_type::UP)
    {
        is_hold_keyboard = keyboard::is_hold('W');
        is_hold_joystick = joystick::is_hold(joystick_button_type::UP);
    }
    else if (_input_type == input_type::LEFT)
    {
        is_hold_keyboard = keyboard::is_hold('A');
        is_hold_joystick = joystick::is_hold(joystick_button_type::LEFT);
    }
    else if (_input_type == input_type::DOWN)
    {
        is_hold_keyboard = keyboard::is_hold('S');
        is_hold_joystick = joystick::is_hold(joystick_button_type::DOWN);
    }
    else if (_input_type == input_type::RIGHT)
    {
        is_hold_keyboard = keyboard::is_hold('D');
        is_hold_joystick = joystick::is_hold(joystick_button_type::RIGHT);
    }
    else if (_input_type == input_type::Y)
    {
        is_hold_keyboard = keyboard::is_hold('I');
        is_hold_joystick = joystick::is_hold(joystick_button_type::Y);
    }
    else if (_input_type == input_type::X)
    {
        is_hold_keyboard = keyboard::is_hold('J');
        is_hold_joystick = joystick::is_hold(joystick_button_type::X);
    }
    else if (_input_type == input_type::A)
    {
        is_hold_keyboard = keyboard::is_hold('K');
        is_hold_joystick = joystick::is_hold(joystick_button_type::A);
    }
    else if (_input_type == input_type::B)
    {
        is_hold_keyboard = keyboard::is_hold('L');
        is_hold_joystick = joystick::is_hold(joystick_button_type::B);
    }
    return is_hold_keyboard || is_hold_joystick;
}

bool input::is_up(const input_type _input_type)
{
    bool is_up_keyboard { false };
    bool is_up_joystick { false };

    if (_input_type == input_type::UP)
    {
        is_up_keyboard = keyboard::is_up('W');
        is_up_joystick = joystick::is_up(joystick_button_type::UP);
    }
    else if (_input_type == input_type::LEFT)
    {
        is_up_keyboard = keyboard::is_up('A');
        is_up_joystick = joystick::is_up(joystick_button_type::LEFT);
    }
    else if (_input_type == input_type::DOWN)
    {
        is_up_keyboard = keyboard::is_up('S');
        is_up_joystick = joystick::is_up(joystick_button_type::DOWN);
    }
    else if (_input_type == input_type::RIGHT)
    {
        is_up_keyboard = keyboard::is_up('D');
        is_up_joystick = joystick::is_up(joystick_button_type::RIGHT);
    }
    else if (_input_type == input_type::Y)
    {
        is_up_keyboard = keyboard::is_up('I');
        is_up_joystick = joystick::is_up(joystick_button_type::Y);
    }
    else if (_input_type == input_type::X)
    {
        is_up_keyboard = keyboard::is_up('J');
        is_up_joystick = joystick::is_up(joystick_button_type::X);
    }
    else if (_input_type == input_type::A)
    {
        is_up_keyboard = keyboard::is_up('K');
        is_up_joystick = joystick::is_up(joystick_button_type::A);
    }
    else if (_input_type == input_type::B)
    {
        is_up_keyboard = keyboard::is_up('L');
        is_up_joystick = joystick::is_up(joystick_button_type::B);
    }
    return is_up_keyboard || is_up_joystick;
}

bool input::check_simultaneous(const input_type _input_type)
{
    bool is_simul_keyboard { false };
    bool is_simul_joystick { false };

    if (_input_type == input_type::UP)
    {
        is_simul_keyboard = keyboard::check_simultaneous('W');
        is_simul_joystick = joystick::check_simultaneous(joystick_button_type::UP);
    }
    else if (_input_type == input_type::LEFT)
    {
        is_simul_keyboard = keyboard::check_simultaneous('A');
        is_simul_joystick = joystick::check_simultaneous(joystick_button_type::LEFT);
    }
    else if (_input_type == input_type::DOWN)
    {
        is_simul_keyboard = keyboard::check_simultaneous('S');
        is_simul_joystick = joystick::check_simultaneous(joystick_button_type::DOWN);
    }
    else if (_input_type == input_type::RIGHT)
    {
        is_simul_keyboard = keyboard::check_simultaneous('D');
        is_simul_joystick = joystick::check_simultaneous(joystick_button_type::RIGHT);
    }
    else if (_input_type == input_type::Y)
    {
        is_simul_keyboard = keyboard::check_simultaneous('I');
        is_simul_joystick = joystick::check_simultaneous(joystick_button_type::Y);
    }
    else if (_input_type == input_type::X)
    {
        is_simul_keyboard = keyboard::check_simultaneous('J');
        is_simul_joystick = joystick::check_simultaneous(joystick_button_type::X);
    }
    else if (_input_type == input_type::A)
    {
        is_simul_keyboard = keyboard::check_simultaneous('K');
        is_simul_joystick = joystick::check_simultaneous(joystick_button_type::A);
    }
    else if (_input_type == input_type::B)
    {
        is_simul_keyboard = keyboard::check_simultaneous('L');
        is_simul_joystick = joystick::check_simultaneous(joystick_button_type::B);
    }
    return is_simul_keyboard || is_simul_joystick;
}

}


