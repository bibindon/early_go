#include "stdafx.hpp"
#include "keyboard.hpp"
#include "character.hpp"
#include "operation.hpp"
#include "camera.hpp"
#include "se_player.hpp"
#include "input.hpp"
#include "stage.hpp"

using std::shared_ptr;

namespace early_go
{
operation::operation(const shared_ptr<camera> &camera)
    : camera_{ camera }
{
}

shared_ptr<camera> operation::get_camera() const
{
    return camera_;
}

void operation::update(
    const shared_ptr<character>& _main_chara,
    const shared_ptr<character>& enemy,
    const shared_ptr<stage>& _stage)
{
    ++frame_counter_;
    int previous_push_elapse { frame_counter_ - current_behavior_->pushed_frame_ };
    _main_chara->update(_stage);
    if (keyboard::is_down('Q'))
    {
        PostQuitMessage(0);
    }
    else if (input::is_down(input_type::UP))
    {
        reserved_behavior_->step_dir_ = direction::FRONT;
        reserved_behavior_->pushed_frame_ = frame_counter_;
        if (input::check_simultaneous(input_type::LEFT))
        {
            reserved_behavior_->action_type_ = action_type::ATTACK;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::Y))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::FRONT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::X))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::LEFT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::A))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::BACK;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::B))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::RIGHT;
            cancel_removed_by_simul_push();
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::STEP;
        }
    }
    else if (input::is_down(input_type::LEFT))
    {
        reserved_behavior_->step_dir_ = direction::LEFT;
        reserved_behavior_->pushed_frame_ = frame_counter_;
        if (input::check_simultaneous(input_type::UP))
        {
            reserved_behavior_->action_type_ = action_type::ATTACK;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::Y))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::FRONT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::X))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::LEFT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::A))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::BACK;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::B))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::RIGHT;
            cancel_removed_by_simul_push();
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::STEP;
        }
    }
    else if (input::is_down(input_type::DOWN))
    {
        reserved_behavior_->step_dir_ = direction::BACK;
        reserved_behavior_->pushed_frame_ = frame_counter_;
        if (input::check_simultaneous(input_type::Y))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::FRONT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::X))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::LEFT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::A))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::BACK;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::B))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::RIGHT;
            cancel_removed_by_simul_push();
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::STEP;
        }
    }
    else if (input::is_down(input_type::RIGHT))
    {
        reserved_behavior_->step_dir_ = direction::RIGHT;
        reserved_behavior_->pushed_frame_ = frame_counter_;
        if (input::check_simultaneous(input_type::Y))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::FRONT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::X))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::LEFT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::A))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::BACK;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::B))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::RIGHT;
            cancel_removed_by_simul_push();
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::STEP;
        }
    }
    else if (input::is_down(input_type::Y))
    {
        reserved_behavior_->rotate_dir_ = direction::FRONT;
        reserved_behavior_->pushed_frame_ = frame_counter_;
        if (input::check_simultaneous(input_type::UP))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::FRONT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::LEFT))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::LEFT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::DOWN))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::BACK;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::RIGHT))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::RIGHT;
            cancel_removed_by_simul_push();
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::ROTATE;
        }
    }
    else if (input::is_down(input_type::X))
    {
        reserved_behavior_->rotate_dir_ = direction::LEFT;
        reserved_behavior_->pushed_frame_ = frame_counter_;
        if (input::check_simultaneous(input_type::UP))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::FRONT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::LEFT))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::LEFT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::DOWN))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::BACK;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::RIGHT))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::RIGHT;
            cancel_removed_by_simul_push();
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::ROTATE;
        }
    }
    else if (input::is_down(input_type::A))
    {
        reserved_behavior_->rotate_dir_ = direction::BACK;
        reserved_behavior_->pushed_frame_ = frame_counter_;
        if (input::check_simultaneous(input_type::UP))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::FRONT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::LEFT))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::LEFT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::DOWN))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::BACK;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::RIGHT))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::RIGHT;
            cancel_removed_by_simul_push();
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::ROTATE;
        }
    }
    else if (input::is_down(input_type::B))
    {
        reserved_behavior_->rotate_dir_ = direction::RIGHT;
        reserved_behavior_->pushed_frame_ = frame_counter_;
        if (input::check_simultaneous(input_type::UP))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::FRONT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::LEFT))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::LEFT;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::DOWN))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::BACK;
            cancel_removed_by_simul_push();
        }
        else if (input::check_simultaneous(input_type::RIGHT))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::RIGHT;
            cancel_removed_by_simul_push();
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::ROTATE;
        }
    }
    else if (input::is_hold(input_type::UP))
    {
        if (reserved_behavior_->action_type_ == action_type::NO_ACTION &&
            previous_push_elapse >= constants::ACTION_INTERVAL_FRAME)
        {
            reserved_behavior_->step_dir_ = direction::FRONT;
            reserved_behavior_->pushed_frame_ = frame_counter_;
            if (input::check_simultaneous(input_type::LEFT))
            {
                reserved_behavior_->action_type_ = action_type::ATTACK;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::Y))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::FRONT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::X))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::LEFT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::A))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::BACK;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::B))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::RIGHT;
                cancel_removed_by_simul_push();
            }
            else
            {
                reserved_behavior_->action_type_ = action_type::STEP;
            }
        }
    }
    else if (input::is_hold(input_type::LEFT))
    {
        if (reserved_behavior_->action_type_ == action_type::NO_ACTION &&
            previous_push_elapse >= constants::ACTION_INTERVAL_FRAME)
        {
            reserved_behavior_->step_dir_ = direction::LEFT;
            reserved_behavior_->pushed_frame_ = frame_counter_;
            if (input::check_simultaneous(input_type::UP))
            {
                reserved_behavior_->action_type_ = action_type::ATTACK;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::Y))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::FRONT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::X))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::LEFT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::A))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::BACK;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::B))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::RIGHT;
                cancel_removed_by_simul_push();
            }
            else
            {
                reserved_behavior_->action_type_ = action_type::STEP;
            }
        }
    }
    else if (input::is_hold(input_type::DOWN))
    {
        if (reserved_behavior_->action_type_ == action_type::NO_ACTION &&
            previous_push_elapse >= constants::ACTION_INTERVAL_FRAME)
        {
            reserved_behavior_->step_dir_ = direction::BACK;
            reserved_behavior_->pushed_frame_ = frame_counter_;
            if (input::check_simultaneous(input_type::Y))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::FRONT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::X))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::LEFT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::A))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::BACK;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::B))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::RIGHT;
                cancel_removed_by_simul_push();
            }
            else
            {
                reserved_behavior_->action_type_ = action_type::STEP;
            }
        }
    }
    else if (input::is_hold(input_type::RIGHT))
    {
        if (reserved_behavior_->action_type_ == action_type::NO_ACTION &&
            previous_push_elapse >= constants::ACTION_INTERVAL_FRAME)
        {
            reserved_behavior_->step_dir_ = direction::RIGHT;
            reserved_behavior_->pushed_frame_ = frame_counter_;
            if (input::check_simultaneous(input_type::Y))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::FRONT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::X))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::LEFT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::A))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::BACK;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::B))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->rotate_dir_ = direction::RIGHT;
                cancel_removed_by_simul_push();
            }
            else
            {
                reserved_behavior_->action_type_ = action_type::STEP;
            }
        }
    }
    else if (input::is_hold(input_type::Y))
    {
        if (reserved_behavior_->action_type_ == action_type::NO_ACTION &&
            previous_push_elapse >= constants::ACTION_INTERVAL_FRAME)
        {
            reserved_behavior_->rotate_dir_ = direction::FRONT;
            reserved_behavior_->pushed_frame_ = frame_counter_;
            if (input::check_simultaneous(input_type::UP))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::FRONT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::LEFT))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::LEFT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::DOWN))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::BACK;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::RIGHT))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::RIGHT;
                cancel_removed_by_simul_push();
            }
            else
            {
                reserved_behavior_->action_type_ = action_type::ROTATE;
            }
        }
    }
    else if (input::is_hold(input_type::X))
    {
        if (reserved_behavior_->action_type_ == action_type::NO_ACTION &&
            previous_push_elapse >= constants::ACTION_INTERVAL_FRAME)
        {
            reserved_behavior_->rotate_dir_ = direction::LEFT;
            reserved_behavior_->pushed_frame_ = frame_counter_;
            if (input::check_simultaneous(input_type::UP))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::FRONT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::LEFT))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::LEFT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::DOWN))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::BACK;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::RIGHT))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::RIGHT;
                cancel_removed_by_simul_push();
            }
            else
            {
                reserved_behavior_->action_type_ = action_type::ROTATE;
            }
        }
    }
    else if (input::is_hold(input_type::A))
    {
        if (reserved_behavior_->action_type_ == action_type::NO_ACTION &&
            previous_push_elapse >= constants::ACTION_INTERVAL_FRAME)
        {
            reserved_behavior_->rotate_dir_ = direction::BACK;
            reserved_behavior_->pushed_frame_ = frame_counter_;
            if (input::check_simultaneous(input_type::UP))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::FRONT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::LEFT))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::LEFT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::DOWN))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::BACK;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::RIGHT))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::RIGHT;
                cancel_removed_by_simul_push();
            }
            else
            {
                reserved_behavior_->action_type_ = action_type::ROTATE;
            }
        }
    }
    else if (input::is_hold(input_type::B))
    {
        if (reserved_behavior_->action_type_ == action_type::NO_ACTION &&
            current_behavior_->pushed_frame_ <= constants::ACTION_INTERVAL_FRAME)
        {
            reserved_behavior_->rotate_dir_ = direction::RIGHT;
            reserved_behavior_->pushed_frame_ = frame_counter_;
            if (input::check_simultaneous(input_type::UP))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::FRONT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::LEFT))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::LEFT;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::DOWN))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::BACK;
                cancel_removed_by_simul_push();
            }
            else if (input::check_simultaneous(input_type::RIGHT))
            {
                reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
                reserved_behavior_->step_dir_ = direction::RIGHT;
                cancel_removed_by_simul_push();
            }
            else
            {
                reserved_behavior_->action_type_ = action_type::ROTATE;
            }
        }
    }

    if (current_behavior_->state_ == behavior_state::CANCEL)
    {
        _main_chara->cancel_action();
        current_behavior_->state_ = behavior_state::FINISH;
    }

    if (current_behavior_->state_ == behavior_state::FINISH ||
        current_behavior_->state_ == behavior_state::ALLOW_NEXT_ACTION ||
        current_behavior_->state_ == behavior_state::NO_STATE)
    {
        if (reserved_behavior_->action_type_ != action_type::NO_ACTION)
        {
            *current_behavior_ = *reserved_behavior_;
            current_behavior_->state_ = behavior_state::READY;

            reserved_behavior_->action_type_ = action_type::NO_ACTION;
            reserved_behavior_->step_dir_ = direction::NONE;
            reserved_behavior_->rotate_dir_ = direction::NONE;
            reserved_behavior_->state_ = behavior_state::NO_STATE;
            ignore_attack_judge_ = false;
        }
    }

    static int _cancelable_counter = 0;
    if (current_behavior_->state_ == behavior_state::READY)
    {
        current_behavior_->state_ = behavior_state::PLAY_CANCELABLE;
        _cancelable_counter = 0;
        if (current_behavior_->action_type_ == action_type::STEP)
        {
            _main_chara->set_action_step(current_behavior_->step_dir_);
        }
        else if (current_behavior_->action_type_ == action_type::ROTATE)
        {
            _main_chara->set_action_rotate(current_behavior_->rotate_dir_);
        }
        else if (current_behavior_->action_type_ == action_type::STEP_AND_ROTATE)
        {
            _main_chara->set_action_step_and_rotate(
                current_behavior_->step_dir_, current_behavior_->rotate_dir_);
        }
        else if (current_behavior_->action_type_ == action_type::ATTACK)
        {
            _main_chara->set_action_attack();
            se_player::get_ton()->play("sound/attack01.wav");
        }
        else if (current_behavior_->action_type_ == action_type::MOVE_NEXT_STAGE)
        {
            _main_chara->set_action_move_next_stage();
            enemy->set_action_move_next_stage();
            camera_->set_move_next_stage_anim(D3DXVECTOR3 { 0.0f, 0.0f, 0.0f });
        }
    }
    else if (current_behavior_->state_ == behavior_state::PLAY_CANCELABLE)
    {
        if (reserved_behavior_->action_type_ != action_type::NO_ACTION)
        {
            reserved_behavior_->state_ = behavior_state::READY;
        }
        ++_cancelable_counter;
        if (_cancelable_counter > SIMULTANEOUS_ALLOW_FRAME)
        {
            _cancelable_counter = 0;
            current_behavior_->state_ = behavior_state::PLAY;
        }
    }
    else if (current_behavior_->state_ == behavior_state::PLAY)
    {
        if (_main_chara->get_action_state() == behavior_state::FINISH)
        {
            current_behavior_->state_ = behavior_state::FINISH;
            ignore_attack_judge_ = false;
        }
        else if (_main_chara->get_action_state() == behavior_state::ALLOW_NEXT_ACTION)
        {
            current_behavior_->state_ = behavior_state::ALLOW_NEXT_ACTION;
        }
    }
    else if (current_behavior_->state_ == behavior_state::ALLOW_NEXT_ACTION)
    {
        if (_main_chara->get_action_state() == behavior_state::FINISH)
        {
            current_behavior_->state_ = behavior_state::FINISH;
            ignore_attack_judge_ = false;
        }
    }

    // collision detection
    cv::Point3i enemy_pos = enemy->get_grid_position();
    const int relative_z { enemy_pos.z % (constants::GRID_NUM_HEIGHT + 1) };
    auto offensive_area = _stage->offensive_area();
    if (offensive_area.at(relative_z).at(enemy_pos.x) && !ignore_attack_judge_)
    {
        ignore_attack_judge_ = true;
        int enemy_health { enemy->get_health() };
        if (enemy_health >= 2)
        {
            enemy->set_animation("Damaged");
            enemy->set_health(enemy_health - 1);
            se_player::get_ton()->play("sound/damage01.wav");
        }
        if (enemy_health == 1)
        {
            reserved_behavior_->action_type_ = action_type::MOVE_NEXT_STAGE;
        }
        if (current_stage_ == constants::MAX_ROOM_NUMBER - 1)
        {
            cv::Point3i grid_pos{_main_chara->get_grid_position()};
            D3DXVECTOR3 pos {
                grid_pos.x * constants::GRID_LENGTH,
                0.0f,
                grid_pos.z * constants::GRID_LENGTH };

            camera_->set_to_close_up_animation(pos);
        }
    }
    _stage->clear();
}

