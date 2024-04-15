#include "shadow_mesh.hpp"

using std::vector;
using std::shared_ptr;

namespace early_go
{
shadow_mesh::shadow_mesh(
    const std::shared_ptr<IDirect3DDevice9>& d3d_device,
    const shared_ptr<z_tex>& _ztex,
    const shared_ptr<depth_buf_shadow>& _depth_buf_shadow,
    const std::string& x_filename,
    const D3DXVECTOR3& pos,
    const D3DXVECTOR3& rotate,
    const float scale)
    : d3d_device_ { d3d_device }
{
    ztex_ = _ztex;
    depth_buf_shadow_ = _depth_buf_shadow;
    pos_ = pos;
    rotate_ = rotate;
    scale_ = scale;

    LPD3DXMESH temp_mesh { nullptr };

    vector<char> buffer { util::get_model_resource(x_filename) };

    LPD3DXBUFFER material_buffer { nullptr };
    HRESULT result { D3DXLoadMeshFromXInMemory(
        &buffer[0],
        static_cast<DWORD>(buffer.size()),
        D3DXMESH_SYSTEMMEM,
        d3d_device_.get(),
        nullptr,
        &material_buffer,
        nullptr,
        &materials_count_,
        &temp_mesh
    ) };

    if (FAILED(result))
    {
        THROW_WITH_TRACE("Failed to load a x-file.");
    }
    d3dx_mesh_.reset(temp_mesh, custom_deleter { });
    vector<std::shared_ptr<IDirect3DTexture9>> temp_textures { materials_count_ };
    textures_.swap(temp_textures);
    D3DXMATERIAL* materials { static_cast<D3DXMATERIAL*>(material_buffer->GetBufferPointer()) };
    for (DWORD i { 0 }; i < materials_count_; ++i)
    {
        if (materials[i].pTextureFilename != nullptr)
        {
            buffer = util::get_model_texture_resource(x_filename, materials[i].pTextureFilename);
            LPDIRECT3DTEXTURE9 temp_texture { };
            if (FAILED ( D3DXCreateTextureFromFileInMemory(
                d3d_device_.get(),
                &buffer[0],
                static_cast<UINT>(buffer.size()),
                &temp_texture)))
            {
                THROW_WITH_TRACE("texture file is not found.");
            }
            else
            {
                textures_.at(i).reset(temp_texture, custom_deleter { });
            }
        }
    }
    safe_release(material_buffer);

    {
        // TODO
        vector<char> buffer{util::get_image_resource("image/blank.png") };
        LPDIRECT3DTEXTURE9 temp_texture{};
        if (FAILED ( D3DXCreateTextureFromFileInMemory(
                d3d_device_.get(),
                &buffer[0],
                static_cast<UINT>(buffer.size()),
                &temp_texture)))
        {
            THROW_WITH_TRACE("texture file is not found.");
        }
        else
        {
            dynamic_textures_.reset( temp_texture, custom_deleter { });
            depth_buf_shadow_->set_dynamic_tex(dynamic_textures_);
        }
    }
}
void shadow_mesh::update()
{
    // do nothing.
}
void shadow_mesh::ztex_render()
{
    D3DXMATRIX world_mat { };
    {
        D3DXMatrixIdentity(&world_mat);
        D3DXMATRIX trans_mat { }, scale_mat { }, rotate_mat { }, trans_mat2 { };

        // TODO center coordinate
        D3DXMatrixTranslation(&trans_mat, 0.0f, 0.0f, 0.0f);
        D3DXMatrixScaling(&scale_mat, scale_, scale_, scale_);
        D3DXMatrixRotationYawPitchRoll(&rotate_mat, rotate_.x, rotate_.y, rotate_.z);
        D3DXMatrixTranslation(&trans_mat2, pos_.x, pos_.y, pos_.z);
        world_mat = world_mat * trans_mat * rotate_mat * scale_mat * trans_mat2;
    }
    ztex_->set_world_matrix(&world_mat);
    for (DWORD i { 0 }; i < materials_count_; i++)
    {
        ztex_->set_param_to_effect();
        ztex_->begin_pass();
        d3dx_mesh_->DrawSubset(i);
        ztex_->end_pass();
    }
}
void shadow_mesh::render(const D3DXMATRIX &view_matrix, const D3DXMATRIX &projection_matrix)
{
    D3DXMATRIX temp1 { view_matrix };
    depth_buf_shadow_->set_camera_view_matrix(&temp1);

    D3DXMATRIX world_mat { };
    {
        D3DXMatrixIdentity(&world_mat);
        D3DXMATRIX trans_mat { }, scale_mat { }, rotate_mat { }, trans_mat2 { };

        // TODO center coordinate
        D3DXMatrixTranslation(&trans_mat, 0.0f, 0.0f, 0.0f);
        D3DXMatrixScaling(&scale_mat, scale_, scale_, scale_);
        D3DXMatrixRotationYawPitchRoll(&rotate_mat, rotate_.x, rotate_.y, rotate_.z);
        D3DXMatrixTranslation(&trans_mat2, pos_.x, pos_.y, pos_.z);
        world_mat = world_mat * trans_mat * rotate_mat * scale_mat * trans_mat2;
    }
    depth_buf_shadow_->set_world_matrix(&world_mat);
    depth_buf_shadow_->set_dynamic_tex(dynamic_textures_);
    for (DWORD i { 0 }; i < materials_count_; i++)
    {
        depth_buf_shadow_->set_tex(textures_.at(i));
        depth_buf_shadow_->set_param_to_effect();
        depth_buf_shadow_->begin_pass();
        d3dx_mesh_->DrawSubset(i);
        depth_buf_shadow_->end_pass();
    }
}
void shadow_mesh::set_position(const D3DXVECTOR3& pos)
{
    pos_ = pos;
}
void shadow_mesh::set_rotate(const D3DXVECTOR3& rotate)
{
    rotate_ = rotate;
}
void shadow_mesh::set_scale(const float scale)
{
    scale_ = scale;
}
}