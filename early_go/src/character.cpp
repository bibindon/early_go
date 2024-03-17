#include "stdafx.hpp"

#include <regex>
#include <cctype>
#include <typeinfo>

#include "abstract_mesh.hpp"
#include "character.hpp"
#include "camera.hpp"
#include "operation.hpp"

namespace early_go
{
    character::character(
        const std::shared_ptr<IDirect3DDevice9> &d3d_device,
        const std::shared_ptr<operation> &a_operation,
        const cv::Point3i &position,
        const direction &direction,
        const float &size)
        : d3d_device_{d3d_device},
          operation_{a_operation},
          position_{position.x * constants::GRID_LENGTH,
                    position.y * constants::GRID_LENGTH,
                    position.z * constants::GRID_LENGTH},
          grid_position_{position},
          direction_{direction},
          size_{size},
          health_{1},
          max_health_{100}
    {
        switch (direction_)
        {
        case direction::FRONT:
            rotation_.x = D3DX_PI;
            break;
        case direction::LEFT:
            rotation_.x = D3DX_PI / 2;
            break;
        case direction::BACK:
            rotation_.x = 0.0f;
            break;
        case direction::RIGHT:
            rotation_.x = D3DX_PI * 3 / 2;
            break;
        }
        rotation_.y = 0.0f;
        rotation_.z = 0.0f;
    }

    character::~character()
    {
    }

    void character::set_default_animation(const std::string &animation_name)
    {
        for (const auto &x : mesh_map_)
        {
            std::string animation_fullname{
                create_animation_fullname(animation_name, x.first)};

            x.second->set_default_animation(animation_fullname);
        }
    }

    void character::set_animation_config(
        const std::string &animation_name,
        const bool &loop,
        const float &duration)
    {
        duration_map_.emplace(animation_name, duration);
        for (const auto &x : mesh_map_)
        {
            std::string animation_fullname{
                create_animation_fullname(animation_name, x.first)};

            x.second->set_animation_config(animation_fullname, loop, duration);
        }
    }

    void character::render(
        const D3DXMATRIX &view_matrix,
        const D3DXMATRIX &projection_matrix,
        const D3DXVECTOR4 &normal_light,
        const float &brightness)
    {
        if (current_action_ != nullptr)
        {
            if (operation::behavior_state::FINISH == (*current_action_)())
            {
                current_action_.reset();
            }
        }

        for (const auto &x : mesh_map_)
        {
            x.second->render(view_matrix, projection_matrix, normal_light, brightness);
        }
    }

    void character::set_position(const cv::Point3i &position)
    {
        grid_position_ = position;
        set_position(D3DXVECTOR3{position.x * constants::GRID_LENGTH,
                                 position.y * constants::GRID_LENGTH,
                                 position.z * constants::GRID_LENGTH});
    }

    void character::set_rotation(const direction &direction)
    {
        D3DXVECTOR3 rotation;
        switch (direction)
        {
        case direction::FRONT:
            rotation.x = D3DX_PI;
            break;
        case direction::LEFT:
            rotation.x = D3DX_PI / 2;
            break;
        case direction::BACK:
            rotation.x = 0.0f;
            break;
        case direction::RIGHT:
            rotation.x = D3DX_PI * 3 / 2;
            break;
        }
        rotation.y = 0.0f;
        rotation.z = 0.0f;

        set_rotation(rotation);
    }

    void character::set_position(const D3DXVECTOR3 &position)
    {
        position_ = position;
        for (const auto &x : mesh_map_)
        {
            x.second->set_position(position_);
        }
    }

    void character::set_rotation(const D3DXVECTOR3 &rotation)
    {
        rotation_ = rotation;
        for (const auto &x : mesh_map_)
        {
            x.second->set_rotation(rotation_);
        }
    }

    void character::set_dynamic_texture(
        const std::string &x_filename,
        const std::string &texture_filename,
        const int &layer_number,
        const abstract_mesh::combine_type &combine_type)
    {
        if (mesh_map_.find(x_filename) != mesh_map_.end())
        {
            mesh_map_.at(x_filename)->set_dynamic_texture(texture_filename, layer_number, combine_type);
        }
    }

    void character::set_dynamic_texture_position(
        const std::string &x_filename,
        const int &layer_number,
        const D3DXVECTOR2 &position)
    {
        if (mesh_map_.find(x_filename) != mesh_map_.end())
        {
            mesh_map_.at(x_filename)->set_dynamic_texture_position(layer_number, position);
        }
    }