void operation::cancel_removed_by_simul_push()
{
    // When simultaneous push is detected, there is necessary that a previous push is
    // canceled.
    // i.e. A->J   "Step left" should be canceld and "Step and rotate" should be set.
    if (current_behavior_->state_ == behavior_state::PLAY_CANCELABLE &&
        (current_behavior_->action_type_ == action_type::STEP ||
            current_behavior_->action_type_ == action_type::ROTATE))
    {
        current_behavior_->state_ = behavior_state::CANCEL;
    }
}

/*
operation::move_next_stage::move_next_stage(
    operation &a_operation, main_window &a_main_window)
    : outer_{a_operation},
      main_window_{a_main_window},
      enemy_move_called_{false},
      main_chara_move_called_{false}
{
    cv::Point3i pos = main_window_.get_main_character()->get_grid_position();
    main_chara_x_ = static_cast<float>(pos.x) * constants::GRID_LENGTH;
    main_chara_z_ = static_cast<float>(pos.z) * constants::GRID_LENGTH;

    pos = main_window_.get_enemy_character()->get_grid_position();
    enemy_x_ = static_cast<float>(pos.x) * constants::GRID_LENGTH;
    enemy_z_ = static_cast<float>(pos.z) * constants::GRID_LENGTH;

    float goal_x{0.0f};
    float goal_z{7.0f * constants::GRID_LENGTH +
                 constants::GRID_LENGTH * (outer_.current_stage_ + 1) * (constants::GRID_NUM_HEIGHT + 1)};

    delta_enemy_x_ = goal_x - enemy_x_;
    delta_enemy_z_ = goal_z - enemy_z_;

    goal_z = 1.0f * constants::GRID_LENGTH + constants::GRID_LENGTH * (outer_.current_stage_ + 1) * (constants::GRID_NUM_HEIGHT + 1);

    delta_main_chara_x_ = goal_x - main_chara_x_;
    delta_main_chara_z_ = goal_z - main_chara_z_;
}

operation::behavior_state operation::move_next_stage::update()
{
    if (count_ == 0)
    {
        ++outer_.current_stage_;
        main_window_.get_enemy_character()->set_health(3);
        main_window_.get_enemy_character()->set_animation("Damaged");
    }
    else if (1.0f <= count_ * constants::ANIMATION_SPEED &&
             count_ * constants::ANIMATION_SPEED < 3.0f)
    {
        if (!enemy_move_called_)
        {
            enemy_move_called_ = true;
            main_window_.get_enemy_character()->set_rotation(direction::BACK);
            main_window_.get_enemy_character()->set_animation("Step_Back");
        }
        float sine{get_sine_curve(count_ * constants::ANIMATION_SPEED - 1.0f, 2.0f)};
        main_window_.get_enemy_character()->set_position(
            D3DXVECTOR3{enemy_x_ + delta_enemy_x_ * sine,
                        0.0,
                        enemy_z_ + delta_enemy_z_ * sine});
    }
    else if (3.0f <= count_ * constants::ANIMATION_SPEED &&
             count_ * constants::ANIMATION_SPEED < 5.0f)
    {
        if (!main_chara_move_called_)
        {
            main_chara_move_called_ = true;
            main_window_.get_main_character()->set_rotation(direction::FRONT);
            main_window_.get_main_character()->set_animation("Step_Front");
            outer_.camera_->move_position(
                D3DXVECTOR3{delta_main_chara_x_, 0.0, delta_main_chara_z_}, 2.0f);
        }
        float sine{get_sine_curve(count_ * constants::ANIMATION_SPEED - 3.0f, 2.0f)};
        main_window_.get_main_character()->set_position(
            D3DXVECTOR3{main_chara_x_ + delta_main_chara_x_ * sine,
                        0.0,
                        main_chara_z_ + delta_main_chara_z_ * sine});
    }
    else if (5.0f <= count_ * constants::ANIMATION_SPEED)
    {
        cv::Point3i pos = main_window_.get_main_character()->get_grid_position();
        pos.x = 0;
        pos.z = 1 + outer_.current_stage_ * 10;
        main_window_.get_main_character()->set_position(pos);
        pos = main_window_.get_enemy_character()->get_grid_position();
        pos.x = 0;
        pos.z = 7 + outer_.current_stage_ * 10;
        main_window_.get_enemy_character()->set_position(pos);
        outer_.reserved_behavior_.reset();
        return operation::behavior_state::FINISH;
    }

    ++count_;
    return operation::behavior_state::PLAY;
}
*/

} // namespace early_go 
