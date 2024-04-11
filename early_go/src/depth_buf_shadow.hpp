// http://marupeke296.com/DXGSmp_No46_DepthBufShadowImp.html

#pragma once

#include "stdafx.hpp"

namespace early_go
{
class depth_buf_shadow
{
public:
    depth_buf_shadow();
    ~depth_buf_shadow();
    bool init(std::shared_ptr<IDirect3DDevice9> &cpDev);
    bool set_shadow_map(std::shared_ptr<IDirect3DTexture9> &cpShadowMap);
    bool set_tex(const std::shared_ptr<IDirect3DTexture9> &_tex);
    void set_world_matrix(D3DXMATRIX *pMat);
    void set_camera_view_matrix(D3DXMATRIX *pMat);
    void set_camera_proj_matrix(D3DXMATRIX *pMat);
    void set_light_view_matrix(D3DXMATRIX *pMat);
    void set_light_proj_matrix(D3DXMATRIX *pMat);
    HRESULT begin();
    HRESULT end();
    HRESULT begin_pass();
    HRESULT end_pass();
    bool set_param_to_effect();

private:
    std::shared_ptr<IDirect3DDevice9> m_cpDev;
    std::shared_ptr<IDirect3DTexture9> m_cpShadowMapTex;
    std::shared_ptr<IDirect3DTexture9> g_mesh_tex_;
    std::shared_ptr<ID3DXEffect> m_cpEffect;
    D3DXMATRIX m_matWorld { };
    D3DXMATRIX m_matCameraView { };
    D3DXMATRIX m_matCameraProj { };
    D3DXMATRIX m_matLightView { };
    D3DXMATRIX m_matLightProj { };
    D3DXHANDLE m_hWorldMat { nullptr };
    D3DXHANDLE m_hCameraViewMat { nullptr };
    D3DXHANDLE m_hCameraProjMat { nullptr };
    D3DXHANDLE m_hLightViewMat { nullptr };
    D3DXHANDLE m_hLightProjMat { nullptr };
    D3DXHANDLE m_hShadowMapTex { nullptr };
    D3DXHANDLE g_mesh_tex_handle_ { nullptr };
    D3DXHANDLE m_hTechnique { nullptr };
};
}
