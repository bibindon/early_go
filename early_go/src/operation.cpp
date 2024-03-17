#include "stdafx.hpp"
#include "key.hpp"
#include "character.hpp"
#include "operation.hpp"
#include "camera.hpp"
#include <boost/type_erasure/typeid_of.hpp>

namespace early_go
{

    template <
        class CurrentKeyAction,
        direction CurrentKeyDirection,
        class RecentKeyAction,
        direction RecentKeyDirection,
        std::enable_if_t<
            !std::is_same<CurrentKeyAction, character::step_and_rotate>::value &&
                !std::is_same<CurrentKeyAction, character::attack>::value,
            std::nullptr_t> = nullptr>
    void set_state(character &,
                   std::shared_ptr<operation::behavior_concept> &,
                   std::shared_ptr<operation::behavior_concept> &) {}
    template <
        class CurrentKeyAction,
        direction CurrentKeyDirection,
        class RecentKeyAction,
        direction RecentKeyDirection,
        std::enable_if_t<
            std::is_same<CurrentKeyAction, character::step_and_rotate>::value &&
                !std::is_same<CurrentKeyAction, character::attack>::value,
            std::nullptr_t> = nullptr>
    void set_state(character &,
                   std::shared_ptr<operation::behavior_concept> &,
                   std::shared_ptr<operation::behavior_concept> &);
    template <
        class CurrentKeyAction,
        direction CurrentKeyDirection,
        class RecentKeyAction,
        direction RecentKeyDirection,
        std::enable_if_t<
            !std::is_same<CurrentKeyAction, character::step_and_rotate>::value &&
                std::is_same<CurrentKeyAction, character::attack>::value,
            std::nullptr_t> = nullptr>
    void set_state(character &,
                   std::shared_ptr<operation::behavior_concept> &,
                   std::shared_ptr<operation::behavior_concept> &);

    const std::chrono::milliseconds operation::DOUBLE_DOWN_CHANCE_FRAME = std::chrono::milliseconds(50);

