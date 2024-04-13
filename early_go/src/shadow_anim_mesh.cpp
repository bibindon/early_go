#include "stdafx.hpp"

#include "shadow_anim_mesh.hpp"
#include "animation_mesh_allocator.hpp"
#include "main_window.hpp"
#include "animation_strategy.hpp"

using std::vector;
using std::string;

namespace early_go
{
const string shadow_anim_mesh::SHADER_FILENAME = "animation_mesh_shader.fx";
// A custom deleter. 
void shadow_anim_mesh::frame_root_deleter_object::operator()(const LPD3DXFRAME frame_root)
{
    // Call the recursive release function. 
    release_mesh_allocator(frame_root);
}

// Releases recursively mesh containers owned by the 'shadow_anim_mesh_frame' inheriting 'D3DXFRAME'.
void shadow_anim_mesh::frame_root_deleter_object::release_mesh_allocator(const LPD3DXFRAME frame)
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
shadow_anim_mesh::shadow_anim_mesh(
    const std::shared_ptr<IDirect3DDevice9> &d3d_device,
    z_tex* ZTexCreator,
    depth_buf_shadow* DepthBS,
    const string &x_filename,
    const D3DXVECTOR3 &position,
    const D3DXVECTOR3 &rotation,
    const float &scale)
    : d3d_device_ { d3d_device }
    , allocator_ { new_crt animation_mesh_allocator { x_filename } }
    , frame_root_ { nullptr, frame_root_deleter_object { allocator_ } }
    , rotation_matrix_ { D3DMATRIX { } }
    , center_coodinate_ { 0.0f, 0.0f, 0.0f }
    , world_handle_ { }
    , world_view_proj_handle_ { }
{
    ZTexCreator_ = ZTexCreator;
    DepthBS_ = DepthBS;

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

    scale_ = scale;
}

shadow_anim_mesh::~shadow_anim_mesh()
{
}

void shadow_anim_mesh::update()
{
}

void shadow_anim_mesh::ztex_render()
{
    D3DXMATRIX world_mat { };
    {
        D3DXMatrixIdentity(&world_mat);
        D3DXMATRIX trans_mat { }, scale_mat { }, rotate_mat { }, trans_mat2 { };

        D3DXMatrixTranslation(&trans_mat, 0.0f, 0.0f, 0.0f);
        D3DXMatrixScaling(&scale_mat, 1.0f, 1.0f, 1.0f);
        D3DXMatrixRotationYawPitchRoll(&rotate_mat, 0.0f, 0.0f, 0.0f);
        D3DXMatrixTranslation(&trans_mat2, 0.0f, 0.5f, 10.0f);
        world_mat = world_mat * trans_mat * rotate_mat * scale_mat * trans_mat2;
    }

    render_frame(frame_root_.get(), true);
//    ZTexCreator_->set_world_matrix(&world_mat); // 立方体のワールド変換行列を登録
//    for (DWORD i { 0 }; i < materials_count_; i++)
//    {
//        ZTexCreator_->set_param_to_effect(); // 描画の直前に呼び出す必要があります
//        ZTexCreator_->begin_pass();
//        d3dx_mesh_->DrawSubset(i); // メッシュ描画
//        ZTexCreator_->end_pass();
//    }
}

// Renders its own animation mesh. 
void shadow_anim_mesh::render(
    const D3DXMATRIX &view_matrix, const D3DXMATRIX &projection_matrix)
{
    view_matrix_ = view_matrix;
    projection_matrix_ = projection_matrix;

    animation_strategy_->update();

    D3DXMATRIX world_matrix { };
    {
        D3DXMatrixIdentity(&world_matrix);
        D3DXMATRIX trans_mat { }, scale_mat { }, rotate_mat { }, trans_mat2 { };

        D3DXMatrixTranslation(&trans_mat, 0.0f, 0.0f, 0.0f);
        D3DXMatrixScaling(&scale_mat, 1.0f, 1.0f, 1.0f);
        D3DXMatrixRotationYawPitchRoll(&rotate_mat, 0.0f, 0.0f, 0.0f);
        D3DXMatrixTranslation(&trans_mat2, 0.0f, 0.5f, 10.0f);
        world_matrix = world_matrix * trans_mat * rotate_mat * scale_mat * trans_mat2;
    }

    update_frame_matrix(frame_root_.get(), &world_matrix);
    render_frame(frame_root_.get(), false);
}

// Updates a world-transformation-matrix each the mesh in the frame. Also, this is a recursive
// function.
void shadow_anim_mesh::update_frame_matrix(
    const LPD3DXFRAME frame_base, const LPD3DXMATRIX parent_matrix)
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
//    if (_ztex)
    {
        ZTexCreator_->set_world_matrix(&frame->combined_matrix_);
    }
//    else
    {
        DepthBS_->set_world_matrix(&frame->combined_matrix_);
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
void shadow_anim_mesh::render_frame(const LPD3DXFRAME frame, const bool ztex)
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

void shadow_anim_mesh::render_mesh_container(
    const LPD3DXMESHCONTAINER mesh_container_base,
    const LPD3DXFRAME frame_base,
    const bool ztex)
{
    // Cast for making child class' function callable. 
    animation_mesh_frame *frame { static_cast<animation_mesh_frame *>(frame_base) };

    D3DXMATRIX world_view_projection_matrix { frame->combined_matrix_ };

//    effect_->SetMatrix(world_handle_, &world_view_projection_matrix);
    if (ztex)
    {
        ZTexCreator_->set_world_matrix(&frame->combined_matrix_);
    }
    else
    {
        DepthBS_->set_world_matrix(&frame->combined_matrix_);
    }

//    world_view_projection_matrix *= view_matrix_;
//    world_view_projection_matrix *= projection_matrix_;

//    effect_->SetMatrix(world_view_proj_handle_, &world_view_projection_matrix);

//    effect_->Begin(nullptr, 0);

//    if (FAILED(effect_->BeginPass(0)))
//    {
//        effect_->End();
//        THROW_WITH_TRACE("Failed 'BeginPass' function.");
//    }
//    DepthBS_->begin_pass();

    animation_mesh_container *mesh_container {
        static_cast<animation_mesh_container *>(mesh_container_base) };

    for (DWORD i { 0 }; i < mesh_container->NumMaterials; ++i)
    {
//        D3DXVECTOR4 color {
//            mesh_container->pMaterials[i].MatD3D.Diffuse.r,
//            mesh_container->pMaterials[i].MatD3D.Diffuse.g,
//            mesh_container->pMaterials[i].MatD3D.Diffuse.b,
//            mesh_container->pMaterials[i].MatD3D.Diffuse.a };
//        effect_->SetVector(diffuse_handle_, &color);
//        effect_->SetTexture(mesh_texture_handle_, mesh_container->texture_.at(i).get());
        if (ztex)
        {
            ZTexCreator_->set_param_to_effect(); // 描画の直前に呼び出す必要があります
            ZTexCreator_->begin_pass();
        }
        else
        {
            DepthBS_->set_tex(mesh_container->texture_.at(i));
            DepthBS_->set_param_to_effect(); // 描画の直前に呼び出す必要があります
            DepthBS_->begin_pass();
        }

//        effect_->CommitChanges();
        mesh_container->MeshData.pMesh->DrawSubset(i);
    }
//    effect_->EndPass();
//    effect_->End();
    if (ztex)
    {
        ZTexCreator_->end_pass();
    }
    else
    {
        DepthBS_->end_pass();
    }
}
} // namespace early_go 

