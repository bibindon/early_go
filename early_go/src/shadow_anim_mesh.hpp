#pragma once
#include "stdafx.hpp"
#include "z_tex.hpp"
#include "depth_buf_shadow.hpp"

namespace early_go
{
class shadow_anim_mesh_allocator;
class animation_strategy;

// A class that provides operations for a mesh file having animations.
class shadow_anim_mesh
{
public:
    shadow_anim_mesh(
        const std::shared_ptr<IDirect3DDevice9> &,
        z_tex* ZTexCreator,
        depth_buf_shadow* DepthBS,
        const std::string &,
        const D3DXVECTOR3 &,
        const D3DXVECTOR3 &,
        const float &);
    ~shadow_anim_mesh();

    void update();
    void ztex_render();
    void render(const D3DXMATRIX &, const D3DXMATRIX &);
private:

    // A struct which deletes the root 'LPD3DXFRAME' object. The reason why the
    // following struct is not a prototype declaration is necessary for the
    // declaration of the following variable using std::unique_ptr.
    struct frame_root_deleter_object
    {
        std::shared_ptr<shadow_anim_mesh_allocator> allocator_;

        void operator()(const LPD3DXFRAME);
        void release_mesh_allocator(const LPD3DXFRAME);
    };

    const static std::string SHADER_FILENAME;
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::vector<std::shared_ptr<IDirect3DTexture9>> textures_ { };
    std::shared_ptr<shadow_anim_mesh_allocator> allocator_;
    std::unique_ptr<D3DXFRAME, frame_root_deleter_object> frame_root_;
    D3DXMATRIX rotation_matrix_;
    D3DXMATRIX view_matrix_;
    D3DXMATRIX projection_matrix_;

    D3DXVECTOR3 center_coodinate_;

    // For effect.
    D3DXHANDLE world_handle_;
    D3DXHANDLE world_view_proj_handle_;

    void update_frame_matrix(const LPD3DXFRAME, const LPD3DXMATRIX);
    void render_frame(const LPD3DXFRAME, const bool ztex);
    void render_mesh_container(const LPD3DXMESHCONTAINER, const LPD3DXFRAME, const bool ztex);

    DWORD materials_count_ { 0 };
//    std::unique_ptr<ID3DXEffect, custom_deleter> effect_;
    z_tex* ZTexCreator_ { nullptr };
    depth_buf_shadow* DepthBS_ { nullptr };
    std::shared_ptr<ID3DXMesh> d3dx_mesh_ { };
    std::unique_ptr<animation_strategy> animation_strategy_;

    D3DXVECTOR3 position_;
    D3DXVECTOR3 rotation_;
    float scale_;

    D3DXHANDLE mesh_texture_handle_ { nullptr };
    D3DXHANDLE diffuse_handle_ { nullptr };
};
} // namespace early_go 

