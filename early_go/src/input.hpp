#pragma once

namespace early_go
{
// Facade of keyboard and joystick.
enum class input_type
{
    UP,
    LEFT,
    DOWN,
    RIGHT,
    Y,
    X,
    A,
    B,
    UNKNOWN,
};
class input
{
public:

    static bool update();
    static bool is_down(const input_type);
    static bool is_hold(const input_type);
    static bool is_up(const input_type);
    static bool check_simultaneous(const input_type keycode);

};

}