    void character::set_dynamic_texture_opacity(
        const std::string &x_filename,
        const int &layer_number,
        const float &opacity)
    {
        if (mesh_map_.find(x_filename) != mesh_map_.end())
        {
            mesh_map_.at(x_filename)->set_dynamic_texture_opacity(layer_number, opacity);
        }
    }

    void character::flip_dynamic_texture(const std::string &x_filename, const int &layer_number)
    {
        if (mesh_map_.find(x_filename) != mesh_map_.end())
        {
            mesh_map_.at(x_filename)->flip_dynamic_texture(layer_number);
        }
    }

    void character::clear_dynamic_texture(const std::string &x_filename, const int &layer_number)
    {
        if (mesh_map_.find(x_filename) != mesh_map_.end())
        {
            mesh_map_.at(x_filename)->clear_dynamic_texture(layer_number);
        }
    }

    void character::set_dynamic_message(
        const std::string &x_filename,
        const int &layer_number,
        const std::string &message,
        const bool &animation,
        const cv::Rect &rect,
        const DWORD &color,
        const std::string &fontname,
        const int &size,
        const int &weight,
        const BYTE &charset,
        const bool &proportional)
    {
        if (mesh_map_.find(x_filename) != mesh_map_.end())
        {
            mesh_map_.at(x_filename)->set_dynamic_message(
                layer_number,
                message,
                animation,
                rect,
                color,
                fontname,
                size,
                weight,
                charset,
                proportional);
        }
    }

    void character::set_dynamic_message_color(
        const std::string &x_filename, const int &layer_number, const D3DXVECTOR4 &color)
    {
        if (mesh_map_.find(x_filename) != mesh_map_.end())
        {
            mesh_map_.at(x_filename)->set_dynamic_message_color(layer_number, color);
        }
    }

    void character::set_animation(const std::string &animation_set)
    {
        for (const auto &x : mesh_map_)
        {
            std::string animation_fullname{
                create_animation_fullname(animation_set, x.first)};

            x.second->set_animation(animation_fullname);
        }
    }

    void character::set_shake_texture(const std::string &x_filename)
    {
        if (mesh_map_.find(x_filename) != mesh_map_.end())
        {
            mesh_map_.at(x_filename)->set_shake_texture();
        }
    }

    void character::set_fade_in(const std::string &x_filename)
    {
        if (mesh_map_.find(x_filename) != mesh_map_.end())
        {
            mesh_map_.at(x_filename)->set_fade_in();
        }
    }

    void character::set_fade_out(const std::string &x_filename)
    {
        if (mesh_map_.find(x_filename) != mesh_map_.end())
        {
            mesh_map_.at(x_filename)->set_fade_out();
        }
    }

    void character::set_step_action(const direction &step_dir)
    {
        current_action_.reset(); // call dtor.
        current_action_.reset(new_crt step{*this, step_dir});
    }

    void character::set_rotate_action(const direction &rotate_dir)
    {
        if (direction_ == rotate_dir)
        {
            return;
        }
        current_action_.reset(); // call dtor.
        current_action_.reset(new_crt rotate{*this, rotate_dir});
    }

    void character::set_step_and_rotate_action(
        const direction &step_dir,
        const direction &rotate_dir)
    {
        current_action_.reset(); // call dtor.
        current_action_.reset(new_crt step_and_rotate{*this, step_dir, rotate_dir});
    }

    void character::cancel_action()
    {
        if (current_action_ != nullptr)
        {
            current_action_->cancel();
            current_action_.reset();
        }
    }

    direction character::get_direction() const
    {
        return direction_;
    }

    cv::Point3i character::get_grid_position() const
    {
        return grid_position_;
    }

    D3DXVECTOR3 character::get_position() const
    {
        return position_;
    }

    int character::get_health() const
    {
        return health_;
    }

    void character::set_health(const int &health)
    {
        health_ = health;
    }

    int character::get_max_health() const
    {
        return max_health_;
    }

    void character::set_max_health(const int &health)
    {
        max_health_ = health;
    }

    // "Idle" + "hoge/piyo/hair.x" -> "Idle_Hair"
    std::string character::create_animation_fullname(
        const std::string &former_name, const std::string &model_fullname)
    {
        std::regex reg{R"(.*/(.*)\.x)"};
        std::smatch match{};
        std::string model_filename{};
        std::string animation_fullname{};

        if (!std::regex_match(model_fullname, match, reg) || match.size() != 2)
        {
            THROW_WITH_TRACE("A model filename doesn't match regular expression.: " +
                             model_fullname);
        }
        else
        {
            model_filename = match[1].str();
            unsigned char c = std::toupper(model_filename.at(0));
            model_filename.erase(0, 1);
            model_filename.insert(model_filename.begin(), c);
            animation_fullname = former_name + "_" + model_filename;
        }
        return animation_fullname;
    }

