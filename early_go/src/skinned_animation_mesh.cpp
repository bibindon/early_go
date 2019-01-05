#include "stdafx.hpp"

#include "skinned_animation_mesh.hpp"
#include "skinned_animation_mesh_allocator.hpp"
#include "basic_window.hpp"
#include "animation_strategy.hpp"

namespace early_go {
  const std::string skinned_animation_mesh::SHADER_FILENAME =
      "skinned_animation_mesh_shader.fx";

/* A custom deleter. */
void skinned_animation_mesh::frame_root_deleter_object::operator()(
    const ::LPD3DXFRAME frame_root)
{
  /* Call the recursive release function. */
  release_mesh_allocator(frame_root);
}

/*
 * Releases recursively mesh containers owned by the
 * 'skinned_animation_mesh_frame' inheriting '::D3DXFRAME'.
 */
void skinned_animation_mesh::frame_root_deleter_object::release_mesh_allocator(
    const ::LPD3DXFRAME frame)
{
  /*
   * Hint:
   *
   * // d3dx9anim.h
   * typedef struct _D3DXFRAME
   * {
   *     LPSTR                   Name;
   *     D3DXMATRIX              TransformationMatrix;
   *
   *     LPD3DXMESHCONTAINER     pMeshContainer;
   *
   *     struct _D3DXFRAME       *pFrameSibling;
   *     struct _D3DXFRAME       *pFrameFirstChild;
   * } D3DXFRAME, *LPD3DXFRAME;
   *
   */

  /* Release the 'pMeshContainer' of the member variable. */
  if (frame->pMeshContainer != nullptr) {
    allocator_->DestroyMeshContainer(frame->pMeshContainer);
  }
  /* Call oneself. */
  if (frame->pFrameSibling != nullptr) {
    release_mesh_allocator(frame->pFrameSibling);
  }
  /* Call oneself. */
  if (frame->pFrameFirstChild != nullptr) {
    release_mesh_allocator(frame->pFrameFirstChild);
  }
  /* Release oneself. */
  allocator_->DestroyFrame(frame);
}

/*
 * Reads a mesh file, and sets the frame and the animation controller given to
 * member variables.
 */
skinned_animation_mesh::skinned_animation_mesh(
    const std::shared_ptr<::IDirect3DDevice9>& d3d_device,
    const std::string& x_filename,
    const ::D3DXVECTOR3& position,
    const ::D3DXVECTOR3& rotation,
    const float& scale)
    : base_mesh{d3d_device, SHADER_FILENAME, position, rotation},
      d3d_device_{d3d_device},
      allocator_{new_crt skinned_animation_mesh_allocator{x_filename}},
      frame_root_{nullptr, frame_root_deleter_object{allocator_}},
      rotation_matrix_{::D3DMATRIX{}},
      center_coodinate_{0.0f, 0.0f, 0.0f},
      scale_{0.0f},
      view_projection_handle_{},
      scale_handle_{}
{
  view_projection_handle_ =
      effect_->GetParameterByName(nullptr, "g_view_projection");
  ::LPD3DXFRAME temp_frame_root{nullptr};
  ::LPD3DXANIMATIONCONTROLLER temp_animation_controller{nullptr};

  std::vector<char> buffer = get_resource(
      "SELECT DATA FROM MODEL WHERE FILENAME = '" + x_filename + "';");
  if (FAILED(::D3DXLoadMeshHierarchyFromXInMemory(
      &buffer[0],
      static_cast<DWORD>(buffer.size()),
      ::D3DXMESH_MANAGED,
      d3d_device_.get(),
      allocator_.get(),
      nullptr,
      &temp_frame_root,
      &temp_animation_controller))) {
    THROW_WITH_TRACE("Failed to load a x-file.: " + x_filename);
  }
  /* lazy initialization */
  frame_root_.reset(temp_frame_root);
  animation_strategy_.reset(
      new_crt normal_animation{temp_animation_controller});

  allocate_all_bone_matrices(frame_root_.get());

  scale_ = scale;
}

/* Renders its own animation mesh. */
void skinned_animation_mesh::render(const ::D3DXMATRIX& view_matrix,
                                    const ::D3DXMATRIX& projection_matrix)
{
  ::D3DXMATRIX view_projection_matrix{view_matrix * projection_matrix};

  effect_->SetMatrix(view_projection_handle_, &view_projection_matrix);

  (*animation_strategy_)();

  ::D3DXMATRIX world_matrix{};
  ::D3DXMatrixIdentity(&world_matrix);
  {
    ::D3DXMATRIX mat{};
    ::D3DXMatrixTranslation(&mat,
                            -center_coodinate_.x,
                            -center_coodinate_.y,
                            -center_coodinate_.z);
    world_matrix *= mat;

    ::D3DXMatrixScaling(&mat, scale_, scale_, scale_);
    world_matrix *= mat;

    ::D3DXMatrixRotationYawPitchRoll(
        &mat, rotation_.x, rotation_.y, rotation_.z);
    world_matrix *= mat;

    ::D3DXMatrixTranslation(&mat, position_.x, position_.y, position_.z);
    world_matrix *= mat;
  }

  update_frame_matrix(frame_root_.get(), &world_matrix);
  render_frame(frame_root_.get());
}

/*
 * Updates a world-transformation-matrix each the mesh in the frame. Also, this
 * is a recursive function.
 */
void skinned_animation_mesh::update_frame_matrix(
    const ::LPD3DXFRAME  frame_base,
    const ::LPD3DXMATRIX parent_matrix)
{
  skinned_animation_mesh_frame *frame{
      static_cast<skinned_animation_mesh_frame*>(frame_base)};
  /*
   * Multiply its own transformation matrix by the parent transformation
   * matrix.
   */
  if (parent_matrix != nullptr) {
    frame->combined_matrix_ = frame->TransformationMatrix * (*parent_matrix);
  } else {
    frame->combined_matrix_ = frame->TransformationMatrix;
  }

  /* Call oneself. */
  if (frame->pFrameSibling != nullptr) {
    update_frame_matrix(frame->pFrameSibling, parent_matrix);
  }
  /* Call oneself. */
  if (frame->pFrameFirstChild != nullptr) {
    update_frame_matrix(frame->pFrameFirstChild, &frame->combined_matrix_);
  }
}

/* Calls the 'render_mesh_container' function recursively. */
void skinned_animation_mesh::render_frame(const ::LPD3DXFRAME frame)
{
  {
    ::LPD3DXMESHCONTAINER mesh_container{frame->pMeshContainer};
    while (mesh_container != nullptr) {
      render_mesh_container(mesh_container);
      mesh_container = mesh_container->pNextMeshContainer;
    }
  }
  /* Call oneself. */
  if (frame->pFrameSibling != nullptr) {
    render_frame(frame->pFrameSibling);
  }
  /* Call oneself. */
  if (frame->pFrameFirstChild != nullptr) {
    render_frame(frame->pFrameFirstChild);
  }
}

void skinned_animation_mesh::render_mesh_container(
    const ::LPD3DXMESHCONTAINER mesh_container_base)
{
  skinned_animation_mesh_container *mesh_container{
      static_cast<skinned_animation_mesh_container*>(mesh_container_base)};

  ::LPD3DXBONECOMBINATION bone_combination{};

  bone_combination = static_cast<::LPD3DXBONECOMBINATION>(
      mesh_container->bone_buffer_->GetBufferPointer());

  const ::DWORD dw_palette_size = mesh_container->palette_size_;

  for (::DWORD i{}; i < mesh_container->bone_count_; ++i) {
    for (::DWORD k{}; k < dw_palette_size; ++k) {
      ::DWORD dw_bone_id = bone_combination[i].BoneId[k];
      if (dw_bone_id == UINT_MAX) {
        continue;
      }
      world_matrix_array_[k] =
          mesh_container->bone_offset_matrices_[dw_bone_id]
              * (*mesh_container->frame_combined_matrix_[dw_bone_id]);
    }
    effect_->SetMatrixArray("g_world_matrix_array",
        &world_matrix_array_[0], dw_palette_size);

    ::DWORD bone_id = bone_combination[i].AttribId;
    ::D3DXVECTOR4 vec4_color{
        mesh_container->pMaterials[bone_id].MatD3D.Diffuse.r,
        mesh_container->pMaterials[bone_id].MatD3D.Diffuse.g,
        mesh_container->pMaterials[bone_id].MatD3D.Diffuse.b,
        mesh_container->pMaterials[bone_id].MatD3D.Diffuse.a
    };
    effect_->SetVector(diffuse_handle_, &vec4_color);
    effect_->SetTexture(mesh_texture_handle_,
        mesh_container->texture_.at(bone_id).get());

    effect_->Begin(nullptr, D3DXFX_DONOTSAVESTATE);

    if (FAILED(effect_->BeginPass(0))) {
      effect_->End();
      THROW_WITH_TRACE("Failed 'BeginPass' function.");
    }
    effect_->CommitChanges();
    mesh_container->MeshData.pMesh->DrawSubset(i);
    effect_->EndPass();
    effect_->End();
  }
}
::HRESULT skinned_animation_mesh::allocate_bone_matrix(
    ::LPD3DXMESHCONTAINER mesh_container)
{
  skinned_animation_mesh_frame *frame{};

  skinned_animation_mesh_container *skinned_mesh_container =
      static_cast<skinned_animation_mesh_container*>(mesh_container);

  ::DWORD bone_count = skinned_mesh_container->pSkinInfo->GetNumBones();
  skinned_mesh_container->frame_combined_matrix_.resize(bone_count);

  // TODO Improve.
  ::DWORD MAX_MATRICES = 26;
  world_matrix_array_.resize(std::min(MAX_MATRICES, bone_count));

  effect_->SetInt("current_bone_numbers",
      skinned_mesh_container->influence_count_ - 1);

  for (::DWORD i{}; i < bone_count; ++i) {
    ::LPD3DXFRAME p = ::D3DXFrameFind(frame_root_.get(),
        skinned_mesh_container->pSkinInfo->GetBoneName(i));

    frame = static_cast<skinned_animation_mesh_frame*>(p);

    if (frame == nullptr) {
      return E_FAIL;
    }
    LPD3DXMATRIX p_matrix = &frame->combined_matrix_;
    skinned_mesh_container->frame_combined_matrix_.at(i) = p_matrix;
  }
  return S_OK;
}
::HRESULT skinned_animation_mesh::allocate_all_bone_matrices(
    ::LPD3DXFRAME frame)
{
  if (frame->pMeshContainer != nullptr) {
    // TODO why frame->pMeshContainer->pNextMeshContainer is unnecessary?
    if (FAILED(allocate_bone_matrix(frame->pMeshContainer))) {
      return E_FAIL;
    }
  }
  if (frame->pFrameSibling != nullptr) {
    if (FAILED(allocate_all_bone_matrices(frame->pFrameSibling))) {
      return E_FAIL;
    }
  }
  if (frame->pFrameFirstChild != nullptr) {
    if (FAILED(allocate_all_bone_matrices(frame->pFrameFirstChild))) {
      return E_FAIL;
    }
  }
  return S_OK;
}
} /* namespace early_go */
