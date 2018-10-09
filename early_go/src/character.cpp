#include "stdafx.hpp"

#include "base_mesh.hpp"
#include "character.hpp"

namespace early_go {
character::character(const std::shared_ptr<::IDirect3DDevice9>& d3d_device,
                     const ::D3DXVECTOR3&                       position,
                     const float&                               size)
  : d3d_device_{d3d_device},
    position_{position},
    size_{size}
{
}

character::~character()
{
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

void character::set_dynamic_texture_position(const std::string&  x_filename,
                                             const int&          layer_number,
                                             const::D3DXVECTOR2& position)
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

}
