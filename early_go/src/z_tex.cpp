#include "z_tex.hpp"

#include "stdafx.hpp"

using std::vector;

namespace early_go
{
z_tex::z_tex()
{
    D3DXMatrixIdentity(&mat_world_);
    D3DXMatrixIdentity(&mat_view_);
    D3DXMatrixIdentity(&mat_proj_);
    mat_world_handle_ = NULL;
    mat_view_handle_ = NULL;
    mat_proj_handle_ = NULL;
    technique_handle_ = NULL;
}
z_tex::~z_tex()
{
}
#pragma warning(push)
#pragma warning(disable: 26812 )
bool z_tex::init(
    const std::shared_ptr<IDirect3DDevice9>& d3dx_device,
    UINT ztex_width,
    UINT ztex_height,
    D3DFORMAT ZTexFormat) // warning 26812
#pragma warning(pop)
{
    if (d3dx_device.get() == NULL)
    {
        return false;
    }
    if (ztex_width == 0 || ztex_height == 0)
    {
        return false;
    }

    HRESULT hr;

    // リソースにあるZ値プロットシェーダプログラムを読み込む
    vector<char> buffer { early_go::util::get_shader_resource("z_tex.fx") };
    LPD3DXEFFECT effect{nullptr};
    if (FAILED(D3DXCreateEffect(
        d3dx_device.get(),
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
    effect_.reset(effect, custom_deleter { });

    // エフェクト内の各種パラメータハンドルを取得
    mat_world_handle_ = effect_->GetParameterByName(NULL, "matWorld");
    mat_view_handle_ = effect_->GetParameterByName(NULL, "matView");
    mat_proj_handle_ = effect_->GetParameterByName(NULL, "matProj");
    technique_handle_ = effect_->GetTechniqueByName("ZValuePlotTec");
    if (!mat_world_handle_ || !mat_view_handle_ || !mat_proj_handle_ || !technique_handle_)
    {
        return false;
    }

    // 指定のZ値テクスチャを生成
    LPDIRECT3DTEXTURE9 temp_texture{nullptr};
    hr = D3DXCreateTexture(
        d3dx_device.get(),
        ztex_width, ztex_height,
        1,
        D3DUSAGE_RENDERTARGET,
        ZTexFormat,
        D3DPOOL_DEFAULT,
        &temp_texture);
    if (FAILED(hr))
    {
        return false;
    }
    ztex_.reset(temp_texture, custom_deleter { });

    LPDIRECT3DSURFACE9 temp_surface;
    ztex_->GetSurfaceLevel(0, &temp_surface); // サーフェイス取得
    ztex_surface_.reset(temp_surface, custom_deleter { });

    // 描画デバイスに定義されているバッファの能力を取得
    IDirect3DSurface9* pSurf;
    d3dx_device->GetDepthStencilSurface(&pSurf);
    D3DSURFACE_DESC Desc;
    pSurf->GetDesc(&Desc);
    pSurf->Release();

    // 独自深度バッファを作成
    LPDIRECT3DSURFACE9 temp_surface2;
    hr = d3dx_device->CreateDepthStencilSurface(
        ztex_width, ztex_height,
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
    depth_buffer_.reset(temp_surface2, custom_deleter { });

    d3dx_device_ = d3dx_device;

    return true;
}
void z_tex::set_world_matrix(D3DXMATRIX* mat)
{
    mat_world_ = *mat;
}
void z_tex::set_view_matrix(D3DXMATRIX* mat)
{
    mat_view_ = *mat;
}
void z_tex::set_proj_matrix(D3DXMATRIX* mat)
{
    mat_proj_ = *mat;
}
void z_tex::set_bone_num(const int bone_num)
{
    bone_num_ = bone_num;
}
void z_tex::set_skin_mat_array(const std::vector<D3DXMATRIX>& skin_mat_arr)
{
    skin_mat_arr_ = std::vector<D3DXMATRIX>(skin_mat_arr);
}
HRESULT z_tex::begin()
{
    // 初期化が正しく行われているかチェック
    if (depth_buffer_.get() == NULL || ztex_.get() == NULL)
    {
        return E_FAIL;
    }

    // デバイスが持っているバッファを一時保存
    LPDIRECT3DSURFACE9 temp_surface1;
    d3dx_device_->GetRenderTarget(0, &temp_surface1);
    origin_buffer_.reset(temp_surface1, custom_deleter { });

    LPDIRECT3DSURFACE9 temp_surface2;
    d3dx_device_->GetDepthStencilSurface(&temp_surface2);
    origin_depth_.reset(temp_surface2, custom_deleter { });

    // デバイスにZ値テクスチャサーフェイスと深度バッファを設定
    d3dx_device_->SetRenderTarget(0, ztex_surface_.get());
    d3dx_device_->SetDepthStencilSurface(depth_buffer_.get());

    // 各サーフェイスを初期化
    d3dx_device_->Clear(
        0,
        NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        D3DCOLOR_ARGB(255, 255, 255, 255),
        1.0f,
        0);

    // プログラマブルシェーダのテクニックを設定
    effect_->SetTechnique(technique_handle_);

    // シェーダの開始を宣言
    UINT Tmp;
    effect_->Begin(&Tmp, 0);

    return S_OK;
}
HRESULT z_tex::begin_pass(const int pass)
{
    effect_->BeginPass(pass); // Z値計算は1パス
    return S_OK;
}
bool z_tex::set_param_to_effect()
{
    if (effect_.get() == NULL)
    {
        return false;
    }

    effect_->SetMatrix(mat_world_handle_, &mat_world_);
    effect_->SetMatrix(mat_view_handle_, &mat_view_);
    effect_->SetMatrix(mat_proj_handle_, &mat_proj_);
    HRESULT result { 0 };
    result = effect_->SetInt("current_bone_numbers", bone_num_);
    if (!skin_mat_arr_.empty())
    {
        if (skin_mat_arr_.size() >= 26)
        {
            result = effect_->SetMatrixArray("g_world_matrix_array", &skin_mat_arr_[0], 26);
        }
        else
        {
            result = effect_->SetMatrixArray(
                "g_world_matrix_array",
                &skin_mat_arr_[0],
                static_cast<UINT>(skin_mat_arr_.size()));
        }
    }
    effect_->CommitChanges();

    return true;
}
HRESULT z_tex::end_pass()
{
    effect_->EndPass();
    return S_OK;
}
HRESULT z_tex::end()
{
    effect_->End();

    // デバイスに元のサーフェイスを戻す
    d3dx_device_->SetRenderTarget(0, origin_buffer_.get());
    d3dx_device_->SetDepthStencilSurface(origin_depth_.get());

    origin_buffer_ = NULL;
    origin_depth_ = NULL;

    // 固定機能に戻す
    d3dx_device_->SetVertexShader(NULL);
    d3dx_device_->SetPixelShader(NULL);

    return S_OK;
}
bool z_tex::get_z_tex(std::shared_ptr<IDirect3DTexture9>& cpTex)
{
    cpTex = ztex_;
    return true;
}
}

