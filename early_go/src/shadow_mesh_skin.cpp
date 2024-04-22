#include "stdafx.hpp"

#include "shadow_mesh_skin.hpp"
#include "skinned_animation_mesh_allocator.hpp"
#include "main_window.hpp"
#include "animation_strategy.hpp"

using std::vector;
using std::string;

namespace early_go
{
// A custom deleter. 
void shadow_mesh_skin::frame_root_deleter_object::operator()(const LPD3DXFRAME frame_root)
{
    // Call the recursive release function. 
    release_mesh_allocator(frame_root);
}

// Releases recursively mesh containers owned by the 'shadow_mesh_anim_frame' inheriting 'D3DXFRAME'.
void shadow_mesh_skin::frame_root_deleter_object::release_mesh_allocator(const LPD3DXFRAME frame)
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
shadow_mesh_skin::shadow_mesh_skin(
    const std::shared_ptr<IDirect3DDevice9> &d3d_device,
    const std::shared_ptr<z_tex>& _ztex,
    const std::shared_ptr<depth_buf_shadow>& _depth_buf_shadow,
    const string &x_filename,
    const D3DXVECTOR3 &position,
    const D3DXVECTOR3 &rotation,
    const float scale)
    : d3d_device_ { d3d_device }
    , allocator_ { new_crt skinned_animation_mesh_allocator { x_filename } }
    , frame_root_ { nullptr, frame_root_deleter_object { allocator_ } }
    , center_coodinate_ { 0.0f, 0.0f, 0.0f }
{
    ztex_ = _ztex;
    depth_buf_shadow_ = _depth_buf_shadow;
    scale_ = scale;
    x_filename_ = x_filename;

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

    allocate_all_bone_matrices(frame_root_.get());
}
HRESULT shadow_mesh_skin::allocate_bone_matrix(LPD3DXMESHCONTAINER mesh_container)
{
    skinned_animation_mesh_frame *frame { nullptr };

    skinned_animation_mesh_container *skinned_mesh_container =
        static_cast<skinned_animation_mesh_container *>(mesh_container);

    DWORD bone_count = skinned_mesh_container->pSkinInfo->GetNumBones();
    skinned_mesh_container->frame_combined_matrix_.resize(bone_count);

    // TODO Improve.
    DWORD MAX_MATRICES = 26;
    world_matrix_array_.resize(std::min(MAX_MATRICES, bone_count));

    bone_num_ = skinned_mesh_container->influence_count_ - 1;

    for (DWORD i { 0 }; i < bone_count; ++i)
    {
        LPD3DXFRAME p = D3DXFrameFind(
            frame_root_.get(),
            skinned_mesh_container->pSkinInfo->GetBoneName(i));

        frame = static_cast<skinned_animation_mesh_frame *>(p);

        if (frame == nullptr)
        {
            return E_FAIL;
        }
        LPD3DXMATRIX p_matrix = &frame->combined_matrix_;
        skinned_mesh_container->frame_combined_matrix_.at(i) = p_matrix;
    }
    return S_OK;
}
HRESULT shadow_mesh_skin::allocate_all_bone_matrices(LPD3DXFRAME frame)
{
    if (frame->pMeshContainer != nullptr)
    {
        // TODO why frame->pMeshContainer->pNextMeshContainer is unnecessary?
        if (FAILED(allocate_bone_matrix(frame->pMeshContainer)))
        {
            return E_FAIL;
        }
    }
    if (frame->pFrameSibling != nullptr)
    {
        if (FAILED(allocate_all_bone_matrices(frame->pFrameSibling)))
        {
            return E_FAIL;
        }
    }
    if (frame->pFrameFirstChild != nullptr)
    {
        if (FAILED(allocate_all_bone_matrices(frame->pFrameFirstChild)))
        {
            return E_FAIL;
        }
    }
    return S_OK;
}

void shadow_mesh_skin::update()
{
    // TODO メッシュの数が増えるとアニメーションの速度が上がってしまう気がする。
    animation_strategy_->update();
}

void shadow_mesh_skin::ztex_render()
{
    ztex_->set_bone_num(bone_num_);

    D3DXMATRIX world_mat { };
    D3DXMATRIX base_mat { }, trans_mat { }, scale_mat { }, rotate_mat { }, trans_mat2 { };
    D3DXMatrixIdentity(&base_mat);
    D3DXMatrixTranslation(&trans_mat, 0.f, 0.f, 0.f);
    D3DXMatrixScaling(&scale_mat, scale_, scale_, scale_);
    D3DXMatrixRotationYawPitchRoll(&rotate_mat, rotate_.x, rotate_.y, rotate_.z);
    D3DXMatrixTranslation(&trans_mat2, pos_.x, pos_.y, pos_.z);
    world_mat = base_mat * trans_mat * scale_mat * rotate_mat * trans_mat2;

    update_frame_matrix(frame_root_.get(), &world_mat);
    render_frame(frame_root_.get(), true);
}

// Renders its own animation mesh. 
void shadow_mesh_skin::render()
{
    depth_buf_shadow_->set_bone_num(bone_num_);

    D3DXMATRIX world_mat { };
    D3DXMATRIX base_mat { }, trans_mat { }, scale_mat { }, rotate_mat { }, trans_mat2 { };
    D3DXMatrixIdentity(&base_mat);
    D3DXMatrixTranslation(&trans_mat, 0.f, 0.f, 0.f);
    D3DXMatrixScaling(&scale_mat, scale_, scale_, scale_);
    D3DXMatrixRotationYawPitchRoll(&rotate_mat, rotate_.x, rotate_.y, rotate_.z);
    D3DXMatrixTranslation(&trans_mat2, pos_.x, pos_.y, pos_.z);
    world_mat = base_mat * trans_mat * scale_mat * rotate_mat * trans_mat2;

    update_frame_matrix(frame_root_.get(), &world_mat);
    render_frame(frame_root_.get(), false);
}

// Updates a world-transformation-matrix each the mesh in the frame. Also, this is a recursive
// function.
void shadow_mesh_skin::update_frame_matrix(
    const LPD3DXFRAME frame_base, const LPD3DXMATRIX parent_matrix)
{
    skinned_animation_mesh_frame *frame { static_cast<skinned_animation_mesh_frame *>(frame_base) };
    // Multiply its own transformation matrix by the parent transformation matrix.
    if (parent_matrix != nullptr)
    {
        frame->combined_matrix_ = frame->TransformationMatrix * (*parent_matrix);
    }
    else
    {
        frame->combined_matrix_ = frame->TransformationMatrix;
    }

    // Call oneself. 
    if (frame->pFrameSibling != nullptr)
    {
        update_frame_matrix(frame->pFrameSibling, parent_matrix);
    }
    // Call oneself. 
    if (frame->pFrameFirstChild != nullptr)
    {
        update_frame_matrix(frame->pFrameFirstChild, &frame->combined_matrix_);
    }
}

// Calls the 'render_mesh_container' function recursively. 
void shadow_mesh_skin::render_frame(const LPD3DXFRAME frame, const bool ztex)
{
    {
        LPD3DXMESHCONTAINER mesh_container { frame->pMeshContainer };
        while (mesh_container != nullptr)
        {
            if (ztex)
            {
                render_mesh_container_ztex(mesh_container);
            }
            else
            {
                render_mesh_container(mesh_container);
            }
            mesh_container = mesh_container->pNextMeshContainer;
        }
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

void shadow_mesh_skin::render_mesh_container_ztex(const LPD3DXMESHCONTAINER mesh_container_base)
{
    skinned_animation_mesh_container *mesh_container{
        static_cast<skinned_animation_mesh_container *>(mesh_container_base)};

    LPD3DXBONECOMBINATION bone_combination { };

    bone_combination = static_cast<LPD3DXBONECOMBINATION>(
        mesh_container->bone_buffer_->GetBufferPointer());

    const DWORD dw_palette_size { mesh_container->palette_size_ };

    for (DWORD i { 0 }; i < mesh_container->bone_count_; ++i)
    {
        for (DWORD k { 0 }; k < dw_palette_size; ++k)
        {
            DWORD dw_bone_id = bone_combination[i].BoneId[k];
            if (dw_bone_id == UINT_MAX)
            {
                continue;
            }
            world_matrix_array_[k] =
                mesh_container->bone_offset_matrices_[dw_bone_id] *
                (*mesh_container->frame_combined_matrix_[dw_bone_id]);
        }
        ztex_->set_skin_mat_array(world_matrix_array_);

        DWORD bone_id = bone_combination[i].AttribId;

        ztex_->set_param_to_effect();
        ztex_->begin_pass(1);
        mesh_container->MeshData.pMesh->DrawSubset(i);
        ztex_->end_pass();
    }
}
void shadow_mesh_skin::render_mesh_container(const LPD3DXMESHCONTAINER mesh_container_base)
{
    skinned_animation_mesh_container *mesh_container{
        static_cast<skinned_animation_mesh_container *>(mesh_container_base)};

    LPD3DXBONECOMBINATION bone_combination { };

    bone_combination = static_cast<LPD3DXBONECOMBINATION>(
        mesh_container->bone_buffer_->GetBufferPointer());

    const DWORD dw_palette_size { mesh_container->palette_size_ };

    for (DWORD i { 0 }; i < mesh_container->bone_count_; ++i)
    {
        for (DWORD k { 0 }; k < dw_palette_size; ++k)
        {
            DWORD dw_bone_id = bone_combination[i].BoneId[k];
            if (dw_bone_id == UINT_MAX)
            {
                continue;
            }
            world_matrix_array_[k] =
                mesh_container->bone_offset_matrices_[dw_bone_id] *
                (*mesh_container->frame_combined_matrix_[dw_bone_id]);
        }
        depth_buf_shadow_->set_skin_mat_array(world_matrix_array_);

        DWORD bone_id = bone_combination[i].AttribId;
        depth_buf_shadow_->set_tex(mesh_container->texture_.at(bone_id));

        depth_buf_shadow_->set_param_to_effect();
        depth_buf_shadow_->begin_pass(1);
        mesh_container->MeshData.pMesh->DrawSubset(i);
        depth_buf_shadow_->end_pass();
    }
}
void shadow_mesh_skin::set_position(const D3DXVECTOR3& pos)
{
    pos_ = pos;
}
void shadow_mesh_skin::set_rotate(const D3DXVECTOR3& rotate)
{
    rotate_ = rotate;
}
void shadow_mesh_skin::set_scale(const float scale)
{
    scale_ = scale;
}
std::string shadow_mesh_skin::x_filename()
{
    return x_filename_;
}
void shadow_mesh_skin::set_animation(const string &animation_set)
{
    animation_strategy_->set_animation(animation_set);
}

void shadow_mesh_skin::set_default_animation(const string &animation_name)
{
    animation_strategy_->set_default_animation(animation_name);
}
void shadow_mesh_skin::set_animation_config(
    const string &animation_name,
    const bool &loop,
    const float &duration)
{
    animation_strategy_->set_animation_config(animation_name, loop, duration);
}
} // namespace early_go 

