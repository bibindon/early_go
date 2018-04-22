#ifndef MESH_HPP
#define MESH_HPP
#include "stdafx.hpp"

namespace early_go {
class mesh
{
public:
  enum class combine_type{
    NORMAL,
//    ADDITION,
//    MULTIPLICATION,
  };

  mesh(const std::shared_ptr<::IDirect3DDevice9>&,
       const std::string&,
       const ::D3DXVECTOR3&,
       const float&);

  ~mesh();

  void set_dynamic_texture(const std::string&, const int&, const combine_type&);
  void set_dynamic_texture_position(const int&,
                                    const ::D3DXVECTOR2&);
  void set_dynamic_texture_opacity(const int&,
                                   const float&);

  void set_dynamic_message(const int&,
                           const std::string&,
                           const ::RECT& = {0, 0, 511, 511},
                           const int& = RGB(0xff, 0xff, 0xff),
                           const std::string& = "‚l‚r ‚oƒSƒVƒbƒN",
                           const int& = 40,
                           const int& = 0,
                           const bool& = false);

  void set_dynamic_message_color(const int&, const ::D3DXVECTOR4&);

  void render(const ::D3DXMATRIX&,
              const ::D3DXMATRIX&,
              const::D3DXVECTOR4&,
              const float&);

private:
  const static std::string                       SHADER_FILENAME;
  std::shared_ptr<::IDirect3DDevice9>            sp_direct3d_device9_;
  ::D3DXVECTOR3                                  vec3_position_;
  std::unique_ptr<::ID3DXMesh, custom_deleter>   up_d3dx_mesh_;
  ::DWORD                                        dw_materials_number_;
  std::unique_ptr<::ID3DXEffect, custom_deleter> up_d3dx_effect_;
  ::D3DXHANDLE                                   d3dx_handle_world_view_proj_;
  ::D3DXHANDLE                                   d3dx_handle_light_normal_;
  ::D3DXHANDLE                                   d3dx_handle_brightness_;
  ::D3DXHANDLE                                   d3dx_handle_mesh_texture_;
  std::array<::D3DXHANDLE, 8>                    ar_d3dx_handle_texture_;
  ::D3DXHANDLE                                   d3dx_handle_diffuse_;
  ::D3DXHANDLE                                   d3dx_handle_texture_position_;
  ::D3DXHANDLE                                   d3dx_handle_texture_opacity_;
  std::vector<::D3DCOLORVALUE>                   vec_d3d_color_;
  std::vector<
      std::unique_ptr<
          ::IDirect3DTexture9, custom_deleter
      >
  > vecup_mesh_texture_;
  ::D3DXVECTOR3 vec3_center_coodinate_;
  float         f_radius_;
  float         f_scale_;

  struct dynamic_texture {
    std::array<
      std::unique_ptr<
          ::IDirect3DTexture9, custom_deleter
      >, 8> arup_texture_;
    std::array<::D3DXVECTOR4, 8> arvec2_position_;
    std::array<float, 8> arf_opacity_;
    std::array<::D3DXVECTOR4, 8> arvec4_color_;
  } dynamic_texture_;
  void update_texture();
  ::HDC hdc_;
  ::HFONT hfont_;
};
} /* namespace early_go */
#endif
