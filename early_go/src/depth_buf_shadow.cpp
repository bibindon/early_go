#include "depth_buf_shadow.hpp"

#include "stdafx.hpp"
#include <tchar.h>

using std::vector;

namespace early_go
{

depth_buf_shadow::depth_buf_shadow()
{
}
bool depth_buf_shadow::init(std::shared_ptr<IDirect3DDevice9> &d3dx_device)
{
    if (d3dx_device == nullptr)
    {
        return false;
    }

    vector<char> buffer { early_go::util::get_shader_resource("depth_buf_shadow.fx") };
    LPD3DXEFFECT effect{nullptr};
    if (FAILED ( D3DXCreateEffect(
        d3dx_device.get(),
        &buffer[0],
        static_cast<UINT>(buffer.size()),
        nullptr,
        nullptr,
        0,
        nullptr,
        &effect,
        nullptr)))
    {
        return false;
    }
    effect_.reset(effect, custom_deleter { });

    mat_world_handle_ = effect_->GetParameterByName(nullptr, "matWorld");
    camera_view_handle_ = effect_->GetParameterByName(nullptr, "matCameraView");
    camera_proj_handle_ = effect_->GetParameterByName(nullptr, "matCameraProj");
    light_view_handle_ = effect_->GetParameterByName(nullptr, "matLightView");
    light_proj_handle_ = effect_->GetParameterByName(nullptr, "matLightProj");
    shadow_map_handle_ = effect_->GetParameterByName(nullptr, "texShadowMap");
    mesh_tex_handle_ = effect_->GetParameterByName(nullptr, "g_mesh_texture");
    tex_handles_.at(0) = effect_->GetParameterByName(nullptr, "g_texture_0");
    tex_handles_.at(1) = effect_->GetParameterByName(nullptr, "g_texture_1");
    tex_handles_.at(2) = effect_->GetParameterByName(nullptr, "g_texture_2");
    tex_handles_.at(3) = effect_->GetParameterByName(nullptr, "g_texture_3");
    tex_handles_.at(4) = effect_->GetParameterByName(nullptr, "g_texture_4");
    tex_handles_.at(5) = effect_->GetParameterByName(nullptr, "g_texture_5");
    tex_handles_.at(6) = effect_->GetParameterByName(nullptr, "g_texture_6");
    tex_handles_.at(7) = effect_->GetParameterByName(nullptr, "g_texture_7");

    tex_pos_handles_ = effect_->GetParameterByName(nullptr, "g_position");
    tex_opacity_handles_ = effect_->GetParameterByName(nullptr, "g_opacity");

    technique_handle_ = effect_->GetTechniqueByName("DepthBufShadowTec");

    if (!mat_world_handle_ ||
        !camera_view_handle_ ||
        !camera_proj_handle_ ||
        !light_view_handle_ ||
        !light_proj_handle_ ||
        !shadow_map_handle_ ||
        !technique_handle_)
    {
        return false;
    }

    d3dx_device_ = d3dx_device;
    return true;
}
void depth_buf_shadow::set_shadow_map(std::shared_ptr<IDirect3DTexture9> &shadow_map_tex)
{
    if (shadow_map_tex == nullptr)
    {
        return;
    }
    shadow_map_tex_ = shadow_map_tex;
}
void depth_buf_shadow::set_tex(const std::shared_ptr<IDirect3DTexture9> &_tex)
{
    this->mesh_tex_ = _tex;
}
void depth_buf_shadow::set_dynamic_tex(const int layer, const std::shared_ptr<IDirect3DTexture9> &_tex)
{
    textures_.at(layer) = _tex;
}
void depth_buf_shadow::set_dynamic_pos(const int layer, const D3DXVECTOR2& pos)
{
    tex_pos_.at(layer) = D3DXVECTOR4 { pos.x, pos.y, 0.f, 0.f };
}
void depth_buf_shadow::set_dynamic_opacity(const int layer, const float alpha)
{
    tex_opacities_.at(layer) = alpha;
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
        nullptr,
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
    d3dx_device_->SetVertexShader(nullptr);
    d3dx_device_->SetPixelShader(nullptr);

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
    HRESULT result { 0 };
    result = effect_->SetTexture(shadow_map_handle_, shadow_map_tex_.get());
    result = effect_->SetTexture(mesh_tex_handle_, mesh_tex_.get());

    effect_->SetVectorArray(tex_pos_handles_, &tex_pos_.at(0), LAYER);
    effect_->SetFloatArray(tex_opacity_handles_, &tex_opacities_.at(0), LAYER);

    for (int i = 0; i < LAYER; ++i)
    {
        effect_->SetTexture(tex_handles_.at(i), textures_.at(i).get());
    }

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

