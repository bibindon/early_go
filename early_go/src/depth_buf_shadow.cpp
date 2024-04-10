#include "depth_buf_shadow.hpp"

#include "stdafx.hpp"
#include "EffectResource.h"
#include <tchar.h>

using std::vector;

namespace early_go
{

depth_buf_shadow::depth_buf_shadow()
{
}
depth_buf_shadow::~depth_buf_shadow()
{
}
bool depth_buf_shadow::init(Com_ptr<IDirect3DDevice9> &cpDev)
{
    if (cpDev == NULL)
    {
        return false;
    }

    vector<char> buffer{ early_go::util::get_shader_resource("depth_buf_shadow.fx") };
    if (FAILED ( D3DXCreateEffect(
        cpDev.GetPtr(),
        &buffer[0],
        static_cast<UINT>(buffer.size()),
        NULL,
        NULL,
        0,
        NULL,
        m_cpEffect.ToCreator(),
        NULL)))
    {
        return false;
    }

    m_hWorldMat = m_cpEffect->GetParameterByName(NULL, "matWorld");
    m_hCameraViewMat = m_cpEffect->GetParameterByName(NULL, "matCameraView");
    m_hCameraProjMat = m_cpEffect->GetParameterByName(NULL, "matCameraProj");
    m_hLightViewMat = m_cpEffect->GetParameterByName(NULL, "matLightView");
    m_hLightProjMat = m_cpEffect->GetParameterByName(NULL, "matLightProj");
    m_hShadowMapTex = m_cpEffect->GetParameterByName(NULL, "texShadowMap");
    m_hTechnique = m_cpEffect->GetTechniqueByName("DepthBufShadowTec");

    if (!m_hWorldMat || !m_hCameraViewMat || !m_hCameraProjMat || !m_hLightViewMat ||
        !m_hLightProjMat || !m_hShadowMapTex || !m_hTechnique)
    {
        return false;
    }

    m_cpDev = cpDev;
    return true;
}
bool depth_buf_shadow::set_shadow_map(Com_ptr<IDirect3DTexture9> &cpShadowMap)
{
    if (cpShadowMap == NULL)
    {
        return false;
    }

    m_cpShadowMapTex = cpShadowMap;
    return true;
}
void depth_buf_shadow::set_world_matrix(D3DXMATRIX *pMat)
{
    m_matWorld = *pMat;
}
void depth_buf_shadow::set_camera_view_matrix(D3DXMATRIX *pMat)
{
    m_matCameraView = *pMat;
}
void depth_buf_shadow::set_camera_proj_matrix(D3DXMATRIX *pMat)
{
    m_matCameraProj = *pMat;
}
void depth_buf_shadow::set_light_view_matrix(D3DXMATRIX *pMat)
{
    m_matLightView = *pMat;
}
void depth_buf_shadow::set_light_proj_matrix(D3DXMATRIX *pMat)
{
    m_matLightProj = *pMat;
}
HRESULT depth_buf_shadow::begin()
{
    // 各サーフェイスを初期化
    m_cpDev->Clear(
        0,
        NULL,
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        D3DCOLOR_ARGB(255, 255, 255, 255),
        1.0f,
        0);

    // プログラマブルシェーダに切り替え
    m_cpEffect->SetTechnique(m_hTechnique);

    // 開始宣言
    UINT Pass;
    m_cpEffect->Begin(&Pass, 0);

    return S_OK;
}
HRESULT depth_buf_shadow::end()
{
    m_cpEffect->End();

    // 固定機能に戻す
    m_cpDev->SetVertexShader(NULL);
    m_cpDev->SetPixelShader(NULL);

    return S_OK;
}
HRESULT depth_buf_shadow::begin_pass()
{
    m_cpEffect->BeginPass(0); // パスは1つのみ
    return S_OK;
}
HRESULT depth_buf_shadow::end_pass()
{
    m_cpEffect->EndPass();
    return S_OK;
}
bool depth_buf_shadow::set_param_to_effect()
{
    m_cpEffect->SetMatrix(m_hWorldMat, &m_matWorld);
    m_cpEffect->SetMatrix(m_hCameraViewMat, &m_matCameraView);
    m_cpEffect->SetMatrix(m_hCameraProjMat, &m_matCameraProj);
    m_cpEffect->SetMatrix(m_hLightViewMat, &m_matLightView);
    m_cpEffect->SetMatrix(m_hLightProjMat, &m_matLightProj);
    HRESULT hr = m_cpEffect->SetTexture(m_hShadowMapTex, m_cpShadowMapTex.GetPtr());

    return true;
}
}