    character::action::action(character &outer, const direction &direction)
        : count_{},
          outer_{outer}
    {
        params_.push_back(direction);
    }

    static float get_sine_curve(int theta, int duration)
    {
        float sine_curve{
            std::sin((static_cast<float>(theta) / duration * D3DX_PI) - (D3DX_PI / 2))};
        sine_curve += 1.0f;
        sine_curve /= 2.0f;
        return sine_curve;
    }

    static float get_irrational_curve(int x, int duration)
    {
        float y = std::sqrt(static_cast<float>(x) / duration);
        return y;
    }

    character::step::step(character &outer, const direction &dir)
        : action{outer, dir},
          relative_direction_{direction::NONE}
    {
        for (int i{}; i < constants::ACTION_INTERVAL_FRAME + 1; ++i)
        {
            float y{get_sine_curve(i, constants::ACTION_INTERVAL_FRAME) * constants::GRID_LENGTH};

            destinations_.push_back(y);
        }
        switch (boost::get<direction>(params_.at(0)))
        {
        case direction::FRONT:
            switch (outer_.direction_)
            {
            case direction::FRONT:
                relative_direction_ = direction::FRONT;
                break;
            case direction::LEFT:
                relative_direction_ = direction::RIGHT;
                break;
            case direction::BACK:
                relative_direction_ = direction::BACK;
                break;
            case direction::RIGHT:
                relative_direction_ = direction::RIGHT;
                break;
            }
            break;
        case direction::LEFT:
            switch (outer_.direction_)
            {
            case direction::FRONT:
                relative_direction_ = direction::LEFT;
                break;
            case direction::LEFT:
                relative_direction_ = direction::FRONT;
                break;
            case direction::BACK:
                relative_direction_ = direction::RIGHT;
                break;
            case direction::RIGHT:
                relative_direction_ = direction::BACK;
                break;
            }
            break;
        case direction::BACK:
            switch (outer_.direction_)
            {
            case direction::FRONT:
                relative_direction_ = direction::BACK;
                break;
            case direction::LEFT:
                relative_direction_ = direction::LEFT;
                break;
            case direction::BACK:
                relative_direction_ = direction::FRONT;
                break;
            case direction::RIGHT:
                relative_direction_ = direction::RIGHT;
                break;
            }
            break;
        case direction::RIGHT:
            switch (outer_.direction_)
            {
            case direction::FRONT:
                relative_direction_ = direction::LEFT;
                break;
            case direction::LEFT:
                relative_direction_ = direction::BACK;
                break;
            case direction::BACK:
                relative_direction_ = direction::RIGHT;
                break;
            case direction::RIGHT:
                relative_direction_ = direction::FRONT;
                break;
            }
            break;
        }
    }

    operation::behavior_state character::step::operator()()
    {
        if (count_ == 0)
        {
            std::shared_ptr<camera> camera = outer_.operation_->get_camera();
            switch (boost::get<direction>(params_.at(0)))
            {
            case direction::FRONT:
                camera->move_position(
                    D3DXVECTOR3{0.0f, 0.0f, constants::GRID_LENGTH}, 1.0f);
                outer_.grid_position_.z += 1;
                break;
            case direction::LEFT:
                camera->move_position(
                    D3DXVECTOR3{-constants::GRID_LENGTH, 0.0f, 0.0f}, 1.0f);
                outer_.grid_position_.x -= 1;
                break;
            case direction::BACK:
                camera->move_position(
                    D3DXVECTOR3{0.0f, 0.0f, -constants::GRID_LENGTH}, 1.0f);
                outer_.grid_position_.z -= 1;
                break;
            case direction::RIGHT:
                camera->move_position(
                    D3DXVECTOR3{constants::GRID_LENGTH, 0.0f, 0.0f}, 1.0f);
                outer_.grid_position_.x += 1;
                break;
            }
            switch (relative_direction_)
            {
            case direction::FRONT:
                outer_.set_animation("Step_Front");
                break;
            case direction::LEFT:
                outer_.set_animation("Step_Left");
                break;
            case direction::BACK:
                outer_.set_animation("Step_Back");
                break;
            case direction::RIGHT:
                outer_.set_animation("Step_Right");
                break;
            }
        }
        if (constants::ACTION_INTERVAL_FRAME <= count_ &&
            (count_)*constants::ANIMATION_SPEED <
                outer_.duration_map_.at("Step_Left"))
        {
            ++count_;
            return operation::behavior_state::CANCELABLE;
        }
        else if (outer_.duration_map_.at("Step_Left") <=
                 (count_)*constants::ANIMATION_SPEED)
        {
            return operation::behavior_state::FINISH;
        }
        D3DXVECTOR3 position{outer_.position_};
        float delta{0.0f};
        size_t count_index{static_cast<size_t>(count_)};
        if (count_index + 1 < destinations_.size())
        {
            delta = destinations_.at(count_index + 1) - destinations_.at(count_index);
        }
        switch (boost::get<direction>(params_.at(0)))
        {
        case direction::FRONT:
            position.z += delta;
            break;
        case direction::LEFT:
            position.x -= delta;
            break;
        case direction::BACK:
            position.z -= delta;
            break;
        case direction::RIGHT:
            position.x += delta;
            break;
        }
        outer_.set_position(position);
        ++count_;
        return operation::behavior_state::PLAY;
    }

