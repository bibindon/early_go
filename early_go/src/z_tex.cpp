#include "z_tex.hpp"

#include "stdafx.hpp"

using std::vector;

namespace early_go
{
z_tex::z_tex()
{
    D3DXMatrixIdentity(&m_matWorld);
    D3DXMatrixIdentity(&m_matView);
    D3DXMatrixIdentity(&m_matProj);
    m_hWorldMat = NULL;
    m_hViewMat = NULL;
    m_hProjMat = NULL;
    m_hTechnique = NULL;
}
z_tex::~z_tex()
{
}
#pragma warning(push)
#pragma warning(disable: 26812 )
bool z_tex::init(
    const std::shared_ptr<IDirect3DDevice9>& cpDev,
    UINT ZTexWidth,
    UINT ZTexHeight,
    D3DFORMAT ZTexFormat) // warning 26812
#pragma warning(pop)
{
    if (cpDev.get() == NULL)
    {
        return false;
    }
    if (ZTexWidth == 0 || ZTexHeight == 0)
    {
        return false;
    }

    HRESULT hr;

    // リソースにあるZ値プロットシェーダプログラムを読み込む
    vector<char> buffer { early_go::util::get_shader_resource("z_tex.fx") };
    LPD3DXEFFECT effect{nullptr};
    if (FAILED(D3DXCreateEffect(
        cpDev.get(),
        &buffer[0],
        static_cast<UINT>(buffer.size()),
        NULL,
        NULL,
        0,
        NULL,
        &effect,
        NULL)))
    {
        return false;
    }
    m_cpEffect.reset(effect, custom_deleter { });

    // エフェクト内の各種パラメータハンドルを取得
    m_hWorldMat = m_cpEffect->GetParameterByName(NULL, "matWorld");
    m_hViewMat = m_cpEffect->GetParameterByName(NULL, "matView");
    m_hProjMat = m_cpEffect->GetParameterByName(NULL, "matProj");
    m_hTechnique = m_cpEffect->GetTechniqueByName("ZValuePlotTec");
    if (!m_hWorldMat || !m_hViewMat || !m_hProjMat || !m_hTechnique)
    {
        return false;
    }

    // 指定のZ値テクスチャを生成
    LPDIRECT3DTEXTURE9 temp_texture{nullptr};
    hr = D3DXCreateTexture(
        cpDev.get(),
        ZTexWidth, ZTexHeight,
        1,
        D3DUSAGE_RENDERTARGET,
        ZTexFormat,
        D3DPOOL_DEFAULT,
        &temp_texture);
    if (FAILED(hr))
    {
        return false;
    }
    m_cpZTex.reset(temp_texture, custom_deleter { });

    LPDIRECT3DSURFACE9 temp_surface;
    m_cpZTex->GetSurfaceLevel(0, &temp_surface); // サーフェイス取得
    m_cpZTexSurf.reset(temp_surface, custom_deleter { });

    // 描画デバイスに定義されているバッファの能力を取得
    IDirect3DSurface9* pSurf;
    cpDev->GetDepthStencilSurface(&pSurf);
    D3DSURFACE_DESC Desc;
    pSurf->GetDesc(&Desc);
    pSurf->Release();

    // 独自深度バッファを作成
    LPDIRECT3DSURFACE9 temp_surface2;
    hr = cpDev->CreateDepthStencilSurface(
        ZTexWidth, ZTexHeight,
        Desc.Format,
        Desc.MultiSampleType,
        Desc.MultiSampleQuality,
        FALSE,
        &temp_surface2,
        NULL);
    if (FAILED(hr))
    {
        return false;
    }
    m_cpDepthBuff.reset(temp_surface2, custom_deleter { });

    m_cpDev = cpDev;

    return true;
}
void z_tex::set_world_matrix(D3DXMATRIX* pMat)
{
    m_matWorld = *pMat;
}
void z_tex::set_view_matrix(D3DXMATRIX* pMat)
{
    m_matView = *pMat;
}
void z_tex::set_proj_matrix(D3DXMATRIX* pMat)
{
    m_matProj = *pMat;
}
HRESULT z_tex::begin()
{
    // 初期化が正しく行われているかチェック
    if (m_cpDepthBuff.get() == NULL || m_cpZTex.get() == NULL)
    {
        return E_FAIL;
    }

    // デバイスが持っているバッファを一時保存
    LPDIRECT3DSURFACE9 temp_surface1;
    m_cpDev->GetRenderTarget(0, &temp_surface1);
    m_cpDevBuffer.reset(temp_surface1, custom_deleter { });

    LPDIRECT3DSURFACE9 temp_surface2;
    m_cpDev->GetDepthStencilSurface(&temp_surface2);
    m_cpDevDepth.reset(temp_surface2, custom_deleter { });

    // デバイスにZ値テクスチャサーフェイスと深度バッファを設定
    m_cpDev->SetRenderTarget(0, m_cpZTexSurf.get());
    m_cpDev->SetDepthStencilSurface(m_cpDepthBuff.get());

    // 各サーフェイスを初期化
    m_cpDev->Clear(
        0,
        NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        D3DCOLOR_ARGB(255, 255, 255, 255),
        1.0f,
        0);

    // プログラマブルシェーダのテクニックを設定
    m_cpEffect->SetTechnique(m_hTechnique);

    // シェーダの開始を宣言
    UINT Tmp;
    m_cpEffect->Begin(&Tmp, 0);

    return S_OK;
}
HRESULT z_tex::begin_pass()
{
    m_cpEffect->BeginPass(0); // Z値計算は1パス
    return S_OK;
}
bool z_tex::set_param_to_effect()
{
    if (m_cpEffect.get() == NULL)
    {
        return false;
    }

    m_cpEffect->SetMatrix(m_hWorldMat, &m_matWorld);
    m_cpEffect->SetMatrix(m_hViewMat, &m_matView);
    m_cpEffect->SetMatrix(m_hProjMat, &m_matProj);

    return true;
}
HRESULT z_tex::end_pass()
{
    m_cpEffect->EndPass();
    return S_OK;
}
HRESULT z_tex::end()
{
    m_cpEffect->End();

    // デバイスに元のサーフェイスを戻す
    m_cpDev->SetRenderTarget(0, m_cpDevBuffer.get());
    m_cpDev->SetDepthStencilSurface(m_cpDevDepth.get());

    m_cpDevBuffer = NULL;
    m_cpDevDepth = NULL;

    // 固定機能に戻す
    m_cpDev->SetVertexShader(NULL);
    m_cpDev->SetPixelShader(NULL);

    return S_OK;
}
bool z_tex::get_z_tex(std::shared_ptr<IDirect3DTexture9>& cpTex)
{
    cpTex = m_cpZTex;
    return true;
}
}

