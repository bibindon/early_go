// http://marupeke296.com/DXGSmp_No46_DepthBufShadowImp.html

#pragma once

#include "ComPtr.h"
#include <d3dx9.h>

namespace IKD
{
class depth_buf_shadow
{
public:
    depth_buf_shadow();
    ~depth_buf_shadow();
    bool init(Com_ptr<IDirect3DDevice9> &cpDev);
    bool set_shadow_map(Com_ptr<IDirect3DTexture9> &cpShadowMap);
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
    Com_ptr<IDirect3DDevice9> m_cpDev;
    Com_ptr<IDirect3DTexture9> m_cpShadowMapTex;
    Com_ptr<ID3DXEffect> m_cpEffect;
    D3DXMATRIX m_matWorld;
    D3DXMATRIX m_matCameraView;
    D3DXMATRIX m_matCameraProj;
    D3DXMATRIX m_matLightView;
    D3DXMATRIX m_matLightProj;
    D3DXHANDLE m_hWorldMat;
    D3DXHANDLE m_hCameraViewMat;
    D3DXHANDLE m_hCameraProjMat;
    D3DXHANDLE m_hLightViewMat;
    D3DXHANDLE m_hLightProjMat;
    D3DXHANDLE m_hShadowMapTex;
    D3DXHANDLE m_hTechnique;
};
}