    character::step::~step()
    {
        if (count_ != 0)
        {
            outer_.set_position(outer_.position_);
        }
    }

    void character::step::cancel()
    {
        if (count_ != 0)
        {
            std::shared_ptr<camera> camera = outer_.operation_->get_camera();
            switch (boost::get<direction>(params_.at(0)))
            {
            case direction::FRONT:
                camera->move_position(
                    D3DXVECTOR3{0.0f, 0.0f, -constants::GRID_LENGTH}, 1.0f);
                outer_.grid_position_.z -= 1;
                break;
            case direction::LEFT:
                camera->move_position(
                    D3DXVECTOR3{constants::GRID_LENGTH, 0.0f, 0.0f}, 1.0f);
                outer_.grid_position_.x += 1;
                break;
            case direction::BACK:
                camera->move_position(
                    D3DXVECTOR3{0.0f, 0.0f, constants::GRID_LENGTH}, 1.0f);
                outer_.grid_position_.z += 1;
                break;
            case direction::RIGHT:
                camera->move_position(
                    D3DXVECTOR3{-constants::GRID_LENGTH, 0.0f, 0.0f}, 1.0f);
                outer_.grid_position_.x -= 1;
                break;
            }
            outer_.set_position(outer_.grid_position_);
        }
    }

    character::rotate::rotate(character &outer, const direction &dir)
        : action{outer, dir},
          relative_direction_{direction::NONE},
          back_up_direction_{outer.direction_}
    {
        switch (outer_.direction_)
        {
        case direction::FRONT:
            switch (boost::get<direction>(params_.at(0)))
            {
            case direction::FRONT:
                relative_direction_ = direction::NONE;
                break;
            case direction::LEFT:
                relative_direction_ = direction::LEFT;
                break;
            case direction::BACK:
                relative_direction_ = direction::BACK;
                break;
            case direction::RIGHT:
                relative_direction_ = direction::RIGHT;
                break;
            }
            break;
        case direction::LEFT:
            switch (boost::get<direction>(params_.at(0)))
            {
            case direction::FRONT:
                relative_direction_ = direction::RIGHT;
                break;
            case direction::LEFT:
                relative_direction_ = direction::NONE;
                break;
            case direction::BACK:
                relative_direction_ = direction::LEFT;
                break;
            case direction::RIGHT:
                relative_direction_ = direction::BACK;
                break;
            }
            break;
        case direction::BACK:
            switch (boost::get<direction>(params_.at(0)))
            {
            case direction::FRONT:
                relative_direction_ = direction::BACK;
                break;
            case direction::LEFT:
                relative_direction_ = direction::RIGHT;
                break;
            case direction::BACK:
                relative_direction_ = direction::NONE;
                break;
            case direction::RIGHT:
                relative_direction_ = direction::LEFT;
                break;
            }
            break;
        case direction::RIGHT:
            switch (boost::get<direction>(params_.at(0)))
            {
            case direction::FRONT:
                relative_direction_ = direction::LEFT;
                break;
            case direction::LEFT:
                relative_direction_ = direction::BACK;
                break;
            case direction::BACK:
                relative_direction_ = direction::RIGHT;
                break;
            case direction::RIGHT:
                relative_direction_ = direction::NONE;
                break;
            }
            break;
        }
    }

