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
        const std::shared_ptr<IDirect3DDevice9> &cpDev,
        UINT ZTexWidth,
        UINT ZTexHeight,
        D3DFORMAT ZTexFormat = D3DFMT_A8R8G8B8);
    void set_world_matrix(D3DXMATRIX *pMat);
    void set_view_matrix(D3DXMATRIX *pMat);
    void set_proj_matrix(D3DXMATRIX *pMat);
    void set_bone_num(const int bone_num);
    void set_skin_mat_array(const std::vector<D3DXMATRIX>& skin_mat_arr);
    HRESULT begin();
    HRESULT end();
    HRESULT begin_pass(const int pass = 0);
    HRESULT end_pass();
    bool set_param_to_effect();
    bool get_z_tex(std::shared_ptr<IDirect3DTexture9> &cpTex);

private:
    std::shared_ptr<IDirect3DDevice9> m_cpDev;
    std::shared_ptr<IDirect3DTexture9> m_cpZTex;
    std::shared_ptr<IDirect3DSurface9> m_cpZTexSurf;
    std::shared_ptr<IDirect3DSurface9> m_cpDepthBuff;
    std::shared_ptr<IDirect3DSurface9> m_cpDevBuffer;
    std::shared_ptr<IDirect3DSurface9> m_cpDevDepth;
    std::shared_ptr<ID3DXEffect> m_cpEffect;

    D3DXMATRIX m_matWorld;
    D3DXMATRIX m_matView;
    D3DXMATRIX m_matProj;
    int bone_num_ { 0 };
    std::vector<D3DXMATRIX> skin_mat_arr_;

    D3DXHANDLE m_hWorldMat;
    D3DXHANDLE m_hViewMat;
    D3DXHANDLE m_hProjMat;
    D3DXHANDLE m_hTechnique;
};
}