    operation::operation(const std::shared_ptr<camera> &camera)
        : camera_{camera},
          current_behavior_{nullptr},
          reserved_behavior_{nullptr},
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
        if (offensive_area_.find(current_stage_z) == offensive_area_.end())
        {
            return;
        }
        if (offensive_area_.at(current_stage_z).find(x) ==
            offensive_area_.at(current_stage_z).end())
        {
            return;
        }
        offensive_area_.at(current_stage_z).at(x) = true;
    }

    std::shared_ptr<camera> operation::get_camera() const
    {
        return camera_;
    }

    void operation::operator()(main_window &a_main_window)
    {
        character &main_character{*a_main_window.get_main_character()};

        if (key::is_down('Q'))
        {
            PostQuitMessage(0);
        }
        if (key::is_down('W'))
        {
            if (current_behavior_ == nullptr)
            {
                current_behavior_.reset(new_crt behavior_concept{
                    character::step{main_character, direction::FRONT}});
            }
            else
            {
                if (check_recent_keycode('A'))
                {
                    set_state<character::attack, direction::NONE,
                              character::step, direction::LEFT>(
                        main_character, current_behavior_, reserved_behavior_);
                }
                else if (check_recent_keycode('I'))
                {
                    set_state<character::step_and_rotate, direction::FRONT,
                              character::rotate, direction::FRONT>(
                        main_character, current_behavior_, reserved_behavior_);
                }
                else if (check_recent_keycode('J'))
                {
                    set_state<character::step_and_rotate, direction::FRONT,
                              character::rotate, direction::LEFT>(
                        main_character, current_behavior_, reserved_behavior_);
                }
                else if (check_recent_keycode('K'))
                {
                    set_state<character::step_and_rotate, direction::FRONT,
                              character::rotate, direction::BACK>(
                        main_character, current_behavior_, reserved_behavior_);
                }
                else if (check_recent_keycode('L'))
                {
                    set_state<character::step_and_rotate, direction::FRONT,
                              character::rotate, direction::RIGHT>(
                        main_character, current_behavior_, reserved_behavior_);
                }
                else
                {
                    reserved_behavior_.reset(new_crt behavior_concept{
                        character::step{main_character, direction::FRONT}});
                }
            }
        }
        if (key::is_down('A'))
        {
            if (current_behavior_ == nullptr)
            {
                current_behavior_.reset(new_crt behavior_concept{
                    character::step{main_character, direction::LEFT}});
            }
            else
            {
                if (check_recent_keycode('W'))
                {
                    set_state<character::attack, direction::NONE,
                              character::step, direction::FRONT>(
                        main_character, current_behavior_, reserved_behavior_);
                }
                else if (check_recent_keycode('I'))
                {
                    set_state<character::step_and_rotate, direction::LEFT,
                              character::rotate, direction::FRONT>(
                        main_character, current_behavior_, reserved_behavior_);
                }
                else if (check_recent_keycode('J'))
                {
                    set_state<character::step_and_rotate, direction::LEFT,
                              character::rotate, direction::LEFT>(
                        main_character, current_behavior_, reserved_behavior_);
                }
                else if (check_recent_keycode('K'))
                {
                    set_state<character::step_and_rotate, direction::LEFT,
                              character::rotate, direction::BACK>(
                        main_character, current_behavior_, reserved_behavior_);
                }
                else if (check_recent_keycode('L'))
                {
                    set_state<character::step_and_rotate, direction::LEFT,
                              character::rotate, direction::RIGHT>(
                        main_character, current_behavior_, reserved_behavior_);
                }
                else
                {
                    reserved_behavior_.reset(new_crt behavior_concept{
                        character::step{main_character, direction::LEFT}});
                }
            }
        }
        if (key::is_down('S'))
        {
            if (current_behavior_ == nullptr)
            {
                current_behavior_.reset(new_crt behavior_concept{
                    character::step{main_character, direction::BACK}});
            }
            else
            {
                if (check_recent_keycode('I'))
                {
                    set_state<character::step_and_rotate, direction::BACK,
                              character::rotate, direction::FRONT>(main_character,
                                                                   current_behavior_,
                                                                   reserved_behavior_);
                }
                else if (check_recent_keycode('J'))
                {
                    set_state<character::step_and_rotate, direction::BACK,
                              character::rotate, direction::LEFT>(main_character,
                                                                  current_behavior_,
                                                                  reserved_behavior_);
                }
                else if (check_recent_keycode('K'))
                {
                    set_state<character::step_and_rotate, direction::BACK,
                              character::rotate, direction::BACK>(main_character,
                                                                  current_behavior_,
                                                                  reserved_behavior_);
                }
                else if (check_recent_keycode('L'))
                {
                    set_state<character::step_and_rotate, direction::BACK,
                              character::rotate, direction::RIGHT>(main_character,
                                                                   current_behavior_,
                                                                   reserved_behavior_);
                }
                else
                {
                    reserved_behavior_.reset(new_crt behavior_concept{
                        character::step{main_character, direction::BACK}});
                }
            }
        }
        if (key::is_down('D'))
        {
            if (current_behavior_ == nullptr)
            {
                current_behavior_.reset(new_crt behavior_concept{
                    character::step{main_character, direction::RIGHT}});
            }
            else
            {
                if (check_recent_keycode('I'))
                {
                    set_state<character::step_and_rotate, direction::RIGHT,
                              character::rotate, direction::FRONT>(main_character,
                                                                   current_behavior_,
                                                                   reserved_behavior_);
                }
                else if (check_recent_keycode('J'))
                {
                    set_state<character::step_and_rotate, direction::RIGHT,
                              character::rotate, direction::LEFT>(main_character,
                                                                  current_behavior_,
                                                                  reserved_behavior_);
                }
                else if (check_recent_keycode('K'))
                {
                    set_state<character::step_and_rotate, direction::RIGHT,
                              character::rotate, direction::BACK>(main_character,
                                                                  current_behavior_,
                                                                  reserved_behavior_);
                }
                else if (check_recent_keycode('L'))
                {
                    set_state<character::step_and_rotate, direction::RIGHT,
                              character::rotate, direction::RIGHT>(main_character,
                                                                   current_behavior_,
                                                                   reserved_behavior_);
                }
                else
                {
                    reserved_behavior_.reset(new_crt behavior_concept{
                        character::step{main_character, direction::RIGHT}});
                }
            }
        }
        if (key::is_down('I'))
        {
            if (current_behavior_ == nullptr)
            {
                current_behavior_.reset(new_crt behavior_concept{
                    character::rotate{main_character, direction::FRONT}});
            }
            else
            {
                if (check_recent_keycode('W'))
                {
                    set_state<character::step_and_rotate, direction::FRONT,
                              character::step, direction::FRONT>(main_character,
                                                                 current_behavior_,
                                                                 reserved_behavior_);
                }
                else if (check_recent_keycode('A'))
                {
                    set_state<character::step_and_rotate, direction::FRONT,
                              character::step, direction::LEFT>(main_character,
                                                                current_behavior_,
                                                                reserved_behavior_);
                }
                else if (check_recent_keycode('S'))
                {
                    set_state<character::step_and_rotate, direction::FRONT,
                              character::step, direction::BACK>(main_character,
                                                                current_behavior_,
                                                                reserved_behavior_);
                }
                else if (check_recent_keycode('D'))
                {
                    set_state<character::step_and_rotate, direction::FRONT,
                              character::step, direction::RIGHT>(main_character,
                                                                 current_behavior_,
                                                                 reserved_behavior_);
                }
                else
                {
                    reserved_behavior_.reset(new_crt behavior_concept{
                        character::rotate{main_character, direction::FRONT}});
                }
            }
        }
        if (key::is_down('J'))
        {
            if (current_behavior_ == nullptr)
            {
                current_behavior_.reset(new_crt behavior_concept{
                    character::rotate{main_character, direction::LEFT}});
            }
            else
            {
                if (check_recent_keycode('W'))
                {
                    set_state<character::step_and_rotate, direction::LEFT,
                              character::step, direction::FRONT>(main_character,
                                                                 current_behavior_,
                                                                 reserved_behavior_);
                }
                else if (check_recent_keycode('A'))
                {
                    set_state<character::step_and_rotate, direction::LEFT,
                              character::step, direction::LEFT>(main_character,
                                                                current_behavior_,
                                                                reserved_behavior_);
                }
                else if (check_recent_keycode('S'))
                {
                    set_state<character::step_and_rotate, direction::LEFT,
                              character::step, direction::BACK>(main_character,
                                                                current_behavior_,
                                                                reserved_behavior_);
                }
                else if (check_recent_keycode('D'))
                {
                    set_state<character::step_and_rotate, direction::LEFT,
                              character::step, direction::RIGHT>(main_character,
                                                                 current_behavior_,
                                                                 reserved_behavior_);
                }
                else
                {
                    reserved_behavior_.reset(new_crt behavior_concept{
                        character::rotate{main_character, direction::LEFT}});
                }
            }
        }
        if (key::is_down('K'))
        {
            if (current_behavior_ == nullptr)
            {
                current_behavior_.reset(new_crt behavior_concept{
                    character::rotate{main_character, direction::BACK}});
            }
            else
            {
                if (check_recent_keycode('W'))
                {
                    set_state<character::step_and_rotate, direction::BACK,
                              character::step, direction::FRONT>(main_character,
                                                                 current_behavior_,
                                                                 reserved_behavior_);
                }
                else if (check_recent_keycode('A'))
                {
                    set_state<character::step_and_rotate, direction::BACK,
                              character::step, direction::LEFT>(main_character,
                                                                current_behavior_,
                                                                reserved_behavior_);
                }
                else if (check_recent_keycode('S'))
                {
                    set_state<character::step_and_rotate, direction::BACK,
                              character::step, direction::BACK>(main_character,
                                                                current_behavior_,
                                                                reserved_behavior_);
                }
                else if (check_recent_keycode('D'))
                {
                    set_state<character::step_and_rotate, direction::BACK,
                              character::step, direction::RIGHT>(main_character,
                                                                 current_behavior_,
                                                                 reserved_behavior_);
                }
                else
                {
                    reserved_behavior_.reset(new_crt behavior_concept{
                        character::rotate{main_character, direction::BACK}});
                }
            }
        }
        if (key::is_down('L'))
        {
            if (current_behavior_ == nullptr)
            {
                current_behavior_.reset(new_crt behavior_concept{
                    character::rotate{main_character, direction::RIGHT}});
            }
            else
            {
                if (check_recent_keycode('W'))
                {
                    set_state<character::step_and_rotate, direction::RIGHT,
                              character::step, direction::FRONT>(main_character,
                                                                 current_behavior_,
                                                                 reserved_behavior_);
                }
                else if (check_recent_keycode('A'))
                {
                    set_state<character::step_and_rotate, direction::RIGHT,
                              character::step, direction::LEFT>(main_character,
                                                                current_behavior_,
                                                                reserved_behavior_);
                }
                else if (check_recent_keycode('S'))
                {
                    set_state<character::step_and_rotate, direction::RIGHT,
                              character::step, direction::BACK>(main_character,
                                                                current_behavior_,
                                                                reserved_behavior_);
                }
                else if (check_recent_keycode('D'))
                {
                    set_state<character::step_and_rotate, direction::RIGHT,
                              character::step, direction::RIGHT>(main_character,
                                                                 current_behavior_,
                                                                 reserved_behavior_);
                }
                else
                {
                    reserved_behavior_.reset(new_crt behavior_concept{
                        character::rotate{main_character, direction::RIGHT}});
                }
            }
        }

        for (auto &&z : offensive_area_)
        {
            for (auto &&x : z.second)
            {
                x.second = false;
            }
        }

        if (current_behavior_ != nullptr)
        {
            switch ((*current_behavior_)())
            {
            case behavior_state::PLAY:
                break;
            case behavior_state::CANCELABLE:
                if (reserved_behavior_ != nullptr)
                {
                    current_behavior_ = reserved_behavior_;
                    reserved_behavior_.reset();
                    (*current_behavior_)();
                }
                break;
            case behavior_state::FINISH:
                if (reserved_behavior_ != nullptr)
                {
                    current_behavior_ = reserved_behavior_;
                    reserved_behavior_.reset();
                    (*current_behavior_)();
                }
                else
                {
                    current_behavior_.reset();
                }
                break;
            }
        }

        // collision detection
        std::shared_ptr<character> enemy = a_main_window.get_enemy_character();

        cv::Point3i enemy_pos = enemy->get_grid_position();
        const int relative_z = enemy_pos.z % (constants::GRID_NUM_HEIGHT + 1);
        if (offensive_area_.at(relative_z).at(enemy_pos.x))
        {
            current_behavior_->cancel();
            int enemy_health = enemy->get_health();
            if (1 <= enemy_health - 1)
            {
                enemy->set_animation("Damaged");
                enemy->set_health(enemy_health - 1);
            }
            else if (current_stage_ != constants::MAX_STAGE_NUMBER - 1)
            {
                current_behavior_.reset(new_crt behavior_concept{
                    move_next_stage(*this, a_main_window)});
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

    template <
        class CurrentKeyAction,
        direction CurrentKeyDirection,
        class RecentKeyAction,
        direction RecentKeyDirection,
        std::enable_if_t<
            std::is_same<CurrentKeyAction, character::step_and_rotate>::value &&
                !std::is_same<CurrentKeyAction, character::attack>::value,
            std::nullptr_t>>
    void set_state(character &a_character,
                   std::shared_ptr<operation::behavior_concept> &current_behavior,
                   std::shared_ptr<operation::behavior_concept> &reserved_behavior)
    {
        constexpr direction step_direction{
            std::is_same<RecentKeyAction, character::rotate>::value
                ? CurrentKeyDirection
                : RecentKeyDirection};
        constexpr direction rotate_direction{
            std::is_same<RecentKeyAction, character::rotate>::value
                ? RecentKeyDirection
                : CurrentKeyDirection};

        if (reserved_behavior != nullptr &&
            boost::type_erasure::typeid_of(*reserved_behavior) ==
                typeid(RecentKeyAction) &&
            boost::get<direction>(reserved_behavior->get_params().at(0)) ==
                RecentKeyDirection)
        {
            reserved_behavior.reset(new_crt operation::behavior_concept{
                character::step_and_rotate{
                    a_character, step_direction, rotate_direction}});
        }
        else if (boost::type_erasure::typeid_of(*current_behavior) ==
                 typeid(character::step_and_rotate))
        {
            std::conditional_t<
                std::is_same_v<RecentKeyAction, character::step>,
                character::rotate,
                character::step>
                step_or_rotate{a_character, CurrentKeyDirection};
            reserved_behavior.reset(
                new_crt operation::behavior_concept{step_or_rotate});
        }
        else
        {
            current_behavior->cancel();
            current_behavior.reset(new_crt operation::behavior_concept{
                character::step_and_rotate{
                    a_character, step_direction, rotate_direction}});
        }
    }
    template <
        class CurrentKeyAction,
        direction CurrentKeyDirection,
        class RecentKeyAction,
        direction RecentKeyDirection,
        std::enable_if_t<
            !std::is_same<CurrentKeyAction, character::step_and_rotate>::value &&
                std::is_same<CurrentKeyAction, character::attack>::value,
            std::nullptr_t>>
    void set_state(character &a_character,
                   std::shared_ptr<operation::behavior_concept> &current_behavior,
                   std::shared_ptr<operation::behavior_concept> &reserved_behavior)
    {
        if (current_behavior == nullptr && reserved_behavior == nullptr)
        {
            current_behavior.reset(new_crt operation::behavior_concept{
                character::attack{a_character}});
        }
        else if (current_behavior != nullptr && reserved_behavior == nullptr)
        {
            if (boost::type_erasure::typeid_of(*current_behavior) ==
                    typeid(RecentKeyAction) &&
                boost::get<direction>(current_behavior->get_params().at(0)) ==
                    RecentKeyDirection)
            {
                current_behavior->cancel();
                current_behavior.reset(new_crt operation::behavior_concept{
                    character::attack{a_character}});
            }
            else
            {
                reserved_behavior.reset(new_crt operation::behavior_concept{
                    character::attack{a_character}});
            }
        }
        else
        {
            reserved_behavior.reset(new_crt operation::behavior_concept{
                character::attack{a_character}});
        }
    }

    bool operation::check_recent_keycode(const int &keycode)
    {
        auto time_out_it = std::find_if(
            std::begin(key::key_deque_), std::end(key::key_deque_),
            [&](const std::pair<
                std::chrono::system_clock::time_point,
                std::array<SHORT, key::KEY_TABLE_LENGTH>> &x)
            {
                std::chrono::system_clock::time_point time_out{
                    key::key_deque_.at(0).first - DOUBLE_DOWN_CHANCE_FRAME};
                return time_out > x.first;
            });
        auto keycode_it = std::find_if(
            std::begin(key::key_deque_), time_out_it,
            [&](const std::pair<
                std::chrono::system_clock::time_point,
                std::array<SHORT, key::KEY_TABLE_LENGTH>> &x)
            { return key::is_down(x.first, keycode); });
        if (keycode_it != time_out_it)
        {
            return true;
        }
        return false;
    }

} /* namespace early_go */
