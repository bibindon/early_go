#include "animation_mesh.hpp"
#include "animation_mesh_allocator.hpp"
#include "basic_window.hpp"
#include "constants.hpp"
#include "exception.hpp"
#include "inline_macro.hpp"

namespace early_go {
/* A custom deleter. */
void animation_mesh::frame_root_deleter_object::operator()(
    const ::LPD3DXFRAME a_kp_d3dx_frame_root_)
{
  /* Call the recursive release function. */
  this->release_mesh_allocator(a_kp_d3dx_frame_root_);
}

/*
 * Releases recursively mesh containers owned by the 'animation_mesh_frame'
 * inheriting '::D3DXFRAME'.
 */
void animation_mesh::frame_root_deleter_object::release_mesh_allocator(
  const ::LPD3DXFRAME a_kp_frame)
{
  /*
   * References:
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
  if (a_kp_frame->pMeshContainer != nullptr) {
    this->sp_animation_mesh_allocator_->
        DestroyMeshContainer(a_kp_frame->pMeshContainer);
  }
  /* Call oneself. */
  if (a_kp_frame->pFrameSibling != nullptr) {
    this->release_mesh_allocator(a_kp_frame->pFrameSibling);
  }
  /* Call oneself. */
  if (a_kp_frame->pFrameFirstChild != nullptr) {
    this->release_mesh_allocator(a_kp_frame->pFrameFirstChild);
  }
  /* Release oneself. */
  this->sp_animation_mesh_allocator_->DestroyFrame(a_kp_frame);
}

/*
 * Reads a mesh file, and sets the frame and the animation controller given to
 * member variables.
 */
animation_mesh::animation_mesh(
    const std::shared_ptr<::IDirect3DDevice9>& a_krsp_direct3d_device9,
    const std::string& a_krsz_xfile_path,
    const ::D3DXVECTOR3& a_kp_vec_position)
    : b_play_animation_{true},
      f_animation_time_{},
      sp_direct3d_device9_{a_krsp_direct3d_device9},
      sp_animation_mesh_allocator_{
          new_crt animation_mesh_allocator{a_krsz_xfile_path}},
      up_d3dx_frame_root_{nullptr,
          frame_root_deleter_object{sp_animation_mesh_allocator_}},
      up_d3dx_animation_controller_{nullptr, custom_deleter{}},
      vec_position_{a_kp_vec_position},
      mat_rotation_{::D3DMATRIX{}},
      mat_world_{::D3DMATRIX{}}
{
  ::LPD3DXFRAME _p_temp_d3dx_frame_root{nullptr};
  ::LPD3DXANIMATIONCONTROLLER _p_temp_d3dx_animation_controller{nullptr};

  if (FAILED(
      ::D3DXLoadMeshHierarchyFromX(a_krsz_xfile_path.c_str(),
                                   ::D3DXMESH_MANAGED,
                                   this->sp_direct3d_device9_.get(),
                                   this->sp_animation_mesh_allocator_.get(),
                                   nullptr,
                                   &_p_temp_d3dx_frame_root,
                                   &_p_temp_d3dx_animation_controller))) {
    ::MessageBox(nullptr, constants::FAILED_TO_READ_X_FILE_MESSAGE.c_str(),
        a_krsz_xfile_path.c_str(), MB_OK);
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to load a x-file."});
  }
  /* lazy initialization */
  this->up_d3dx_frame_root_.reset(_p_temp_d3dx_frame_root);
  this->up_d3dx_animation_controller_.reset(_p_temp_d3dx_animation_controller);
}

/* Renders its own animation mesh. */
void animation_mesh::render()
{
  if (this->b_play_animation_) {
    this->f_animation_time_ += constants::ANIMATION_SPEED;
    this->up_d3dx_animation_controller_->AdvanceTime(
        constants::ANIMATION_SPEED, nullptr);
  }
  this->update_direct3d_device();
  this->update_frame_matrix(this->up_d3dx_frame_root_.get(), &this->mat_world_);
  this->render_frame(this->up_d3dx_frame_root_.get());

  std::stringstream ss{};
  ss << std::fixed << std::setprecision(2)
      << "animation time: " << this->f_animation_time_ << std::endl;
  basic_window::render_string_object::render_string(ss.str(), 10, 10);
}

bool animation_mesh::get_play_animation() const
{
  return this->b_play_animation_;
}

void animation_mesh::set_play_animation(const bool& a_krb_play_animation)
{
  this->b_play_animation_ = a_krb_play_animation;
}

float animation_mesh::get_animation_time() const
{
  return this->f_animation_time_;
}

void animation_mesh::update_direct3d_device()
{
  /* Set world transform matrix. */
  {
    ::D3DXMATRIXA16 _mat_world{};
    ::D3DXMATRIXA16 _mat_position{};
    ::D3DXMatrixIdentity(&_mat_world);
    ::D3DXMatrixTranslation(&_mat_position,
        this->vec_position_.x, this->vec_position_.y, this->vec_position_.z);

    ::D3DXMatrixMultiply(&this->mat_world_, &_mat_world, &_mat_position);
    this->sp_direct3d_device9_->SetTransform(D3DTS_WORLD, &this->mat_world_);
  }
  /* Set view transform matrix. */
  {
    ::D3DXVECTOR3 _vec_eye_position    { 4.0f, 4.0f, -2.5f};
    ::D3DXVECTOR3 _vec_look_at_position{-1.0f, 0.0f,  1.0f};
    ::D3DXVECTOR3 _vec_up_vector       { 0.0f, 1.0f,  0.0f};
    ::D3DXMATRIXA16 _mat_view{};
    ::D3DXMatrixLookAtLH(&_mat_view,
        &_vec_eye_position, &_vec_look_at_position, &_vec_up_vector);

    this->sp_direct3d_device9_->SetTransform(::D3DTS_VIEW, &_mat_view);
  }
  /* Set projection transform matrix. */
  {
    ::D3DXMATRIXA16 _mat_projection;
    ::D3DXMatrixPerspectiveFovLH(
        &_mat_projection,
        D3DX_PI / 4,
        static_cast<float>(constants::WINDOW_WIDTH) / constants::WINDOW_HEIGHT,
        0.1f,
        10.0f);

    this->sp_direct3d_device9_->SetTransform(::D3DTS_PROJECTION,
                                             &_mat_projection);
  }
}

/*
 * Updates a world-transformation-matrix each the mesh in the frame. Also, this
 * is a recursive function.
 */
void animation_mesh::update_frame_matrix(const ::LPD3DXFRAME a_kp_frame_base,
    const ::LPD3DXMATRIX a_kp_parent_matrix)
{
  animation_mesh_frame *_animation_mesh_frame{
      static_cast<animation_mesh_frame*>(a_kp_frame_base)};
  /*
   * Multiply its own transformation matrix by the parent transformation
   * matrix.
   */
  if (a_kp_parent_matrix != nullptr) {
    ::D3DXMatrixMultiply(
        &_animation_mesh_frame->combined_transformation_matrix_,
        &_animation_mesh_frame->TransformationMatrix, a_kp_parent_matrix);
  } else {
    _animation_mesh_frame->combined_transformation_matrix_ =
        _animation_mesh_frame->TransformationMatrix;
  }

  /* Call oneself. */
  if (_animation_mesh_frame->pFrameSibling != nullptr) {
    this->update_frame_matrix(_animation_mesh_frame->pFrameSibling,
        a_kp_parent_matrix);
  }
  /* Call oneself. */
  if (_animation_mesh_frame->pFrameFirstChild != nullptr) {
    this->update_frame_matrix(_animation_mesh_frame->pFrameFirstChild,
        &_animation_mesh_frame->combined_transformation_matrix_);
  }
}

/* Calls the 'render_mesh_container' function recursively. */
void animation_mesh::render_frame(const ::LPD3DXFRAME a_kp_frame)
{
  {
    ::LPD3DXMESHCONTAINER _p_d3dx_mesh_container{a_kp_frame->pMeshContainer};
    while (_p_d3dx_mesh_container != nullptr) {
      this->render_mesh_container(_p_d3dx_mesh_container, a_kp_frame);
      _p_d3dx_mesh_container = _p_d3dx_mesh_container->pNextMeshContainer;
    }
  }

  /* Call oneself. */
  if (a_kp_frame->pFrameSibling != nullptr) {
    this->render_frame(a_kp_frame->pFrameSibling);
  }
  /* Call oneself. */
  if (a_kp_frame->pFrameFirstChild != nullptr) {
    this->render_frame(a_kp_frame->pFrameFirstChild);
  }
}

void animation_mesh::render_mesh_container(
    const ::LPD3DXMESHCONTAINER a_kp_mesh_container_base,
    const ::LPD3DXFRAME a_kp_frame_base)
{
  /* Cast for making child class' function callable. */
  animation_mesh_frame *_p_frame{
      static_cast<animation_mesh_frame*>(a_kp_frame_base)};

  this->sp_direct3d_device9_->SetTransform(
      D3DTS_WORLD, &_p_frame->combined_transformation_matrix_);

  /* Cast for making child class' function callable. */
  animation_mesh_container *_p_mesh_container_base{
      static_cast<animation_mesh_container*>(a_kp_mesh_container_base)};
  /*
   * Set materials and textures, and draw materials as many as
   * '_p_mesh_container_base' has.
   */
  for (unsigned int i{}; i < _p_mesh_container_base->NumMaterials; ++i) {
    this->sp_direct3d_device9_->SetMaterial(
        &_p_mesh_container_base->pMaterials[i].MatD3D);
    this->sp_direct3d_device9_->SetTexture(0,
        _p_mesh_container_base->vecup_texture_.at(i).get());

    _p_mesh_container_base->MeshData.pMesh->DrawSubset(i);
  }
}
} /* namespace early_go */
