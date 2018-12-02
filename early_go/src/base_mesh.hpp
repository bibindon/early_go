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

  static const int64_t TEXTURE_PIXEL_SIZE;
  void set_dynamic_message(const int&,
                           const std::string&,
                           const bool& = false,
                           const ::RECT& = {
                               0,
                               0,
                               static_cast<LONG>(TEXTURE_PIXEL_SIZE) - 1,
                               static_cast<LONG>(TEXTURE_PIXEL_SIZE) - 1},
                           const int& = RGB(0xff, 0xff, 0xff),
                           const std::string& = "ＭＳ Ｐゴシック",
                           const int& = 40,
                           const int& = 0);

  void set_dynamic_message_color(const int&, const ::D3DXVECTOR4&);
  void set_position(const ::D3DXVECTOR3&);
  void set_rotation(const ::D3DXVECTOR3&);
  void set_shake_texture();
protected:
  std::shared_ptr<::IDirect3DDevice9>            d3d_device_;
  std::unique_ptr<::ID3DXEffect, custom_deleter> effect_;
  std::unique_ptr<animation_strategy>            animation_strategy_;
  struct dynamic_texture {
    static constexpr int LAYER_NUMBER{8};
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
                          ::UINT,
                          ::UINT,
                          const std::string&,
                          const int&,
                          const int&);
      ~text_message_writer();
      void operator()();
      bool write_character();

    private:
      std::shared_ptr<::IDirect3DTexture9>& texture_;
      const std::string                     message_;
      const bool                            is_message_animated_;
      const ::RECT                          rect_;
      const int                             color_;
      ::UINT                                font_width_sum_;
      ::UINT                                font_height_sum_;
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
    struct texture_shaker {
      texture_shaker();
      void operator()(base_mesh&);
    private:
      int count_;
      // A fixed value is used. That's because random value doesn't become
      // appropriate. 
      static constexpr int SHAKE_POSITIONS_SIZE{16};
      const static std::array<::D3DXVECTOR2, SHAKE_POSITIONS_SIZE>
          SHAKING_POSITIONS;
      const static int SHAKE_FRAME;
      const static int SHAKE_DURATION;
      ::D3DXVECTOR2 current_position_;
      ::D3DXVECTOR2 previous_position_;
    };
    std::shared_ptr<texture_shaker> texture_shaker_;
  } dynamic_texture_;
  ::D3DXHANDLE                                   texture_position_handle_;
  ::D3DXHANDLE                                   texture_opacity_handle_;
  ::D3DXHANDLE                                   light_normal_handle_;
  ::D3DXHANDLE                                   brightness_handle_;
  ::D3DXHANDLE                                   mesh_texture_handle_;
  ::D3DXHANDLE                                   diffuse_handle_;
  std::array<::D3DXHANDLE, dynamic_texture::LAYER_NUMBER> texture_handle_;
  ::D3DXVECTOR3                                  position_;
  ::D3DXVECTOR3                                  rotation_;
private:
  virtual void render(const ::D3DXMATRIX&, const ::D3DXMATRIX&) = 0;
};
}

#endif
