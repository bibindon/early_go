#pragma once
#include "stdafx.hpp"
#include "z_tex.hpp"
#include "depth_buf_shadow.hpp"
#include "dynamic_tex.hpp"

namespace early_go
{
class skinned_animation_mesh_allocator;
class animation_strategy;

// A class that provides operations for a mesh file having animations.
class shadow_mesh_skin
{
public:
    shadow_mesh_skin(
        const std::shared_ptr<IDirect3DDevice9> &,
        const std::shared_ptr<z_tex>& _ztex,
        const std::shared_ptr<depth_buf_shadow>& depth_buf_shadow,
        const std::string &,
        const D3DXVECTOR3 &,
        const D3DXVECTOR3 &,
        const float);
    HRESULT allocate_bone_matrix(LPD3DXMESHCONTAINER mesh_container);
    HRESULT allocate_all_bone_matrices(LPD3DXFRAME frame);

    void update();
    void ztex_render();
    void render();
    void set_position(const D3DXVECTOR3& pos);
    void set_rotate(const D3DXVECTOR3& rotate);
    void set_scale(const float scale);
    std::string x_filename();

    void set_animation(const std::string &);
    void set_default_animation(const std::string &);
    void set_animation_config(const std::string &, const bool &, const float &);

    std::shared_ptr<dynamic_tex> get_dynamic_tex() { return dynamic_tex_; }

private:

    // A struct which deletes the root 'LPD3DXFRAME' object. The reason why the
    // following struct is not a prototype declaration is necessary for the
    // declaration of the following variable using std::unique_ptr.
    struct frame_root_deleter_object
    {
        std::shared_ptr<skinned_animation_mesh_allocator> allocator_;

        void operator()(const LPD3DXFRAME);
        void release_mesh_allocator(const LPD3DXFRAME);
    };

    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::shared_ptr<skinned_animation_mesh_allocator> allocator_;
    std::unique_ptr<D3DXFRAME, frame_root_deleter_object> frame_root_;

    D3DXVECTOR3 center_coodinate_;

    void update_frame_matrix(const LPD3DXFRAME, const LPD3DXMATRIX);
    void render_frame(const LPD3DXFRAME, const bool ztex);
    void render_mesh_container_ztex(const LPD3DXMESHCONTAINER);
    void render_mesh_container(const LPD3DXMESHCONTAINER);

    std::shared_ptr<z_tex> ztex_ { nullptr };
    std::shared_ptr<depth_buf_shadow> depth_buf_shadow_ { nullptr };
    std::unique_ptr<animation_strategy> animation_strategy_;

    D3DXVECTOR3 pos_;
    D3DXVECTOR3 rotate_;
    float scale_;

    std::vector<D3DXMATRIX> world_matrix_array_;
    int bone_num_;

    std::string x_filename_ { "" };
    std::shared_ptr<dynamic_tex> dynamic_tex_;
};
} // namespace early_go 

