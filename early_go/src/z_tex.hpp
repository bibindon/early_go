// http://marupeke296.com/DXGSmp_No46_DepthBufShadowImp.html

#pragma once

#include "stdafx.hpp"

namespace early_go
{
class z_tex
{
public:
    z_tex();
    virtual ~z_tex();
    bool init(
        const std::shared_ptr<IDirect3DDevice9> &d3dx_device,
        UINT ztex_width,
        UINT ztex_height,
        D3DFORMAT ZTexFormat = D3DFMT_A8R8G8B8);
    void set_world_matrix(D3DXMATRIX *mat);
    void set_view_matrix(D3DXMATRIX *mat);
    void set_proj_matrix(D3DXMATRIX *mat);
    void set_bone_num(const int bone_num);
    void set_skin_mat_array(const std::vector<D3DXMATRIX>& skin_mat_arr);
    HRESULT begin();
    HRESULT end();
    HRESULT begin_pass(const int pass = 0);
    HRESULT end_pass();
    bool set_param_to_effect();
    bool get_z_tex(std::shared_ptr<IDirect3DTexture9> &cpTex);

private:
    std::shared_ptr<IDirect3DDevice9> d3dx_device_;
    std::shared_ptr<IDirect3DTexture9> ztex_;
    std::shared_ptr<IDirect3DSurface9> ztex_surface_;
    std::shared_ptr<IDirect3DSurface9> depth_buffer_;
    std::shared_ptr<IDirect3DSurface9> origin_buffer_;
    std::shared_ptr<IDirect3DSurface9> origin_depth_;
    std::shared_ptr<ID3DXEffect> effect_;

    D3DXMATRIX mat_world_;
    D3DXMATRIX mat_view_;
    D3DXMATRIX mat_proj_;
    int bone_num_ { 0 };
    std::vector<D3DXMATRIX> skin_mat_arr_;

    D3DXHANDLE mat_world_handle_;
    D3DXHANDLE mat_view_handle_;
    D3DXHANDLE mat_proj_handle_;
    D3DXHANDLE technique_handle_;
};
}
