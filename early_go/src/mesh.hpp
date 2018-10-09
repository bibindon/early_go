#ifndef MESH_HPP
#define MESH_HPP
#include "stdafx.hpp"
#include "base_mesh.hpp"

namespace early_go {
class mesh : public base_mesh {
public:

  mesh(const std::shared_ptr<::IDirect3DDevice9>&,
       const std::string&,
       const ::D3DXVECTOR3&,
       const float&);

private:
  const static std::string                       SHADER_FILENAME;
  std::unique_ptr<::ID3DXMesh, custom_deleter>   up_d3dx_mesh_;
  ::DWORD                                        dw_materials_number_;
  ::D3DXHANDLE                                   d3dx_handle_world_view_proj_;
  std::vector<::D3DCOLORVALUE>                   vec_d3d_color_;
  std::vector<
      std::unique_ptr<
          ::IDirect3DTexture9, custom_deleter
      >
  > vecup_mesh_texture_;
  ::D3DXVECTOR3                                  vec3_center_coodinate_;
  float                                          f_radius_;
  float                                          f_scale_;

  void do_render(const ::D3DXMATRIX&, const ::D3DXMATRIX&) override;
};
} /* namespace early_go */
#endif
