// ZTexCreator.h (2007. 2. 28)

// Z値テクスチャ生成クラス宣言部
//  生成者 ： IKD

#pragma once

#include <d3dx9.h>
#include "ComPtr.h"
#include "EffectResource.h"

namespace IKD
{

    ///////////////////////////////////////////////
    // z_texクラス
    //
    //   Z値テクスチャを作成するヘルパークラス
    //////////////////////////////
    class z_tex
    {
    protected:
        Com_ptr<IDirect3DDevice9> m_cpDev;        // 描画デバイス
        Com_ptr<IDirect3DTexture9> m_cpZTex;      // Z値テクスチャ
        Com_ptr<IDirect3DSurface9> m_cpZTexSurf;  // Z値テクスチャサーフェイス
        Com_ptr<IDirect3DSurface9> m_cpDepthBuff; // 深度バッファ
        Com_ptr<IDirect3DSurface9> m_cpDevBuffer; // デバイスバックバッファ
        Com_ptr<IDirect3DSurface9> m_cpDevDepth;  // デバイス深度バッファ
        Com_ptr<ID3DXEffect> m_cpEffect;          // Z値プロットエフェクト
        D3DXMATRIX m_matWorld;                    // ワールド変換行列
        D3DXMATRIX m_matView;                     // ビュー変換行列
        D3DXMATRIX m_matProj;                     // 射影変換行列
        D3DXHANDLE m_hWorldMat;                   // ワールド変換行列ハンドル
        D3DXHANDLE m_hViewMat;                    // ビュー変換行列ハンドル
        D3DXHANDLE m_hProjMat;                    // 射影変換行列ハンドル
        D3DXHANDLE m_hTechnique;                  // テクニックへのハンドル

    public:
        z_tex();
        virtual ~z_tex();

    public:
        // 初期化メソッド
        bool Init(Com_ptr<IDirect3DDevice9> &cpDev, UINT ZTexWidth, UINT ZTexHeight, D3DFORMAT ZTexFormat = D3DFMT_A8R8G8B8);

        // 描画対象オブジェクトのワールド変換行列を設定
        void SetWorldMatrix(D3DXMATRIX *pMat);

        // ビュー行列を設定
        void SetViewMatrix(D3DXMATRIX *pMat);

        // 射影変換行列を設定
        void SetProjMatrix(D3DXMATRIX *pMat);

        // 描画の開始を宣言する
        HRESULT Begin();

        // 描画の終了を宣言する
        HRESULT End();

        // パスの開始を宣言する
        HRESULT BeginPass();

        // パスの終了を宣言する
        HRESULT EndPass();

        // 登録されているパラメータ情報をエフェクトにセット
        bool SetParamToEffect();

        // Z値テクスチャを取得する
        bool GetZTex(Com_ptr<IDirect3DTexture9> &cpTex);
    };

}