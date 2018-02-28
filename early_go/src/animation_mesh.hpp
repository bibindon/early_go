#ifndef ANIMATION_MESH_HPP
#define ANIMATION_MESH_HPP

#include "inline_macro.hpp"

namespace early_go {

class animation_mesh_allocator;

/*
 * A class that provides operations for a mesh file having animations.
 */
class animation_mesh {
public:
  animation_mesh(const std::shared_ptr<::IDirect3DDevice9>&,
                 const std::string&);
  void render(const ::LPD3DXMATRIX);
  bool get_play_animation() const;
  void set_play_animation(const bool&);
  float get_animation_time() const;

private:
  /*
   * A struct which deletes the root '::LPD3DXFRAME' object. The reason why the
   * following struct is not a prototype declaration is necessary for the
   * declaration of the following variable using std::unique_ptr.
   */
  struct frame_root_deleter_object
  {
    std::shared_ptr<animation_mesh_allocator> sp_animation_mesh_allocator_;

    void operator()(const ::LPD3DXFRAME);
    void release_mesh_allocator(const ::LPD3DXFRAME);
  };

  bool                                       b_play_animation_;
  float                                      f_animation_time_;
  std::shared_ptr<::IDirect3DDevice9>        sp_direct3d_device9_;
  std::shared_ptr<animation_mesh_allocator>  sp_animation_mesh_allocator_;
  std::unique_ptr<::D3DXFRAME,
                  frame_root_deleter_object> up_d3dx_frame_root_;
  std::unique_ptr<::ID3DXAnimationController,
                  custom_deleter>            up_d3dx_animation_controller_;

  void update_frame_matrix(const ::LPD3DXFRAME, const ::LPD3DXMATRIX);
  void render_frame(const ::LPD3DXFRAME);
  void render_mesh_container(const ::LPD3DXMESHCONTAINER, const ::LPD3DXFRAME);
};
} /* namespace early_go */
#endif
