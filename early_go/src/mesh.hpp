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
  void render();

private:
  const static std::string HLSL_FILENAME;
  ::D3DXVECTOR3                                  vec_position_;
  std::unique_ptr<::ID3DXMesh, custom_deleter>   up_d3dx_mesh_;
  ::DWORD                                        dw_materials_number_;
  std::unique_ptr<::ID3DXEffect, custom_deleter> up_d3dx_effect_;
  ::D3DXHANDLE                                   d3dx_handle_world_view_proj_;
  ::D3DXHANDLE                                   d3dx_handle_light_position_;
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