    operation::behavior_state character::rotate::operator()()
    {
        if (count_ == 0)
        {
            outer_.direction_ = boost::get<direction>(params_.at(0));
            switch (relative_direction_)
            {
            case direction::LEFT:
                outer_.set_animation("Rotate_Left");
                break;
            case direction::BACK:
                outer_.set_animation("Rotate_Back");
                break;
            case direction::RIGHT:
                outer_.set_animation("Rotate_Right");
                break;
            case direction::NONE:
                break;
            }
        }
        if (constants::ACTION_INTERVAL_FRAME <= count_ &&
            (count_)*constants::ANIMATION_SPEED <
                outer_.duration_map_.at("Rotate_Left"))
        {
            ++count_;
            return operation::behavior_state::CANCELABLE;
        }
        else if (outer_.duration_map_.at("Rotate_Left") <=
                 (count_)*constants::ANIMATION_SPEED)
        {
            return operation::behavior_state::FINISH;
        }
        ++count_;
        return operation::behavior_state::PLAY;
    }

    character::rotate::~rotate()
    {
        if (count_ != 0)
        {
            outer_.set_rotation(outer_.direction_);
        }
    }

    void character::rotate::cancel()
    {
        outer_.direction_ = back_up_direction_;
        outer_.set_rotation(outer_.direction_);
    }

    character::step_and_rotate::step_and_rotate(
        character &outer, const direction &step_dir, const direction &rotate_dir)
        : action{outer, direction::NONE},
          step_{outer, step_dir},
          rotate_{outer, rotate_dir} {}

    operation::behavior_state character::step_and_rotate::operator()()
    {
        step_();
        operation::behavior_state result = rotate_();
        count_ = rotate_.count_;
        return result;
    }

    character::step_and_rotate::~step_and_rotate()
    {
    }

    void character::step_and_rotate::cancel()
    {
        step_.cancel();
        rotate_.cancel();
    }

    character::attack::attack(character &outer)
        : action{outer, direction::NONE},
          availability_{true}
    {
    }

    operation::behavior_state character::attack::operator()()
    {
        if (availability_)
        {
            int x = outer_.grid_position_.x;
            int z = outer_.grid_position_.z;
            switch (outer_.direction_)
            {
            case direction::FRONT:
                ++z;
                break;
            case direction::LEFT:
                --x;
                break;
            case direction::BACK:
                --z;
                break;
            case direction::RIGHT:
                ++x;
                break;
            }
            outer_.operation_->set_offensive_position(x, z);
        }

        if (count_ == 0)
        {
            outer_.set_animation("Attack");
        }
        if ((count_)*constants::ANIMATION_SPEED >=
            outer_.duration_map_.at("Attack"))
        {
            return operation::behavior_state::FINISH;
        }
        ++count_;
        return operation::behavior_state::PLAY;
    }

    void character::attack::cancel()
    {
        availability_ = false;
    }

    character::attack::~attack()
    {
    }

    void character::set_normal_move(
        const std::string &name, const int &power, const int &max_power)
    {
        normal_move_.emplace_back(normal_move{name, power, max_power});
    }

    std::vector<std::string> character::get_normal_move()
    {
        std::vector<std::string> ret{};
        for (auto &&x : normal_move_)
        {
            ret.emplace_back(x.name_);
        }
        return ret;
    }

    std::pair<int, int> character::get_normal_move_power(const std::string &name)
    {
        decltype(normal_move_)::const_iterator it{
            std::find_if(normal_move_.cbegin(), normal_move_.cend(),
                [&](auto &&x) { return x.name_ == name; })};

        return std::make_pair(it->power_, it->max_power_);
    }

    std::vector<std::string> character::get_special_move()
    {
        std::vector<std::string> ret{};
        for (auto &&x : special_move_)
        {
            ret.emplace_back(x.name_);
        }
        return ret;
    }

    void character::set_special_move(
        const std::string &name, const int &power, const int &max_power)
    {
        special_move_.emplace_back(special_move{name, power, max_power});
    }

    std::pair<int, int> character::get_special_move_power(const std::string &name)
    {
        decltype(special_move_)::const_iterator it{
            std::find_if(special_move_.cbegin(), special_move_.cend(),
                         [&](auto &&x)
                         { return x.name_ == name; })};

        return std::make_pair(it->power_, it->max_power_);
    }

    bool character::is_tex_animation_finished(
        const std::string &x_filename, const int &layer_number)
    {
        if (mesh_map_.find(x_filename) != mesh_map_.end())
        {
            return mesh_map_.at(x_filename)->is_tex_animation_finished(layer_number);
        }
        return true;
    }

    character::normal_move::normal_move(
        const std::string &name, const int &power, const int &max_power)
        : name_{name},
          power_{power},
          max_power_{max_power}
    {
    }

    character::special_move::special_move(
        const std::string &name, const int &power, const int &max_power)
        : normal_move{name, power, max_power}
    {
    }

}
