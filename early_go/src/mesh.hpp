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

  void set_dynamic_texture(const std::string&, const int&, const combine_type&);
  void set_dynamic_texture_position(const int&,
                                    const ::D3DXVECTOR2&);
  void set_dynamic_texture_opacity(const int&,
                                   const float&);

  void set_dynamic_message(const int&,
                           const std::string&,
                           const bool& = false,
                           const ::RECT& = {
                               0,
                               0,
                               dynamic_texture::PIXEL_NUMBER - 1,
                               dynamic_texture::PIXEL_NUMBER - 1},
                           const int& = RGB(0xff, 0xff, 0xff),
                           const std::string& = "�l�r �o�S�V�b�N",
                           const int& = 40,
                           const int& = 0);

  void set_dynamic_message_color(const int&, const ::D3DXVECTOR4&);

  void render(const ::D3DXMATRIX&,
              const ::D3DXMATRIX&,
              const::D3DXVECTOR4&,
              const float&);

private:
  struct dynamic_texture {
    static constexpr int LAYER_NUMBER = 8;
    static const int     PIXEL_NUMBER;
    std::array<
        std::shared_ptr<::IDirect3DTexture9>,
        LAYER_NUMBER
    > arsp_texture_;
    std::array<::D3DXVECTOR4, LAYER_NUMBER> arvec2_position_;
    std::array<float, LAYER_NUMBER>         arf_opacity_;
    std::array<::D3DXVECTOR4, LAYER_NUMBER> arvec4_color_;

    struct text_message_writer {
      text_message_writer(std::shared_ptr<::IDirect3DDevice9>,
                          std::shared_ptr<::IDirect3DTexture9>&,
                          const std::string,
                          const bool,
                          const ::RECT,
                          const int,
                          int,
                          int,
                          const std::string&,
                          const int&,
                          const int&);
      ~text_message_writer();
      void operator()();
      bool write_character();

      std::shared_ptr<::IDirect3DTexture9>& sp_texture_;
      const std::string                     krsz_message_;
      const bool                            krb_animation_;
      const ::RECT                          kr_rect_;
      const int                             kri_color_;
      int                                   font_width_sum_;
      int                                   font_height_sum_;
      ::TEXTMETRIC                          text_metric_;
      ::HDC                                 hdc_;
      ::HFONT                               hfont_;
      std::size_t                           character_index_;
      std::vector<::DWORD*>                 vpw_tex_buffer_;
    };
    std::array<
        std::shared_ptr<text_message_writer>,
        LAYER_NUMBER
    > arsp_writer;
  } dynamic_texture_;
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
  std::array<
      ::D3DXHANDLE, dynamic_texture::LAYER_NUMBER
  > ar_d3dx_handle_texture_;
  ::D3DXHANDLE                                   d3dx_handle_diffuse_;
  ::D3DXHANDLE                                   d3dx_handle_texture_position_;
  ::D3DXHANDLE                                   d3dx_handle_texture_opacity_;
  std::vector<::D3DCOLORVALUE>                   vec_d3d_color_;
  std::vector<
      std::unique_ptr<
          ::IDirect3DTexture9, custom_deleter
      >
  > vecup_mesh_texture_;
  ::D3DXVECTOR3                                  vec3_center_coodinate_;
  float                                          f_radius_;
  float                                          f_scale_;

  void update_texture();
};
} /* namespace early_go */
#endif
