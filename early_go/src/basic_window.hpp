#ifndef BASIC_WINDOW_HPP
#define BASIC_WINDOW_HPP
#include "stdafx.hpp"

namespace early_go {

class mesh;
class animation_mesh;

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
  /* prototype declaration */
  struct window_procedure_object;

  ::MSG                                         msg_;
  std::unique_ptr<::IDirect3D9, custom_deleter> up_direct3d9_;
  ::D3DPRESENT_PARAMETERS                       d3d_present_parameters_;
  std::shared_ptr<::IDirect3DDevice9>           sp_direct3d_device9_;
  std::shared_ptr<::ID3DXFont>                  sp_id3dx_font_;
  std::shared_ptr<animation_mesh>               sp_animation_mesh_;
  std::shared_ptr<animation_mesh>               sp_animation_mesh2_;
  std::shared_ptr<mesh>                         sp_mesh_;
  std::shared_ptr<mesh>                         sp_mesh2_;

  void initialize_direct3d(const ::HWND&);
  void render();
  void update_light();
};
} /* namespace early_go */
#endif
