// http://marupeke296.com/DXGSmp_No46_DepthBufShadowImp.html

#pragma once

#include <d3dx9.h>
#include "ComPtr.h"
#include "EffectResource.h"

namespace early_go
{
class z_tex
{
public:
    z_tex();
    virtual ~z_tex();
    bool init(
        Com_ptr<IDirect3DDevice9> &cpDev,
        UINT ZTexWidth,
        UINT ZTexHeight,
        D3DFORMAT ZTexFormat = D3DFMT_A8R8G8B8);
    void set_world_matrix(D3DXMATRIX *pMat);
    void set_view_matrix(D3DXMATRIX *pMat);
    void set_proj_matrix(D3DXMATRIX *pMat);
    HRESULT begin();
    HRESULT end();
    HRESULT begin_pass();
    HRESULT end_pass();
    bool set_param_to_effect();
    bool get_z_tex(Com_ptr<IDirect3DTexture9> &cpTex);

private:
    Com_ptr<IDirect3DDevice9> m_cpDev;
    Com_ptr<IDirect3DTexture9> m_cpZTex;
    Com_ptr<IDirect3DSurface9> m_cpZTexSurf;
    Com_ptr<IDirect3DSurface9> m_cpDepthBuff;
    Com_ptr<IDirect3DSurface9> m_cpDevBuffer;
    Com_ptr<IDirect3DSurface9> m_cpDevDepth;
    Com_ptr<ID3DXEffect> m_cpEffect;
    D3DXMATRIX m_matWorld;
    D3DXMATRIX m_matView;
    D3DXMATRIX m_matProj;
    D3DXHANDLE m_hWorldMat;
    D3DXHANDLE m_hViewMat;
    D3DXHANDLE m_hProjMat;
    D3DXHANDLE m_hTechnique;
};
}
