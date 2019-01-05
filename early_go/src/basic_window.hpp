#ifndef BASIC_WINDOW_HPP
#define BASIC_WINDOW_HPP
#include "stdafx.hpp"

namespace early_go {

class mesh;
class animation_mesh;
class skinned_animation_mesh;
class camera;
class character;
class base_mesh;
class operation;

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
    static std::weak_ptr<::ID3DXFont> weak_font_;
    static void render_string(const std::string&, const int&, const int&);
  };

  explicit basic_window(const ::HINSTANCE&);
  int operator()();
  std::shared_ptr<character> get_main_character();
  std::shared_ptr<character> get_enemy_character();

private:

  ::MSG                                         msg_;
  std::unique_ptr<::IDirect3D9, custom_deleter> direct3d9_;
  ::D3DPRESENT_PARAMETERS                       d3d_present_parameters_;
  std::shared_ptr<::IDirect3DDevice9>           d3d_device_;
  std::shared_ptr<::ID3DXFont>                  font_;
  std::shared_ptr<base_mesh>                    animation_mesh_;
  std::shared_ptr<base_mesh>                    skinned_animation_mesh_;
  std::shared_ptr<base_mesh>                    skinned_animation_mesh2_;
  std::shared_ptr<base_mesh>                    mesh_;
  std::shared_ptr<base_mesh>                    mesh2_;

  std::shared_ptr<character>                    early_;
  std::shared_ptr<character>                    suo_;

  ::D3DXVECTOR3                                 light_direction_;
  float                                         light_brightness_;
  std::shared_ptr<camera>                       camera_;

  void initialize_direct3d(const ::HWND&);
  void debug();
  void render();

  std::shared_ptr<operation>                        operation_;
};
} /* namespace early_go */
#endif
