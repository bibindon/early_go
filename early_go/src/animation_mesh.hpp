#ifndef ANIMATION_MESH_HPP
#define ANIMATION_MESH_HPP
#include "stdafx.hpp"

namespace early_go {

class animation_mesh_allocator;

/*
 * A class that provides operations for a mesh file having animations.
 */
class animation_mesh {
public:
  animation_mesh(const std::shared_ptr<::IDirect3DDevice9>&,
                 const std::string&,
                 const ::D3DXVECTOR3&,
                 const float&);
  void play_animation_set(const std::size_t&);
  void play_animation_set(const std::string&);
  void render(const ::D3DXMATRIX&,
              const ::D3DXMATRIX&,
              const::D3DXVECTOR4&,
              const float&);
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

  const static std::string                   SHADER_FILENAME;
  std::vector<
      std::unique_ptr<
          ::ID3DXAnimationSet, custom_deleter
      >
  > vecup_animation_set_;
  bool                                       b_play_animation_;
  float                                      f_animation_time_;
  std::shared_ptr<::IDirect3DDevice9>        sp_direct3d_device9_;
  std::shared_ptr<animation_mesh_allocator>  sp_animation_mesh_allocator_;
  std::unique_ptr<::D3DXFRAME,
                  frame_root_deleter_object> up_d3dx_frame_root_;
  std::unique_ptr<::ID3DXAnimationController,
                  custom_deleter>            up_d3dx_animation_controller_;
  ::D3DXVECTOR3                              vec3_position_;
  ::D3DXMATRIX                               mat_rotation_;
  ::D3DXMATRIX                               mat_view_;
  ::D3DXMATRIX                               mat_projection_;

  ::D3DXVECTOR3 vec3_center_coodinate_{};
  float         f_radius_{};
  float         f_scale_{};

  // For effect.
  std::unique_ptr<::ID3DXEffect, custom_deleter> up_d3dx_effect_;
  ::D3DXHANDLE                                   d3dx_handle_world_;
  ::D3DXHANDLE                                   d3dx_handle_world_view_proj_;
  ::D3DXHANDLE                                   d3dx_handle_light_normal_;
  ::D3DXHANDLE                                   d3dx_handle_brightness_;
  ::D3DXHANDLE                                   d3dx_handle_texture_;
  ::D3DXHANDLE                                   d3dx_handle_diffuse_;

  void update_frame_matrix(const ::LPD3DXFRAME, const ::LPD3DXMATRIX);
  void render_frame(const ::LPD3DXFRAME);
  void render_mesh_container(const ::LPD3DXMESHCONTAINER, const ::LPD3DXFRAME);
};
} /* namespace early_go */
#endif
