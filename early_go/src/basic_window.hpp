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
  void render();

  struct shake_novel_window {
    shake_novel_window();
    bool operator()(std::shared_ptr<character>, const std::string&);
  private:
    int  count_;
    // A fixed value is used. That's because random value doesn't become
    // appropriate. 
    static constexpr int SHAKE_POSITIONS_SIZE{16};
    const static std::array<::D3DXVECTOR2, SHAKE_POSITIONS_SIZE>
        SHAKING_POSITIONS;
    const static int SHAKE_FRAME;
    const static int SHAKE_DURATION;
    ::D3DXVECTOR2 current_shaking_position_;
    ::D3DXVECTOR2 previous_shaking_position_;
  };
  std::shared_ptr<shake_novel_window> shake_novel_window_;
};
} /* namespace early_go */
#endif
