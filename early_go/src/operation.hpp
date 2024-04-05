#ifndef OPERATION_HPP
#define OPERATION_HPP

#include "stdafx.hpp"
#include "keyboard.hpp"
#include "main_window.hpp"

namespace early_go
{
class camera;
class stage;

class operation
{
public:
    operation(const std::shared_ptr<camera> &);
    void update(
        const std::shared_ptr<character>&,
        const std::shared_ptr<character>&,
        const std::shared_ptr<stage>& _stage);

    enum class behavior_state
    {
        NO_STATE,
        READY,
        PLAY_CANCELABLE,
        PLAY,
        STOP,
        ALLOW_NEXT_ACTION,
        CANCEL,
        FINISH,
    };

    enum class action_type
    {
        NO_ACTION,
        STEP,
        ROTATE,
        STEP_AND_ROTATE,
        ATTACK,
        MOVE_NEXT_STAGE,
        CAMERA_CLOSE_UP,
    };

    struct behavior
    {
        action_type action_type_;
        direction step_dir_;
        direction rotate_dir_;
        behavior_state state_;
    };

    std::shared_ptr<camera> get_camera() const;

private:
    std::shared_ptr<camera> camera_;
    std::shared_ptr<behavior> current_behavior_ { new_crt behavior { } };
    std::shared_ptr<behavior> reserved_behavior_ { new_crt behavior { } };

    // std::unordered_map is used because using minus index is necessary.
//    std::unordered_map<int, std::unordered_map<int, bool>> offensive_area_ { };

    int current_stage_ { 0 };

    // Prevent continues frame damage.
    bool ignore_attack_judge_ { false };

    void cancel_removed_by_simul_push();
};
} // namespace early_go 
#endif
