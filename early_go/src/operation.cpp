#include "stdafx.hpp"
#include "key.hpp"
#include "character.hpp"
#include "operation.hpp"
#include "camera.hpp"

using std::shared_ptr;

namespace early_go
{

operation::operation(const shared_ptr<camera> &camera)
    : camera_{camera},
      current_behavior_{ new_crt behavior{} },
      reserved_behavior_{ new_crt behavior{} },
      current_stage_{0}
{
    for (int z = 0; z < constants::GRID_NUM_HEIGHT; ++z)
    {
        for (int x = -(constants::GRID_NUM_WIDTH - 1) / 2;
             x < (constants::GRID_NUM_WIDTH + 1) / 2; ++x)
        {
            offensive_area_[z][x] = false;
        }
    }
}

void operation::set_offensive_position(const int &x, const int &z)
{
    const int current_stage_z = z % (constants::GRID_NUM_HEIGHT + 1);
    // Check out of range(z).
    if (offensive_area_.find(current_stage_z) == offensive_area_.end())
    {
        return;
    }
    // Check out of range(x).
    if (offensive_area_.at(current_stage_z).find(x) == offensive_area_.at(current_stage_z).end())
    {
        return;
    }
    offensive_area_.at(current_stage_z).at(x) = true;
}

shared_ptr<camera> operation::get_camera() const
{
    return camera_;
}

void operation::operator()(main_window &a_main_window)
{
    shared_ptr<character> main_character2 = a_main_window.get_main_character();
    character &main_character{*(main_character2)};
//    character &main_character{*(a_main_window.get_main_character())};

    if (key::is_down('Q'))
    {
        PostQuitMessage(0);
    }
    else if (key::is_down('W'))
    {
        reserved_behavior_->step_dir_ = direction::FRONT;
        reserved_behavior_->state_ = behavior_state::READY;
        if (key::check_simultaneous('A'))
        {
            reserved_behavior_->action_type_ = action_type::ATTACK;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('I'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::FRONT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('J'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::LEFT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('K'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::BACK;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('L'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::RIGHT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::STEP;
        }
    }
    else if (key::is_down('A'))
    {
        reserved_behavior_->step_dir_ = direction::LEFT;
        reserved_behavior_->state_ = behavior_state::READY;
        if (key::check_simultaneous('W'))
        {
            reserved_behavior_->action_type_ = action_type::ATTACK;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('I'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::FRONT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('J'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::LEFT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('K'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::BACK;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('L'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::RIGHT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::STEP;
        }
    }
    else if (key::is_down('S'))
    {
        reserved_behavior_->step_dir_ = direction::BACK;
        reserved_behavior_->state_ = behavior_state::READY;
        if (key::check_simultaneous('I'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::FRONT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('J'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::LEFT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('K'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::BACK;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('L'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::RIGHT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::STEP;
        }
    }
    else if (key::is_down('D'))
    {
        reserved_behavior_->step_dir_ = direction::RIGHT;
        reserved_behavior_->state_ = behavior_state::READY;
        if (key::check_simultaneous('I'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::FRONT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('J'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::LEFT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('K'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::BACK;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('L'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->rotate_dir_ = direction::RIGHT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::STEP;
        }
    }
    else if (key::is_down('I'))
    {
        reserved_behavior_->rotate_dir_ = direction::FRONT;
        reserved_behavior_->state_ = behavior_state::READY;
        if (key::check_simultaneous('W'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::FRONT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('A'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::LEFT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('S'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::BACK;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('D'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::RIGHT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::ROTATE;
        }
    }
    else if (key::is_down('J'))
    {
        reserved_behavior_->rotate_dir_ = direction::LEFT;
        reserved_behavior_->state_ = behavior_state::READY;
        if (key::check_simultaneous('W'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::FRONT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('A'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::LEFT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('S'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::BACK;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('D'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::RIGHT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::ROTATE;
        }
    }
    else if (key::is_down('K'))
    {
        reserved_behavior_->rotate_dir_ = direction::BACK;
        reserved_behavior_->state_ = behavior_state::READY;
        if (key::check_simultaneous('W'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::FRONT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('A'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::LEFT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('S'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::BACK;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('D'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::RIGHT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::ROTATE;
        }
    }
    else if (key::is_down('L'))
    {
        reserved_behavior_->rotate_dir_ = direction::RIGHT;
        reserved_behavior_->state_ = behavior_state::READY;
        if (key::check_simultaneous('W'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::FRONT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('A'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::LEFT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('S'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::BACK;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else if (key::check_simultaneous('D'))
        {
            reserved_behavior_->action_type_ = action_type::STEP_AND_ROTATE;
            reserved_behavior_->step_dir_ = direction::RIGHT;
            current_behavior_->state_ = behavior_state::CANCEL;
        }
        else
        {
            reserved_behavior_->action_type_ = action_type::ROTATE;
        }
    }

    // Clear offensive_area_
    for (auto &&z : offensive_area_)
    {
        for (auto &&x : z.second)
        {
            x.second = false;
        }
    }

    if (current_behavior_->state_ == behavior_state::READY)
    {
        current_behavior_->state_ = behavior_state::PLAY;
        if (current_behavior_->action_type_ == action_type::STEP)
        {
            main_character2->set_action_step(current_behavior_->step_dir_);
        }
        else if (current_behavior_->action_type_ == action_type::ROTATE)
        {
            main_character2->set_action_rotate(current_behavior_->rotate_dir_);
        }
        else if (current_behavior_->action_type_ == action_type::STEP_AND_ROTATE)
        {
            main_character2->set_action_step_and_rotate(
                current_behavior_->step_dir_, current_behavior_->rotate_dir_);
        }
        else if (current_behavior_->action_type_ == action_type::ATTACK)
        {
            main_character2->set_action_attack();
        }
    }
    else if (current_behavior_->state_ == behavior_state::PLAY)
    {
        if (main_character2->get_action_state() == behavior_state::FINISH)
        {
            current_behavior_->state_ = behavior_state::FINISH;
        }
        else if (main_character2->get_action_state() == behavior_state::ALLOW_NEXT_ACTION)
        {
            current_behavior_->state_ = behavior_state::ALLOW_NEXT_ACTION;
        }
    }
    else if (current_behavior_->state_ == behavior_state::CANCEL)
    {
            main_character2->cancel_action();
            current_behavior_->state_ = behavior_state::FINISH;
    }

    if (current_behavior_->state_ == behavior_state::FINISH ||
        current_behavior_->state_ == behavior_state::ALLOW_NEXT_ACTION ||
        current_behavior_->state_ == behavior_state::NO_STATE)
    {
        if (reserved_behavior_->action_type_ != action_type::NO_ACTION)
        {
            *current_behavior_ = *reserved_behavior_;

            reserved_behavior_->action_type_ = action_type::NO_ACTION;
            reserved_behavior_->step_dir_ = direction::NONE;
            reserved_behavior_->rotate_dir_ = direction::NONE;
            reserved_behavior_->state_ = behavior_state::NO_STATE;
        }
    }

    // collision detection
    shared_ptr<character> enemy = a_main_window.get_enemy_character();

    cv::Point3i enemy_pos = enemy->get_grid_position();
    const int relative_z = enemy_pos.z % (constants::GRID_NUM_HEIGHT + 1);
    if (offensive_area_.at(relative_z).at(enemy_pos.x))
    {
//        current_behavior_->cancel();
        main_character2->cancel_action();
        int enemy_health = enemy->get_health();
        if (1 <= enemy_health - 1)
        {
            enemy->set_animation("Damaged");
            enemy->set_health(enemy_health - 1);
        }
        else if (current_stage_ != constants::MAX_STAGE_NUMBER - 1)
        {
        //    current_behavior_.reset(new_crt behavior_concept{
        //        move_next_stage(*this, a_main_window)});
        }
        else
        {
            cv::Point3i grid_pos{main_character.get_grid_position()};
            D3DXVECTOR3 pos{grid_pos.x * constants::GRID_LENGTH, 0.0f,
                            grid_pos.z * constants::GRID_LENGTH};
            camera_->set_to_close_up_animation(pos);
        }
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

operation::behavior_state operation::move_next_stage::operator()()
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
