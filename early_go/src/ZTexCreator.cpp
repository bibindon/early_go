// ZTexCreator.cpp (2007. 2. 28)

// Z値テクスチャ生成クラス実装部

#include "ZTexCreator.h"

using namespace IKD;

// コンストラクタ
CZTexCreator::CZTexCreator()
{
    D3DXMatrixIdentity(&m_matWorld);
    D3DXMatrixIdentity(&m_matView);
    D3DXMatrixIdentity(&m_matProj);
    m_hWorldMat = NULL;
    m_hViewMat = NULL;
    m_hProjMat = NULL;
    m_hTechnique = NULL;
}

// デストラクタ
CZTexCreator::~CZTexCreator()
{
    // COMポインタを明示的に消去
    m_cpDepthBuff = NULL; // 深度バッファ
    m_cpZTexSurf = NULL;  // Z値テクスチャサーフェイス
    m_cpZTex = NULL;      // Z値テクスチャ
    m_cpDevBuffer = NULL; // デバイスバックバッファ
    m_cpDevDepth = NULL;  // デバイス深度バッファ
    m_cpEffect = NULL;    // Z値プロットエフェクト
    m_cpDev = NULL;       // 描画デバイス
}

// 初期化メソッド
bool CZTexCreator::Init(Com_ptr<IDirect3DDevice9> &cpDev, UINT ZTexWidth, UINT ZTexHeight, D3DFORMAT ZTexFormat)
{
    if (cpDev.GetPtr() == NULL)
        return false;
    if (ZTexWidth == 0 || ZTexHeight == 0)
        return false;

    HRESULT hr;

    // リソースにあるZ値プロットシェーダプログラムを読み込む
    if (FAILED(D3DXCreateEffectFromResource(
            cpDev.GetPtr(),
            NULL,
            MAKEINTRESOURCE(FXID_ZVALUEPLOT),
            NULL,
            NULL,
            0,
            NULL,
            m_cpEffect.ToCreator(),
            NULL)))
        return false;

    // エフェクト内の各種パラメータハンドルを取得
    m_hWorldMat = m_cpEffect->GetParameterByName(NULL, "matWorld");
    m_hViewMat = m_cpEffect->GetParameterByName(NULL, "matView");
    m_hProjMat = m_cpEffect->GetParameterByName(NULL, "matProj");
    m_hTechnique = m_cpEffect->GetTechniqueByName("ZValuePlotTec");
    if (!m_hWorldMat || !m_hViewMat || !m_hProjMat || !m_hTechnique)
        return false;

    // 指定のZ値テクスチャを生成
    hr = D3DXCreateTexture(
        cpDev.GetPtr(),
        ZTexWidth, ZTexHeight,
        1,
        D3DUSAGE_RENDERTARGET,
        ZTexFormat,
        D3DPOOL_DEFAULT,
        m_cpZTex.ToCreator());
    if (FAILED(hr))
        return false;

    m_cpZTex->GetSurfaceLevel(0, m_cpZTexSurf.ToCreator()); // サーフェイス取得

    // 描画デバイスに定義されているバッファの能力を取得
    IDirect3DSurface9 *pSurf;
    cpDev->GetDepthStencilSurface(&pSurf);
    D3DSURFACE_DESC Desc;
    pSurf->GetDesc(&Desc);
    pSurf->Release();

    // 独自深度バッファを作成
    hr = cpDev->CreateDepthStencilSurface(
        ZTexWidth, ZTexHeight,
        Desc.Format,
        Desc.MultiSampleType,
        Desc.MultiSampleQuality,
        FALSE,
        m_cpDepthBuff.ToCreator(),
        NULL);
    if (FAILED(hr))
        return false;

    m_cpDev = cpDev;

    return true;
}

// 描画対象オブジェクトのワールド変換行列を設定
void CZTexCreator::SetWorldMatrix(D3DXMATRIX *pMat)
{
    m_matWorld = *pMat;
}

// ビュー行列を設定
void CZTexCreator::SetViewMatrix(D3DXMATRIX *pMat)
{
    m_matView = *pMat;
}

// 射影変換行列を設定
void CZTexCreator::SetProjMatrix(D3DXMATRIX *pMat)
{
    m_matProj = *pMat;
}

// 描画の開始を宣言する
HRESULT CZTexCreator::Begin()
{
    // 初期化が正しく行われているかチェック
    if (m_cpDepthBuff.GetPtr() == NULL || m_cpZTex.GetPtr() == NULL)
        return E_FAIL;

    // デバイスが持っているバッファを一時保存
    m_cpDev->GetRenderTarget(0, m_cpDevBuffer.ToCreator());
    m_cpDev->GetDepthStencilSurface(m_cpDevDepth.ToCreator());

    // デバイスにZ値テクスチャサーフェイスと深度バッファを設定
    m_cpDev->SetRenderTarget(0, m_cpZTexSurf.GetPtr());
    m_cpDev->SetDepthStencilSurface(m_cpDepthBuff.GetPtr());

    // 各サーフェイスを初期化
    m_cpDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 0);

    // プログラマブルシェーダのテクニックを設定
    m_cpEffect->SetTechnique(m_hTechnique);

    // シェーダの開始を宣言
    UINT Tmp;
    m_cpEffect->Begin(&Tmp, 0);

    return S_OK;
}

// パスの開始を宣言する
HRESULT CZTexCreator::BeginPass()
{
    m_cpEffect->BeginPass(0); // Z値計算は1パス
    return S_OK;
}

// 登録されているパラメータ情報をエフェクトにセット
bool CZTexCreator::SetParamToEffect()
{
    if (m_cpEffect.GetPtr() == NULL)
        return false;

    m_cpEffect->SetMatrix(m_hWorldMat, &m_matWorld);
    m_cpEffect->SetMatrix(m_hViewMat, &m_matView);
    m_cpEffect->SetMatrix(m_hProjMat, &m_matProj);

    return true;
}

// パスの終了を宣言する
HRESULT CZTexCreator::EndPass()
{
    m_cpEffect->EndPass();
    return S_OK;
}

// 描画の終了を宣言する
HRESULT CZTexCreator::End()
{
    m_cpEffect->End();

    // デバイスに元のサーフェイスを戻す
    m_cpDev->SetRenderTarget(0, m_cpDevBuffer.GetPtr());
    m_cpDev->SetDepthStencilSurface(m_cpDevDepth.GetPtr());

    m_cpDevBuffer = NULL;
    m_cpDevDepth = NULL;

    // 固定機能に戻す
    m_cpDev->SetVertexShader(NULL);
    m_cpDev->SetPixelShader(NULL);

    return S_OK;
}

// Z値テクスチャを取得する
bool CZTexCreator::GetZTex(Com_ptr<IDirect3DTexture9> &cpTex)
{
    cpTex = m_cpZTex;
    return true;
}
