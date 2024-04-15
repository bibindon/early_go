#include "stdafx.hpp"

#include "shadow_skin_mesh.hpp"
#include "skinned_animation_mesh_allocator.hpp"
#include "main_window.hpp"
#include "animation_strategy.hpp"

using std::vector;
using std::string;

namespace early_go
{
const string shadow_skin_mesh::SHADER_FILENAME = "animation_mesh_shader.fx";
// A custom deleter. 
void shadow_skin_mesh::frame_root_deleter_object::operator()(const LPD3DXFRAME frame_root)
{
    // Call the recursive release function. 
    release_mesh_allocator(frame_root);
}

// Releases recursively mesh containers owned by the 'shadow_anim_mesh_frame' inheriting 'D3DXFRAME'.
void shadow_skin_mesh::frame_root_deleter_object::release_mesh_allocator(const LPD3DXFRAME frame)
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
shadow_skin_mesh::shadow_skin_mesh(
    const std::shared_ptr<IDirect3DDevice9> &d3d_device,
    const std::shared_ptr<z_tex>& _ztex,
    const std::shared_ptr<depth_buf_shadow>& depth_buf_shadow,
    const string &x_filename,
    const D3DXVECTOR3 &position,
    const D3DXVECTOR3 &rotation,
    const float &scale)
    : d3d_device_ { d3d_device }
    , allocator_ { new_crt skinned_animation_mesh_allocator { x_filename } }
    , frame_root_ { nullptr, frame_root_deleter_object { allocator_ } }
    , rotation_matrix_ { D3DMATRIX { } }
    , center_coodinate_ { 0.0f, 0.0f, 0.0f }
    , world_handle_ { }
    , world_view_proj_handle_ { }
{
    ztex_ = _ztex;
    depth_buf_shadow_ = depth_buf_shadow;

//    world_handle_ = effect_->GetParameterByName(nullptr, "matWorld");
//    world_view_proj_handle_ = effect_->GetParameterByName(nullptr, "g_world_view_projection");

    LPD3DXFRAME temp_root_frame { nullptr };
    LPD3DXANIMATIONCONTROLLER temp_animation_controller { nullptr };

    vector<char> buffer = util::get_model_resource(x_filename);

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

    scale_ = scale;
}
HRESULT shadow_skin_mesh::allocate_bone_matrix(LPD3DXMESHCONTAINER mesh_container)
{
    skinned_animation_mesh_frame *frame{};

    skinned_animation_mesh_container *skinned_mesh_container =
        static_cast<skinned_animation_mesh_container *>(mesh_container);

    DWORD bone_count = skinned_mesh_container->pSkinInfo->GetNumBones();
    skinned_mesh_container->frame_combined_matrix_.resize(bone_count);

    // TODO Improve.
    DWORD MAX_MATRICES = 26;
    world_matrix_array_.resize(std::min(MAX_MATRICES, bone_count));

    bone_num_ = skinned_mesh_container->influence_count_ - 1;

    for (DWORD i{}; i < bone_count; ++i)
    {
        LPD3DXFRAME p = D3DXFrameFind(frame_root_.get(),
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
HRESULT shadow_skin_mesh::allocate_all_bone_matrices(LPD3DXFRAME frame)
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

shadow_skin_mesh::~shadow_skin_mesh()
{
}

void shadow_skin_mesh::update()
{
}

void shadow_skin_mesh::ztex_render()
{
    animation_strategy_->update();

    ztex_->set_bone_num(bone_num_);

    D3DXMATRIX world_mat { };
    {
        D3DXMatrixIdentity(&world_mat);
        D3DXMATRIX trans_mat { }, scale_mat { }, rotate_mat { }, trans_mat2 { };

        D3DXMatrixTranslation(&trans_mat, 0.0f, 0.0f, 0.0f);
        D3DXMatrixScaling(&scale_mat, 3.01f, 3.01f, 3.01f);
        D3DXMatrixRotationYawPitchRoll(&rotate_mat, 0.0f, 0.0f, 0.0f);
        D3DXMatrixTranslation(&trans_mat2, 1.0f, 0.0f, 3.0f);
        world_mat = world_mat * trans_mat * rotate_mat * scale_mat * trans_mat2;
    }

    update_frame_matrix(frame_root_.get(), &world_mat);
    render_frame(frame_root_.get(), true);
//    ztex_->set_world_matrix(&world_mat); // 立方体のワールド変換行列を登録
//    for (DWORD i { 0 }; i < materials_count_; i++)
//    {
//        ztex_->set_param_to_effect(); // 描画の直前に呼び出す必要があります
//        ztex_->begin_pass();
//        d3dx_mesh_->DrawSubset(i); // メッシュ描画
//        ztex_->end_pass();
//    }
}

// Renders its own animation mesh. 
void shadow_skin_mesh::render(
    const D3DXMATRIX &view_matrix, const D3DXMATRIX &projection_matrix)
{
    depth_buf_shadow_->set_bone_num(bone_num_);
    view_matrix_ = view_matrix;
    projection_matrix_ = projection_matrix;

    D3DXMATRIX world_matrix { };
    {
        D3DXMatrixIdentity(&world_matrix);
        D3DXMATRIX trans_mat { }, scale_mat { }, rotate_mat { }, trans_mat2 { };

        D3DXMatrixTranslation(&trans_mat, 0.0f, 0.0f, 0.0f);
        D3DXMatrixScaling(&scale_mat, 3.01f, 3.01f, 3.01f);
        D3DXMatrixRotationYawPitchRoll(&rotate_mat, 0.0f, 0.0f, 0.0f);
        D3DXMatrixTranslation(&trans_mat2, 1.0f, 0.0f, 3.0f);
        world_matrix = world_matrix * trans_mat * rotate_mat * scale_mat * trans_mat2;
    }

    update_frame_matrix(frame_root_.get(), &world_matrix);
    render_frame(frame_root_.get(), false);
}

// Updates a world-transformation-matrix each the mesh in the frame. Also, this is a recursive
// function.
void shadow_skin_mesh::update_frame_matrix(
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

    // 立方体のワールド変換行列を登録
//    if (_ztex)
    {
//        ztex_->set_world_matrix(&frame->combined_matrix_);
    }
//    else
    {
        depth_buf_shadow_->set_world_matrix(&frame->combined_matrix_);
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
void shadow_skin_mesh::render_frame(const LPD3DXFRAME frame, const bool ztex)
{
    {
        LPD3DXMESHCONTAINER mesh_container { frame->pMeshContainer };
        while (mesh_container != nullptr)
        {
            render_mesh_container(mesh_container, frame, ztex);
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

void shadow_skin_mesh::render_mesh_container(
    const LPD3DXMESHCONTAINER mesh_container_base,
    const LPD3DXFRAME frame_base,
    const bool ztex)
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
        if (ztex)
        {
            ztex_->set_skin_mat_array(world_matrix_array_);
        }
        else
        {
            depth_buf_shadow_->set_skin_mat_array(world_matrix_array_);
        }

        DWORD bone_id = bone_combination[i].AttribId;
        if (!ztex)
        {
            depth_buf_shadow_->set_tex(mesh_container->texture_.at(bone_id));
        }

        if (ztex)
        {
            ztex_->set_param_to_effect();
            ztex_->begin_pass(1);
            mesh_container->MeshData.pMesh->DrawSubset(i);
            ztex_->end_pass();
        }
        else
        {
            depth_buf_shadow_->set_param_to_effect();
            depth_buf_shadow_->begin_pass(1);
            mesh_container->MeshData.pMesh->DrawSubset(i);
            depth_buf_shadow_->end_pass();
        }
//        effect_->Begin(nullptr, D3DXFX_DONOTSAVESTATE);

//        if (FAILED(effect_->BeginPass(0)))
//        {
//            effect_->End();
//            THROW_WITH_TRACE("Failed 'BeginPass' function.");
//        }
//        effect_->CommitChanges();
//        mesh_container->MeshData.pMesh->DrawSubset(i);
//        effect_->EndPass();
//        effect_->End();
    }































//    // Cast for making child class' function callable. 
//    skinned_animation_mesh_frame *frame { static_cast<skinned_animation_mesh_frame *>(frame_base) };
//
//    D3DXMATRIX world_view_projection_matrix { frame->combined_matrix_ };
//
////    effect_->SetMatrix(world_handle_, &world_view_projection_matrix);
//    if (ztex)
//    {
//        ztex_->set_world_matrix(&frame->combined_matrix_);
//        ztex_->set_bone_num(bone_num_);
//    }
//    else
//    {
//        depth_buf_shadow_->set_world_matrix(&frame->combined_matrix_);
//    }
//
//    world_view_projection_matrix *= view_matrix_;
//    world_view_projection_matrix *= projection_matrix_;

//    effect_->SetMatrix(world_view_proj_handle_, &world_view_projection_matrix);

//    effect_->Begin(nullptr, 0);

//    if (FAILED(effect_->BeginPass(0)))
//    {
//        effect_->End();
//        THROW_WITH_TRACE("Failed 'BeginPass' function.");
//    }
//    depth_buf_shadow_->begin_pass();

//    skinned_animation_mesh_container *mesh_container {
//        static_cast<skinned_animation_mesh_container *>(mesh_container_base) };
//
//    for (DWORD i { 0 }; i < mesh_container->NumMaterials; ++i)
//    {
////        D3DXVECTOR4 color {
////            mesh_container->pMaterials[i].MatD3D.Diffuse.r,
////            mesh_container->pMaterials[i].MatD3D.Diffuse.g,
////            mesh_container->pMaterials[i].MatD3D.Diffuse.b,
////            mesh_container->pMaterials[i].MatD3D.Diffuse.a };
////        effect_->SetVector(diffuse_handle_, &color);
////        effect_->SetTexture(mesh_texture_handle_, mesh_container->texture_.at(i).get());
//        if (ztex)
//        {
//            ztex_->set_param_to_effect(); // 描画の直前に呼び出す必要があります
//            ztex_->begin_pass(1);
//        }
//        else
//        {
//            depth_buf_shadow_->set_tex(mesh_container->texture_.at(i));
//            depth_buf_shadow_->set_param_to_effect(); // 描画の直前に呼び出す必要があります
//            depth_buf_shadow_->begin_pass();
//        }
//
////        effect_->CommitChanges();
//        mesh_container->MeshData.pMesh->DrawSubset(i);
//
//        if (ztex)
//        {
//            ztex_->end_pass();
//        }
//        else
//        {
//            depth_buf_shadow_->end_pass();
//        }
//    }
//    effect_->EndPass();
//    effect_->End();
}
} // namespace early_go 

