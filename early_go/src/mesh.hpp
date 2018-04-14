#ifndef MESH_HPP
#define MESH_HPP
#include "stdafx.hpp"

namespace early_go {
class mesh
{
public:
  mesh(const std::shared_ptr<::IDirect3DDevice9>&,
       const std::string&,
       const ::D3DXVECTOR3& = ::D3DVECTOR{});

  void render(const ::D3DXMATRIXA16&,
 const ::D3DXMATRIXA16&, const::D3DXVECTOR4&, const float&);

private:
  const static std::string                       SHADER_FILENAME;
  ::D3DXVECTOR3                                  vec3_position_;
  std::unique_ptr<::ID3DXMesh, custom_deleter>   up_d3dx_mesh_;
  ::DWORD                                        dw_materials_number_;
  std::unique_ptr<::ID3DXEffect, custom_deleter> up_d3dx_effect_;
  ::D3DXHANDLE                                   d3dx_handle_world_view_proj_;
  ::D3DXHANDLE                                   d3dx_handle_light_normal_;
  ::D3DXHANDLE                                   d3dx_handle_brightness_;
  ::D3DXHANDLE                                   d3dx_handle_scale_;
  ::D3DXHANDLE                                   d3dx_handle_texture_;
  ::D3DXHANDLE                                   d3dx_handle_diffuse_;
  std::vector<::D3DCOLORVALUE>                   vec_d3d_color_;
  std::vector<
      std::unique_ptr<
          ::IDirect3DTexture9, custom_deleter
      >
  > vecup_mesh_texture_;
};
} /* namespace early_go */
#endif
