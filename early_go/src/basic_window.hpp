#ifndef BASIC_WINDOW_HPP
#define BASIC_WINDOW_HPP
#include "stdafx.hpp"

namespace early_go {

class mesh;
class animation_mesh;
class skinned_animation_mesh;

/*
 * A class that handles basic window processings.
 */
class basic_window
{
public:
  /*
   * A struct for showing debug strings anytime. The reason why this struct is
   * not prototype decralation is necessary that a client of this class can look
   * the 'render_string' function.
   */
  struct render_string_object {
    static std::weak_ptr<::ID3DXFont> swp_id3dx_font_;
    static void render_string(const std::string&, const int&, const int&);
  };

  explicit basic_window(const ::HINSTANCE&);
  int operator()();

private:

  ::MSG                                         msg_;
  std::unique_ptr<::IDirect3D9, custom_deleter> up_direct3d9_;
  ::D3DPRESENT_PARAMETERS                       d3d_present_parameters_;
  std::shared_ptr<::IDirect3DDevice9>           sp_direct3d_device9_;
  std::shared_ptr<::ID3DXFont>                  sp_id3dx_font_;
  std::shared_ptr<animation_mesh>               sp_animation_mesh_;
  std::shared_ptr<skinned_animation_mesh>       sp_skinned_animation_mesh_;
  std::shared_ptr<mesh>                         sp_mesh_;
  std::shared_ptr<mesh>                         sp_mesh2_;

  ::D3DXMATRIXA16                               mat_view_;
  ::D3DXMATRIXA16                               mat_projection_;
  ::D3DXVECTOR3                                 light_direction_;
  float                                         light_brightness_;
  ::D3DXVECTOR3                                 vec_eye_position_;
  ::D3DXVECTOR3                                 vec_look_at_position_;

  void initialize_direct3d(const ::HWND&);
  void render();
};
} /* namespace early_go */
#endif
