#include "stdafx.hpp"

#include "animation_mesh.hpp"
#include "animation_mesh_allocator.hpp"
#include "basic_window.hpp"

namespace early_go {
const std::string animation_mesh::SHADER_FILENAME = "animation_mesh_shader.fx";
/* A custom deleter. */
void animation_mesh::frame_root_deleter_object::operator()(
    const ::LPD3DXFRAME frame_root)
{
  /* Call the recursive release function. */
  release_mesh_allocator(frame_root);
}

/*
 * Releases recursively mesh containers owned by the 'animation_mesh_frame'
 * inheriting '::D3DXFRAME'.
 */
void animation_mesh::frame_root_deleter_object::release_mesh_allocator(
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
animation_mesh::animation_mesh(
    const std::shared_ptr<::IDirect3DDevice9>& d3d_device,
    const std::string& x_filename,
    const ::D3DXVECTOR3& position,
    const float& scale)
    : base_mesh{d3d_device, SHADER_FILENAME, position},
      is_animated_{true},
      animation_time_{},
      d3d_device_{d3d_device},
      allocator_{new_crt animation_mesh_allocator{x_filename}},
      frame_root_{nullptr, frame_root_deleter_object{allocator_}},
      rotation_matrix_{::D3DMATRIX{}},
      center_coodinate_{},
      scale_{},
      world_handle_{},
      world_view_proj_handle_{}
{
  world_handle_ = effect_->GetParameterByName(nullptr, "g_world");
  world_view_proj_handle_ =
      effect_->GetParameterByName(nullptr, "g_world_view_projection");

  ::LPD3DXFRAME temp_root_frame{nullptr};
  ::LPD3DXANIMATIONCONTROLLER temp_animation_controller{nullptr};

  std::vector<char> buffer = get_resource(
      "SELECT DATA FROM MODEL WHERE FILENAME = '" + x_filename + "';");

  HRESULT result{::D3DXLoadMeshHierarchyFromXInMemory(
      &buffer[0],
      static_cast<::DWORD>(buffer.size()),
      ::D3DXMESH_MANAGED,
      d3d_device_.get(),
      allocator_.get(),
      nullptr,
      &temp_root_frame,
      &temp_animation_controller)};

  if (FAILED(result)) {
    THROW_WITH_TRACE("Failed to load a x-file.");
  }
  /* lazy initialization */
  frame_root_.reset(temp_root_frame);
  animation_strategy_.reset(
      new_crt normal_animation{temp_animation_controller});

  scale_ = scale;
}

/* Renders its own animation mesh. */
void animation_mesh::do_render(const ::D3DXMATRIX& view_matrix,
                               const ::D3DXMATRIX& projection_matrix)
{
  view_matrix_ = view_matrix;
  projection_matrix_ = projection_matrix;

  if (is_animated_) {
    animation_time_ += constants::ANIMATION_SPEED;
    animation_strategy_->animation_controller_->AdvanceTime(
        constants::ANIMATION_SPEED, nullptr);
  }

  ::D3DXMATRIX world_matrix{};
  ::D3DXMatrixIdentity(&world_matrix);
  {
    ::D3DXMATRIX mat{};
    ::D3DXMatrixTranslation(&mat,
        -center_coodinate_.x, -center_coodinate_.y, -center_coodinate_.z);
    world_matrix *= mat;

    ::D3DXMatrixScaling(&mat, scale_, scale_, scale_);
    world_matrix *= mat;

    ::D3DXMatrixTranslation(&mat, position_.x, position_.y, position_.z);
    world_matrix *= mat;
  }

  update_frame_matrix(frame_root_.get(), &world_matrix);
  render_frame(frame_root_.get());

  std::stringstream ss{};
  ss << std::fixed << std::setprecision(2)
      << "animation time: " << animation_time_ << std::endl;
  basic_window::render_string_object::render_string(ss.str(), 10, 10);
}

bool animation_mesh::get_play_animation() const
{
  return is_animated_;
}

void animation_mesh::set_play_animation(const bool& is_animated)
{
  is_animated_ = is_animated;
}

float animation_mesh::get_animation_time() const
{
  return animation_time_;
}

/*
 * Updates a world-transformation-matrix each the mesh in the frame. Also, this
 * is a recursive function.
 */
void animation_mesh::update_frame_matrix(const ::LPD3DXFRAME  frame_base,
                                         const ::LPD3DXMATRIX parent_matrix)
{
  animation_mesh_frame *frame{static_cast<animation_mesh_frame*>(frame_base)};
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
void animation_mesh::render_frame(const ::LPD3DXFRAME frame)
{
  {
    ::LPD3DXMESHCONTAINER mesh_container{frame->pMeshContainer};
    while (mesh_container != nullptr) {
      render_mesh_container(mesh_container, frame);
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

void animation_mesh::render_mesh_container(
    const ::LPD3DXMESHCONTAINER mesh_container_base,
    const ::LPD3DXFRAME         frame_base)
{
  /* Cast for making child class' function callable. */
  animation_mesh_frame *frame{static_cast<animation_mesh_frame*>(frame_base)};

  ::D3DXMATRIX world_view_projection_matrix{frame->combined_matrix_};

  effect_->SetMatrix(world_handle_, &world_view_projection_matrix);

  world_view_projection_matrix *= view_matrix_;
  world_view_projection_matrix *= projection_matrix_;

  effect_->SetMatrix(world_view_proj_handle_, &world_view_projection_matrix);

  effect_->Begin(nullptr, 0);

  if (FAILED(effect_->BeginPass(0))) {
    effect_->End();
    THROW_WITH_TRACE("Failed 'BeginPass' function.");
  }

  animation_mesh_container *mesh_container{
      static_cast<animation_mesh_container*>(mesh_container_base)};

  for (::DWORD i{}; i < mesh_container->NumMaterials; ++i) {
    ::D3DXVECTOR4 color{mesh_container->pMaterials[i].MatD3D.Diffuse.r,
                        mesh_container->pMaterials[i].MatD3D.Diffuse.g,
                        mesh_container->pMaterials[i].MatD3D.Diffuse.b,
                        mesh_container->pMaterials[i].MatD3D.Diffuse.a};
    effect_->SetVector(diffuse_handle_, &color);
    effect_->SetTexture(mesh_texture_handle_,
        mesh_container->texture_.at(i).get());

    effect_->CommitChanges();
    mesh_container->MeshData.pMesh->DrawSubset(i);
  }
  effect_->EndPass();
  effect_->End();
}
} /* namespace early_go */
