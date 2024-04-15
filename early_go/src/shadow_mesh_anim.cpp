// 複数のアニメーションメッシュを表示したいときに、
// このクラスのインスタンスをたくさん作るのではなく
// 位置と角度を変えて、アニメーションの再生位置を指定して
// 一つのインスタンスを複数回表示することで、超高速化できる。
// しかし、現在、オブジェクトをたくさん表示する機会があるか不明なので未対応。
#include "stdafx.hpp"

#include "shadow_mesh_anim.hpp"
#include "animation_mesh_allocator.hpp"
#include "main_window.hpp"
#include "animation_strategy.hpp"

using std::vector;
using std::string;

namespace early_go
{
const string shadow_mesh_anim::SHADER_FILENAME = "animation_mesh_shader.fx";
// A custom deleter. 
void shadow_mesh_anim::frame_root_deleter_object::operator()(const LPD3DXFRAME frame_root)
{
    // Call the recursive release function. 
    release_mesh_allocator(frame_root);
}

// Releases recursively mesh containers owned by the 'shadow_mesh_anim_frame' inheriting 'D3DXFRAME'.
void shadow_mesh_anim::frame_root_deleter_object::release_mesh_allocator(const LPD3DXFRAME frame)
{
    // Release the 'pMeshContainer' of the member variable. 
    if (frame->pMeshContainer != nullptr)
    {
        allocator_->DestroyMeshContainer(frame->pMeshContainer);
    }
    // Call oneself. 
    if (frame->pFrameSibling != nullptr)
    {
        release_mesh_allocator(frame->pFrameSibling);
    }
    // Call oneself. 
    if (frame->pFrameFirstChild != nullptr)
    {
        release_mesh_allocator(frame->pFrameFirstChild);
    }
    // Release oneself. 
    allocator_->DestroyFrame(frame);
}

// Reads a mesh file, and sets the frame and the animation controller given to member variables.
shadow_mesh_anim::shadow_mesh_anim(
    const std::shared_ptr<IDirect3DDevice9> &d3d_device,
    const std::shared_ptr<z_tex>& _ztex,
    const std::shared_ptr<depth_buf_shadow>& _depth_buf_shadow,
    const string &x_filename,
    const D3DXVECTOR3 &position,
    const D3DXVECTOR3 &rotation,
    const float scale)
    : d3d_device_ { d3d_device }
    , allocator_ { new_crt animation_mesh_allocator { x_filename } }
    , frame_root_ { nullptr, frame_root_deleter_object { allocator_ } }
    , rotation_matrix_ { D3DMATRIX { } }
    , center_coodinate_ { 0.0f, 0.0f, 0.0f }
    , world_handle_ { }
    , world_view_proj_handle_ { }
{
    ztex_ = _ztex;
    depth_buf_shadow_ = _depth_buf_shadow;
    pos_ = position;
    rotate_ = rotation;
    scale_ = scale;

    LPD3DXFRAME temp_root_frame { nullptr };
    LPD3DXANIMATIONCONTROLLER temp_animation_controller { nullptr };

    vector<char> buffer { util::get_model_resource(x_filename) };

    HRESULT result { D3DXLoadMeshHierarchyFromXInMemory(
        &buffer[0],
        static_cast<DWORD>(buffer.size()),
        D3DXMESH_MANAGED,
        d3d_device_.get(),
        allocator_.get(),
        nullptr,
        &temp_root_frame,
        &temp_animation_controller) };

    if (FAILED(result))
    {
        THROW_WITH_TRACE("Failed to load a x-file.");
    }
    // lazy initialization 
    frame_root_.reset(temp_root_frame);
    animation_strategy_.reset(new_crt normal_animation { temp_animation_controller });

}

void shadow_mesh_anim::update()
{
    animation_strategy_->update();
}

void shadow_mesh_anim::ztex_render()
{
    D3DXMATRIX base_mat { }, trans_mat { }, scale_mat { }, rotate_mat { }, trans_mat2 { };
    D3DXMatrixIdentity(&base_mat);
    D3DXMatrixTranslation(&trans_mat, 0.f, 0.f, 0.f);
    D3DXMatrixScaling(&scale_mat, scale_, scale_, scale_);
    D3DXMatrixRotationYawPitchRoll(&rotate_mat, rotate_.x, rotate_.y, rotate_.z);
    D3DXMatrixTranslation(&trans_mat2, pos_.x, pos_.y, pos_.z);
    world_mat_ = base_mat * trans_mat * rotate_mat * scale_mat * trans_mat2;

    update_frame_matrix(frame_root_.get(), &world_mat_, true);
    render_frame(frame_root_.get(), true);
}

// Renders its own animation mesh. 
void shadow_mesh_anim::render()
{
    D3DXMATRIX base_mat { }, trans_mat { }, scale_mat { }, rotate_mat { }, trans_mat2 { };
    D3DXMatrixIdentity(&base_mat);
    D3DXMatrixTranslation(&trans_mat, 0.f, 0.f, 0.f);
    D3DXMatrixScaling(&scale_mat, scale_, scale_, scale_);
    D3DXMatrixRotationYawPitchRoll(&rotate_mat, rotate_.x, rotate_.y, rotate_.z);
    D3DXMatrixTranslation(&trans_mat2, pos_.x, pos_.y, pos_.z);
    world_mat_ = base_mat * trans_mat * rotate_mat * scale_mat * trans_mat2;

    update_frame_matrix(frame_root_.get(), &world_mat_, false);
    render_frame(frame_root_.get(), false);
}

// Updates a world-transformation-matrix each the mesh in the frame. Also, this is a recursive
// function.
void shadow_mesh_anim::update_frame_matrix(
    const LPD3DXFRAME frame_base,
    const LPD3DXMATRIX parent_matrix,
    const bool ztex)
{
    animation_mesh_frame *frame { static_cast<animation_mesh_frame *>(frame_base) };
    // Multiply its own transformation matrix by the parent transformation matrix.
    if (parent_matrix != nullptr)
    {
        frame->combined_matrix_ = frame->TransformationMatrix * (*parent_matrix);
    }
    else
    {
        frame->combined_matrix_ = frame->TransformationMatrix;
    }

    // 立方体のワールド変換行列を登録
    if (ztex)
    {
        ztex_->set_world_matrix(&frame->combined_matrix_);
    }
    else
    {
        depth_buf_shadow_->set_world_matrix(&frame->combined_matrix_);
    }

    // Call oneself. 
    if (frame->pFrameSibling != nullptr)
    {
        update_frame_matrix(frame->pFrameSibling, parent_matrix, ztex);
    }
    // Call oneself. 
    if (frame->pFrameFirstChild != nullptr)
    {
        update_frame_matrix(frame->pFrameFirstChild, &frame->combined_matrix_, ztex);
    }
}

// Calls the 'render_mesh_container' function recursively. 
void shadow_mesh_anim::render_frame(const LPD3DXFRAME frame, const bool ztex)
{
    LPD3DXMESHCONTAINER mesh_container { frame->pMeshContainer };
    while (mesh_container != nullptr)
    {
        if (ztex)
        {
            render_mesh_container_ztex(mesh_container, frame);
        }
        else
        {
            render_mesh_container(mesh_container, frame);
        }
        mesh_container = mesh_container->pNextMeshContainer;
    }

    // Call oneself. 
    if (frame->pFrameSibling != nullptr)
    {
        render_frame(frame->pFrameSibling, ztex);
    }
    // Call oneself. 
    if (frame->pFrameFirstChild != nullptr)
    {
        render_frame(frame->pFrameFirstChild, ztex);
    }
}

void shadow_mesh_anim::render_mesh_container_ztex(
    const LPD3DXMESHCONTAINER mesh_container_base,
    const LPD3DXFRAME frame_base)
{
    // Cast for making child class' function callable. 
    animation_mesh_frame *frame { static_cast<animation_mesh_frame *>(frame_base) };

    ztex_->set_world_matrix(&frame->combined_matrix_);

    animation_mesh_container *mesh_container {
        static_cast<animation_mesh_container *>(mesh_container_base) };

    for (DWORD i { 0 }; i < mesh_container->NumMaterials; ++i)
    {
        ztex_->set_param_to_effect();
        ztex_->begin_pass();
        mesh_container->MeshData.pMesh->DrawSubset(i);
        ztex_->end_pass();
    }
}
void shadow_mesh_anim::render_mesh_container(
    const LPD3DXMESHCONTAINER mesh_container_base,
    const LPD3DXFRAME frame_base)
{
    // Cast for making child class' function callable. 
    animation_mesh_frame *frame { static_cast<animation_mesh_frame *>(frame_base) };

    depth_buf_shadow_->set_world_matrix(&frame->combined_matrix_);

    animation_mesh_container *mesh_container {
        static_cast<animation_mesh_container *>(mesh_container_base) };

    for (DWORD i { 0 }; i < mesh_container->NumMaterials; ++i)
    {
        depth_buf_shadow_->set_tex(mesh_container->texture_.at(i));
        depth_buf_shadow_->set_param_to_effect();
        depth_buf_shadow_->begin_pass();
        mesh_container->MeshData.pMesh->DrawSubset(i);
        depth_buf_shadow_->end_pass();
    }
}
void shadow_mesh_anim::set_position(const D3DXVECTOR3& pos)
{
    pos_ = pos;
}
void shadow_mesh_anim::set_rotate(const D3DXVECTOR3& rotate)
{
    rotate_ = rotate;
}
void shadow_mesh_anim::set_scale(const float scale)
{
    scale_ = scale;
}
} // namespace early_go 

