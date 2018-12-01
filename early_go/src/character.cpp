#include "stdafx.hpp"

#include <regex>
#include <cctype>

#include "base_mesh.hpp"
#include "character.hpp"

namespace early_go {
character::character(const std::shared_ptr<::IDirect3DDevice9>& d3d_device,
                     const ::D3DXVECTOR3&                       position,
                     const ::D3DXVECTOR3&                       rotation,
                     const float&                               size)
  : d3d_device_{d3d_device},
    position_{position},
    rotation_{rotation},
    size_{size}
{
}

character::~character()
{
}

void character::set_default_animation(const std::string& animation_name)
{
  for (const auto& x : mesh_map_) {
    std::string animation_fullname{
        create_animation_fullname(animation_name, x.first)};

    x.second->set_default_animation(animation_fullname);
  }
}

void character::set_animation_config(const std::string& animation_name,
                                     const bool&        loop,
                                     const float&       duration)
{
  for (const auto& x : mesh_map_) {
    std::string animation_fullname{
        create_animation_fullname(animation_name, x.first)};

    x.second->set_animation_config(animation_fullname, loop, duration);
  }
}

void character::render(const ::D3DXMATRIX&  view_matrix,
                       const ::D3DXMATRIX&  projection_matrix,
                       const ::D3DXVECTOR4& normal_light,
                       const float&         brightness)
{
  for (const auto& x : mesh_map_) {
    x.second->render(view_matrix, projection_matrix, normal_light, brightness);
  }
}

void character::set_position(const ::D3DXVECTOR3& position)
{
  position_ = position;
  for (const auto& x : mesh_map_) {
    x.second->set_position(position_);
  }
}

void character::set_rotation(const ::D3DXVECTOR3& rotation)
{
  rotation_ = rotation;
  for (const auto& x : mesh_map_) {
    x.second->set_rotation(rotation_);
  }
}

void character::set_dynamic_texture(const std::string& x_filename,
                                    const std::string& texture_filename,
                                    const int&         layer_number,
                                    const base_mesh::combine_type& combine_type)
{
  if (mesh_map_.find(x_filename) != mesh_map_.end()) {
    mesh_map_.at(x_filename)->set_dynamic_texture(
        texture_filename, layer_number, combine_type);
  }
}

void character::set_dynamic_texture_position(const std::string&   x_filename,
                                             const int&           layer_number,
                                             const ::D3DXVECTOR2& position)
{
  if (mesh_map_.find(x_filename) != mesh_map_.end()) {
    mesh_map_.at(x_filename)->set_dynamic_texture_position(
        layer_number, position);
  }
}

void character::set_dynamic_texture_opacity(const std::string& x_filename,
                                            const int&         layer_number,
                                            const float&       opacity)
{
  if (mesh_map_.find(x_filename) != mesh_map_.end()) {
    mesh_map_.at(x_filename)->set_dynamic_texture_opacity(
        layer_number, opacity);
  }
}

void character::set_dynamic_message(const std::string& x_filename,
                                    const int&         layer_number,
                                    const std::string& message,
                                    const bool&        animation,
                                    const ::RECT&      rect,
                                    const int&         color,
                                    const std::string& fontname,
                                    const int&         size,
                                    const int&         weight)
{
  if (mesh_map_.find(x_filename) != mesh_map_.end()) {
    mesh_map_.at(x_filename)->set_dynamic_message(
        layer_number, message, animation, rect, color, fontname, size, weight);
  }
}

void character::set_dynamic_message_color(const std::string& x_filename,
                                          const int& layer_number,
                                          const::D3DXVECTOR4& color)
{
  if (mesh_map_.find(x_filename) != mesh_map_.end()) {
    mesh_map_.at(x_filename)->set_dynamic_message_color(layer_number, color);
  }
}

void character::set_animation(const std::string& animation_set)
{
  for (const auto& x : mesh_map_) {
    std::string animation_fullname{
        create_animation_fullname(animation_set, x.first)};

    x.second->set_animation(animation_fullname);
  }
}

// "Idle" + "hoge/piyo/hair.x" -> "Idle_Hair"
std::string character::create_animation_fullname(
    const std::string& former_name, const std::string& model_fullname)
{
  std::regex reg{R"(.*/(.*)\.x)"};
  std::smatch match{};
  std::string model_filename{};
  std::string animation_fullname{};

  if (!std::regex_match(model_fullname, match, reg) || match.size() != 2) {
    THROW_WITH_TRACE("A model filename doesn't match regular expression.: " +
        model_fullname);
  } else {
    model_filename = match[1].str();
    unsigned char c = std::toupper(model_filename.at(0));
    model_filename.erase(0, 1);
    model_filename.insert(model_filename.begin(), c);
    animation_fullname = former_name + "_" + model_filename;
  }
  return animation_fullname;
}
}
