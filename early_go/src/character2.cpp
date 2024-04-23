#include "stdafx.hpp"

#include "character2.hpp"
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
void character2::update()
{
    for (const auto &x : mesh_map_skin_)
    {
        x.second->update();
    }
}
void character2::render_ztex()
{
    for (const auto &x : mesh_map_anim_)
    {
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
}
D3DXVECTOR3 character2::get_pos()
{
    return pos_;
}
void character2::set_rotate(const D3DXVECTOR3& rotate)
{
    rotate_ = rotate;
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
int character2::get_health() const
{
    return health_;
}

void character2::set_health(const int &health)
{
    health_ = health;
}

int character2::get_max_health() const
{
    return max_health_;
}

void character2::set_max_health(const int &health)
{
    max_health_ = health;
}
}
