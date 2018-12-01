#ifndef BASE_MESH_HPP
#define BASE_MESH_HPP

#include "stdafx.hpp"
#include "animation_strategy.hpp"

namespace early_go {

  // TODO : アニメーションについてはストラテジパターンでごまかす。
class base_mesh {
public:
  base_mesh(
      const std::shared_ptr<::IDirect3DDevice9>&,
      const std::string&,
      const ::D3DXVECTOR3&,
      const ::D3DXVECTOR3&);
  enum class combine_type{
    NORMAL,
//    ADDITION,
//    MULTIPLICATION,
  };
  static constexpr int LAYER_NUMBER{8};
  void set_animation(const std::string&);
  void set_default_animation(const std::string&);
  void set_animation_config(const std::string&, const bool&, const float&);
  void render(const ::D3DXMATRIX&,
              const ::D3DXMATRIX&,
              const ::D3DXVECTOR4&,
              const float&);

  void set_dynamic_texture(const std::string&, const int&, const combine_type&);
  void set_dynamic_texture_position(const int&,
                                    const ::D3DXVECTOR2&);
  void set_dynamic_texture_opacity(const int&,
                                   const float&);

  static const int TEXTURE_PIXEL_SIZE;
  void set_dynamic_message(const int&,
                           const std::string&,
                           const bool& = false,
                           const ::RECT& = {
                               0,
                               0,
                               TEXTURE_PIXEL_SIZE - 1,
                               TEXTURE_PIXEL_SIZE - 1},
                           const int& = RGB(0xff, 0xff, 0xff),
                           const std::string& = "ＭＳ Ｐゴシック",
                           const int& = 40,
                           const int& = 0);

  void set_dynamic_message_color(const int&, const ::D3DXVECTOR4&);
  void set_position(const ::D3DXVECTOR3&);
  void set_rotation(const ::D3DXVECTOR3&);
protected:
  std::shared_ptr<::IDirect3DDevice9>            d3d_device_;
  std::unique_ptr<::ID3DXEffect, custom_deleter> effect_;
  std::unique_ptr<animation_strategy>            animation_strategy_;
  struct dynamic_texture {
    std::array<
        std::shared_ptr<::IDirect3DTexture9>,
        LAYER_NUMBER
    > textures_;
    std::array<::D3DXVECTOR4, LAYER_NUMBER> positions_;
    std::array<float, LAYER_NUMBER>         opacities_;
    std::array<::D3DXVECTOR4, LAYER_NUMBER> colors_;

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

      std::shared_ptr<::IDirect3DTexture9>& texture_;
      const std::string                     message_;
      const bool                            is_message_animated_;
      const ::RECT                          rect_;
      const int                             color_;
      int                                   font_width_sum_;
      int                                   font_height_sum_;
      ::TEXTMETRIC                          text_metric_;
      ::HDC                                 hdc_;
      ::HFONT                               hfont_;
      std::size_t                           character_index_;
      std::vector<::DWORD*>                 texture_buffer_;
    };
    std::array<
        std::shared_ptr<text_message_writer>,
        LAYER_NUMBER
    > writer_;
  } dynamic_texture_;
  ::D3DXHANDLE                                   texture_position_handle_;
  ::D3DXHANDLE                                   texture_opacity_handle_;
  ::D3DXHANDLE                                   light_normal_handle_;
  ::D3DXHANDLE                                   brightness_handle_;
  ::D3DXHANDLE                                   mesh_texture_handle_;
  ::D3DXHANDLE                                   diffuse_handle_;
  std::array<::D3DXHANDLE, LAYER_NUMBER>         texture_handle_;
  ::D3DXVECTOR3                                  position_;
  ::D3DXVECTOR3                                  rotation_;
private:
  virtual void do_render(const ::D3DXMATRIX&, const ::D3DXMATRIX&) = 0;
};
}

#endif
