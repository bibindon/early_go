#ifndef SKINNED_ANIMATION_MESH_HPP
#define SKINNED_ANIMATION_MESH_HPP
#include "stdafx.hpp"
#include "base_mesh.hpp"

namespace early_go {

class skinned_animation_mesh_allocator;

/*
 * A class that provides operations for a mesh file having animations.
 */
class skinned_animation_mesh : public base_mesh {
public:
  skinned_animation_mesh(const std::shared_ptr<::IDirect3DDevice9>&,
                         const std::string&,
                         const ::D3DXVECTOR3&,
                         const float&);
  bool get_play_animation() const; // not used
  void set_play_animation(const bool&); // not used
  float get_animation_time() const; // not used

private:
  void do_render(const ::D3DXMATRIX&, const ::D3DXMATRIX&) override;
  /*
   * A struct which deletes the root '::LPD3DXFRAME' object. The reason why the
   * following struct is not a prototype declaration is necessary for the
   * declaration of the following variable using std::unique_ptr.
   */
  struct frame_root_deleter_object
  {
    std::shared_ptr<skinned_animation_mesh_allocator>
        sp_skinned_animation_mesh_allocator_;

    void operator()(const ::LPD3DXFRAME);
    void release_mesh_allocator(const ::LPD3DXFRAME);
  };

  const static std::string                   SHADER_FILENAME;
  bool                                       b_play_animation_;
  float                                      f_animation_time_;
  std::shared_ptr<::IDirect3DDevice9>        sp_direct3d_device9_;
  std::shared_ptr<skinned_animation_mesh_allocator> 
      sp_skinned_animation_mesh_allocator_;
  std::unique_ptr<::D3DXFRAME, frame_root_deleter_object>
      up_d3dx_frame_root_;
  ::D3DXVECTOR3                              vec3_position_;
  ::D3DXMATRIX                            mat_rotation_;
  std::vector<::D3DXMATRIX>               vecmat_world_matrix_array_;

  ::D3DXVECTOR3 vec3_center_coodinate_{};
  float         f_scale_{};

  // For effect.
  ::D3DXHANDLE                                   d3dx_handle_view_projection_;
  ::D3DXHANDLE                                   d3dx_handle_scale_;

  void update_frame_matrix(const ::LPD3DXFRAME, const ::LPD3DXMATRIX);
  void render_frame(const ::LPD3DXFRAME);
  void render_mesh_container(const ::LPD3DXMESHCONTAINER);

  ::HRESULT allocate_bone_matrix(::LPD3DXMESHCONTAINER);
  ::HRESULT allocate_all_bone_matrices(::LPD3DXFRAME);
};
} /* namespace early_go */
#endif