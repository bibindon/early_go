#include "stdafx.hpp"

#include "character2.hpp"
#include "action.hpp"
#include <regex>

using std::shared_ptr;
using std::string;
using std::vector;

namespace early_go
{
character2::character2(
    const std::shared_ptr<IDirect3DDevice9>& d3dx_device,
    const cv::Point3i& pos,
    const direction& direction,
    const float scale)
{
}
character2::~character2()
{
}
void character2::add_shadow_mesh_anim(const std::shared_ptr<shadow_mesh_anim>& mesh)
{
    mesh_map_anim_.emplace(mesh->x_filename(), mesh);
}
void character2::add_shadow_mesh_skin(const std::shared_ptr<shadow_mesh_skin>& mesh)
{
    mesh_map_skin_.emplace(mesh->x_filename(), mesh);
}
void character2::update(const shared_ptr<stage>& _stage)
{
    for (const auto &x : mesh_map_anim_)
    {
        x.second->update();
    }
    for (const auto &x : mesh_map_skin_)
    {
        x.second->update();
    }
    if (current_action_ != nullptr && _stage != nullptr)
    {
        action_state_ = current_action_->update(_stage);
        if (action_state_ == operation::behavior_state::FINISH)
        {
            current_action_.reset();
        }
    }
}
void character2::render_ztex()
{
    for (const auto &x : mesh_map_anim_)
    {
        x.second->set_position(pos_);
        x.second->set_rotate(rotate_);
        x.second->set_scale(scale_);
        x.second->ztex_render();
    }
    for (const auto &x : mesh_map_skin_)
    {
        x.second->set_position(pos_);
        x.second->set_rotate(rotate_);
        x.second->set_scale(scale_);
        x.second->ztex_render();
    }
}
void character2::render_depth()
{
    for (const auto &x : mesh_map_anim_)
    {
        x.second->set_position(pos_);
        x.second->set_rotate(rotate_);
        x.second->set_scale(scale_);
        x.second->render();
    }
    for (const auto &x : mesh_map_skin_)
    {
        x.second->set_position(pos_);
        x.second->set_rotate(rotate_);
        x.second->set_scale(scale_);
        x.second->render();
    }
}
void character2::set_grid_position(vec3i pos)
{
    // 位置や向きを決定しても即座には反映されず、
    // 移動や回転のアニメーションをしている時間がある。
    // そのため
    // grid_position_とpos_
    // direction_とrotate_がそれぞれ用意されており、違いを考慮する必要がある。
    grid_position_ = pos;
}
void character2::set_direction(const direction direction)
{
    // 位置や向きを決定しても即座には反映されず、
    // 移動や回転のアニメーションをしている時間がある。
    // そのため
    // grid_position_とpos_
    // direction_とrotate_がそれぞれ用意されており、違いを考慮する必要がある。
    direction_ = direction;
}
void character2::set_default_animation(const std::string &animation_name)
{
    for (const auto &x : mesh_map_anim_)
    {
        std::string animation_fullname { create_animation_fullname(animation_name, x.first) };
        x.second->set_default_animation(animation_fullname);
    }
    for (const auto &x : mesh_map_skin_)
    {
        std::string animation_fullname { create_animation_fullname(animation_name, x.first) };
        x.second->set_default_animation(animation_fullname);
    }
}
void character2::set_animation_config(
    const std::string &animation_name,
    const bool &loop,
    const float &duration)
{
    duration_map_.emplace(animation_name, duration);
    for (const auto &x : mesh_map_anim_)
    {
        std::string animation_fullname { create_animation_fullname(animation_name, x.first) };
        x.second->set_animation_config(animation_fullname, loop, duration);
    }
    for (const auto &x : mesh_map_skin_)
    {
        std::string animation_fullname { create_animation_fullname(animation_name, x.first) };
        x.second->set_animation_config(animation_fullname, loop, duration);
    }
}
void character2::set_animation(const std::string &animation_name)
{
    for (const auto &x : mesh_map_anim_)
    {
        std::string animation_fullname { create_animation_fullname(animation_name, x.first) };
        x.second->set_animation(animation_fullname);
    }
    for (const auto &x : mesh_map_skin_)
    {
        std::string animation_fullname { create_animation_fullname(animation_name, x.first) };
        x.second->set_animation(animation_fullname);
    }
}
void character2::set_pos(const D3DXVECTOR3& pos)
{
    pos_ = pos;
    for (const auto &x : mesh_map_anim_)
    {
        x.second->set_position(pos);
    }
    for (const auto &x : mesh_map_skin_)
    {
        x.second->set_position(pos);
    }
}
D3DXVECTOR3 character2::get_pos()
{
    return pos_;
}
void character2::set_rotate(const D3DXVECTOR3& rotate)
{
    rotate_ = rotate;
}
void character2::set_rotate_by_direction(const direction dir)
{
    D3DXVECTOR3 rotation { };
    switch (dir)
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

    set_rotate(rotation);
}
void character2::set_scale(const float scale)
{
    scale_ = scale;
}
// "Idle" + "hoge/piyo/hair.x" -> "Idle_Hair"
std::string character2::create_animation_fullname(
    const std::string &former_name, const std::string &model_fullname)
{
    std::regex reg{R"(.*/(.*)\.x)"};
    std::smatch match{};
    std::string model_filename{};
    std::string animation_fullname{};

    if (!std::regex_match(model_fullname, match, reg) || match.size() != 2)
    {
        THROW_WITH_TRACE("A model filename doesn't match regular expression.: " + model_fullname);
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
void character2::set_action_step(const direction step_dir)
{
    current_action_.reset( new_crt step { *this, step_dir });
}
void character2::set_action_rotate(const direction rotate_dir)
{
    current_action_.reset(new_crt rotate { *this, rotate_dir });
}
void character2::set_action_step_and_rotate(const direction step_dir, const direction rotate_dir)
{
    current_action_.reset(new_crt step_and_rotate { *this, step_dir, rotate_dir });
}
void character2::set_action_attack()
{
    current_action_.reset(new_crt attack { *this });
}
void character2::cancel_action()
{
    if (current_action_ != nullptr)
    {
        current_action_->cancel();
        current_action_.reset();
    }
}
shared_ptr<dynamic_tex> character2::get_dynamic_tex(const std::string& x_filename)
{
    return dynamic_tex_;
}
}
