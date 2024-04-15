#include "depth_buf_shadow.hpp"

#include "stdafx.hpp"
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
bool depth_buf_shadow::init(std::shared_ptr<IDirect3DDevice9> &d3dx_device)
{
    if (d3dx_device == NULL)
    {
        return false;
    }

    vector<char> buffer{ early_go::util::get_shader_resource("depth_buf_shadow.fx") };
    LPD3DXEFFECT effect{nullptr};
    if (FAILED ( D3DXCreateEffect(
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

    mat_world_handle_ = effect_->GetParameterByName(NULL, "matWorld");
    camera_view_handle_ = effect_->GetParameterByName(NULL, "matCameraView");
    camera_proj_handle_ = effect_->GetParameterByName(NULL, "matCameraProj");
    light_view_handle_ = effect_->GetParameterByName(NULL, "matLightView");
    light_proj_handle_ = effect_->GetParameterByName(NULL, "matLightProj");
    shadow_map_handle_ = effect_->GetParameterByName(NULL, "texShadowMap");
    g_mesh_tex_handle_ = effect_->GetParameterByName(NULL, "g_mesh_texture");
    g_tex_handle_ = effect_->GetParameterByName(NULL, "g_texture_0");
    technique_handle_ = effect_->GetTechniqueByName("DepthBufShadowTec");

    if (!mat_world_handle_ || !camera_view_handle_ || !camera_proj_handle_ || !light_view_handle_ ||
        !light_proj_handle_ || !shadow_map_handle_ || !technique_handle_)
    {
        return false;
    }

    d3dx_device_ = d3dx_device;
    return true;
}
bool depth_buf_shadow::set_shadow_map(std::shared_ptr<IDirect3DTexture9> &shadow_map_tex)
{
    if (shadow_map_tex == NULL)
    {
        return false;
    }
    shadow_map_tex_ = shadow_map_tex;
    return true;
}
bool depth_buf_shadow::set_tex(const std::shared_ptr<IDirect3DTexture9> &_tex)
{
    this->g_mesh_tex_ = _tex;
    return true;
}
bool depth_buf_shadow::set_dynamic_tex(const std::shared_ptr<IDirect3DTexture9> &_tex)
{
    this->g_texture0_ = _tex;
    return true;
}
void depth_buf_shadow::set_world_matrix(D3DXMATRIX *mat)
{
    mat_world_ = *mat;
}
void depth_buf_shadow::set_camera_view_matrix(D3DXMATRIX *mat)
{
    camera_mat_view_ = *mat;
}
void depth_buf_shadow::set_camera_proj_matrix(D3DXMATRIX *mat)
{
    camera_mat_proj_ = *mat;
}
void depth_buf_shadow::set_light_view_matrix(D3DXMATRIX *mat)
{
    light_mat_view_ = *mat;
}
void depth_buf_shadow::set_light_proj_matrix(D3DXMATRIX *mat)
{
    light_mat_proj_ = *mat;
}
void depth_buf_shadow::set_bone_num(const int bone_num)
{
    bone_num_ = bone_num;
}
void depth_buf_shadow::set_skin_mat_array(const std::vector<D3DXMATRIX>& skin_mat_arr)
{
    skin_mat_arr_ = std::vector<D3DXMATRIX>(skin_mat_arr);
}
HRESULT depth_buf_shadow::begin()
{
    // 各サーフェイスを初期化
    d3dx_device_->Clear(
        0,
        NULL,
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        D3DCOLOR_ARGB(255, 255, 255, 255),
        1.0f,
        0);

    // プログラマブルシェーダに切り替え
    effect_->SetTechnique(technique_handle_);

    // 開始宣言
    UINT Pass;
    effect_->Begin(&Pass, 0);

    return S_OK;
}
HRESULT depth_buf_shadow::end()
{
    effect_->End();

    // 固定機能に戻す
    d3dx_device_->SetVertexShader(NULL);
    d3dx_device_->SetPixelShader(NULL);

    return S_OK;
}
HRESULT depth_buf_shadow::begin_pass(const int pass)
{
    effect_->BeginPass(pass);
    return S_OK;
}
HRESULT depth_buf_shadow::end_pass()
{
    effect_->EndPass();
    return S_OK;
}
bool depth_buf_shadow::set_param_to_effect()
{
    effect_->SetMatrix(mat_world_handle_, &mat_world_);
    effect_->SetMatrix(camera_view_handle_, &camera_mat_view_);
    effect_->SetMatrix(camera_proj_handle_, &camera_mat_proj_);
    effect_->SetMatrix(light_view_handle_, &light_mat_view_);
    effect_->SetMatrix(light_proj_handle_, &light_mat_proj_);
    HRESULT hr = effect_->SetTexture(shadow_map_handle_, shadow_map_tex_.get());
    hr = effect_->SetTexture(g_mesh_tex_handle_, g_mesh_tex_.get());
    hr = effect_->SetTexture(g_tex_handle_, g_texture0_.get());
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
}

