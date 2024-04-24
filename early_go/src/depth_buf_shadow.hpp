// http://marupeke296.com/DXGSmp_No46_DepthBufShadowImp.html

#pragma once

#include "stdafx.hpp"

namespace early_go
{
class depth_buf_shadow
{
public:
    depth_buf_shadow();
    bool init(std::shared_ptr<IDirect3DDevice9> &d3dx_device);
    void set_shadow_map(std::shared_ptr<IDirect3DTexture9> &shadow_map_tex);
    void set_tex(const std::shared_ptr<IDirect3DTexture9> &_tex);
    void set_dynamic_tex(const int layer, const std::shared_ptr<IDirect3DTexture9> &_tex);
    void set_dynamic_pos(const int layer, const D3DXVECTOR2& pos);
    void set_dynamic_opacity(const int layer, const float alpha);
    void set_world_matrix(D3DXMATRIX *mat);
    void set_camera_view_matrix(D3DXMATRIX *mat);
    void set_camera_proj_matrix(D3DXMATRIX *mat);
    void set_light_view_matrix(D3DXMATRIX *mat);
    void set_light_proj_matrix(D3DXMATRIX *mat);
    void set_bone_num(const int bone_num);
    void set_skin_mat_array(const std::vector<D3DXMATRIX>& skin_mat_arr);
    HRESULT begin();
    HRESULT end();
    HRESULT begin_pass(const int pass = 0);
    HRESULT end_pass();
    bool set_param_to_effect();

    static constexpr int LAYER { 8 };

private:
    std::shared_ptr<IDirect3DDevice9> d3dx_device_ { };
    std::shared_ptr<IDirect3DTexture9> shadow_map_tex_ { };
    std::shared_ptr<IDirect3DTexture9> mesh_tex_ { };
    std::vector<std::shared_ptr<IDirect3DTexture9> > textures_ { };
    std::array<D3DXVECTOR4, LAYER> tex_pos_ { };
    std::array<float, LAYER> tex_opacities_ { };
    std::shared_ptr<ID3DXEffect> effect_ { };
    D3DXMATRIX mat_world_ { };
    D3DXMATRIX camera_mat_view_ { };
    D3DXMATRIX camera_mat_proj_ { };
    D3DXMATRIX light_mat_view_ { };
    D3DXMATRIX light_mat_proj_ { };
    D3DXHANDLE mat_world_handle_ { nullptr };
    D3DXHANDLE camera_view_handle_ { nullptr };
    D3DXHANDLE camera_proj_handle_ { nullptr };
    D3DXHANDLE light_view_handle_ { nullptr };
    D3DXHANDLE light_proj_handle_ { nullptr };
    D3DXHANDLE shadow_map_handle_ { nullptr };
    D3DXHANDLE mesh_tex_handle_ { nullptr };

    std::array<D3DXHANDLE, LAYER> tex_handles_ { };

    D3DXHANDLE tex_pos_handles_ { nullptr };
    D3DXHANDLE tex_opacity_handles_ { nullptr };

    D3DXHANDLE technique_handle_ { nullptr };

    int bone_num_ { 0 };
    std::vector<D3DXMATRIX> skin_mat_arr_;
};
}
