#pragma once
#include "stdafx.hpp"
#include "z_tex.hpp"
#include "depth_buf_shadow.hpp"

namespace early_go
{
class animation_mesh_allocator;
class animation_strategy;

// A class that provides operations for a mesh file having animations.
class shadow_mesh_anim
{
public:
    shadow_mesh_anim(
        const std::shared_ptr<IDirect3DDevice9> &,
        const std::shared_ptr<z_tex>& _ztex,
        const std::shared_ptr<depth_buf_shadow>& depth_buf_shadow,
        const std::string &,
        const D3DXVECTOR3 &,
        const D3DXVECTOR3 &,
        const float);

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
private:

    // A struct which deletes the root 'LPD3DXFRAME' object. The reason why the
    // following struct is not a prototype declaration is necessary for the
    // declaration of the following variable using std::unique_ptr.
    struct frame_root_deleter_object
    {
        std::shared_ptr<animation_mesh_allocator> allocator_;

        void operator()(const LPD3DXFRAME);
        void release_mesh_allocator(const LPD3DXFRAME);
    };

    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::vector<std::shared_ptr<IDirect3DTexture9>> textures_ { };
    std::shared_ptr<animation_mesh_allocator> allocator_;
    std::unique_ptr<D3DXFRAME, frame_root_deleter_object> frame_root_;

    D3DXVECTOR3 center_coodinate_;

    void update_frame_matrix(const LPD3DXFRAME, const LPD3DXMATRIX, const bool ztex);
    void render_frame(const LPD3DXFRAME, const bool ztex);
    void render_mesh_container_ztex(const LPD3DXMESHCONTAINER, const LPD3DXFRAME);
    void render_mesh_container(const LPD3DXMESHCONTAINER, const LPD3DXFRAME);

    std::shared_ptr<z_tex> ztex_ { nullptr };
    std::shared_ptr<depth_buf_shadow> depth_buf_shadow_ { nullptr };
    std::unique_ptr<animation_strategy> animation_strategy_;

    D3DXVECTOR3 pos_ { 0.0f, 0.0f, 0.0f };
    D3DXVECTOR3 rotate_ { 0.0f, 0.0f, 0.0f };
    float scale_ { 1.0f };
    std::string x_filename_ { "" };
};
} // namespace early_go 

