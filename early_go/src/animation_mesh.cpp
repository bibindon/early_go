#include "stdafx.hpp"

#include "animation_mesh.hpp"
#include "animation_mesh_allocator.hpp"
#include "basic_window.hpp"

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
    const std::string& a_krsz_xfile_name,
    const ::D3DXVECTOR3& a_kp_vec_position)
    : b_play_animation_{true},
      f_animation_time_{},
      sp_direct3d_device9_{a_krsp_direct3d_device9},
      sp_animation_mesh_allocator_{
          new_crt animation_mesh_allocator{a_krsz_xfile_name}},
      up_d3dx_frame_root_{nullptr,
          frame_root_deleter_object{sp_animation_mesh_allocator_}},
      up_d3dx_animation_controller_{nullptr, custom_deleter{}},
      vec_position_{a_kp_vec_position},
      mat_rotation_{::D3DMATRIX{}},
      up_d3dx_effect_{nullptr, custom_deleter{}},
      d3dx_handle_world_view_proj_{},
      d3dx_handle_light_position_{},
      d3dx_handle_brightness_{},
      d3dx_handle_scale_{},
      d3dx_handle_texture_{},
      d3dx_handle_diffuse_{}
{
  ::HRESULT _hresult{};

  std::vector<char> _data = get_resource(
      "select data from shader_file where filename = '"
      + constants::MESH_HLSL + "';");
  ::LPD3DXEFFECT _temp_p_d3dx_effect{};
  ::D3DXCreateEffect(a_krsp_direct3d_device9.get(),
                     &_data[0],
                     static_cast<::UINT>(_data.size()),
                     nullptr,
                     nullptr,
                     0,
                     nullptr,
                     &_temp_p_d3dx_effect,
                     nullptr);
  this->up_d3dx_effect_.reset(_temp_p_d3dx_effect);
  if (FAILED(_hresult)) {
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to create an effect file."});
  }

  this->d3dx_handle_world_view_proj_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "hlsl_world_view_projection");
  this->d3dx_handle_light_position_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "hlsl_light_position");
  this->d3dx_handle_brightness_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "hlsl_light_brightness");
  this->d3dx_handle_scale_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "hlsl_model_scale");
  this->d3dx_handle_texture_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "hlsl_texture");
  this->d3dx_handle_diffuse_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "hlsl_diffuse");

  ::LPD3DXFRAME _p_temp_d3dx_frame_root{nullptr};
  ::LPD3DXANIMATIONCONTROLLER _p_temp_d3dx_animation_controller{nullptr};

  _data = get_resource(
      "select data from x_file where filename = '" + a_krsz_xfile_name + "';");
  if (FAILED(::D3DXLoadMeshHierarchyFromXInMemory(
      &_data[0],
      static_cast<DWORD>(_data.size()),
      ::D3DXMESH_MANAGED,
      this->sp_direct3d_device9_.get(),
      this->sp_animation_mesh_allocator_.get(),
      nullptr,
      &_p_temp_d3dx_frame_root,
      &_p_temp_d3dx_animation_controller))) {
    ::MessageBox(nullptr, constants::FAILED_TO_READ_X_FILE_MESSAGE.c_str(),
        a_krsz_xfile_name.c_str(), MB_OK);
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to load a x-file."});
  }
  /* lazy initialization */
  this->up_d3dx_frame_root_.reset(_p_temp_d3dx_frame_root);
  this->up_d3dx_animation_controller_.reset(_p_temp_d3dx_animation_controller);
}

/* Renders its own animation mesh. */
void animation_mesh::render(const ::D3DXMATRIX& a_kr_mat_view,
                            const ::D3DXMATRIX& a_kr_mat_projection,
                            const ::D3DXVECTOR3& a_kr_light_position,
                            const float& a_kr_brightness)
{
  this->mat_view_ = a_kr_mat_view;
  this->mat_projection_ = a_kr_mat_projection;

  ::D3DXVECTOR4 _light_position{a_kr_light_position, 1.0f};
  this->up_d3dx_effect_->SetVector(
      this->d3dx_handle_light_position_, &_light_position);
  this->up_d3dx_effect_->SetFloat(
      this->d3dx_handle_brightness_, a_kr_brightness);

  if (this->b_play_animation_) {
    this->f_animation_time_ += constants::ANIMATION_SPEED;
    this->up_d3dx_animation_controller_->AdvanceTime(
        constants::ANIMATION_SPEED, nullptr);
  }

  ::D3DXMATRIX _mat_world{};
  ::D3DXMatrixIdentity(&_mat_world);
  ::D3DXMatrixTranslation(&_mat_world,
      this->vec_position_.x, this->vec_position_.y, this->vec_position_.z);
  this->update_frame_matrix(this->up_d3dx_frame_root_.get(), &_mat_world);
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

  ::D3DXMATRIX _matWorldViewProj;
  ::D3DXMatrixIdentity(&_matWorldViewProj);

  _matWorldViewProj *= _p_frame->combined_transformation_matrix_;
  _matWorldViewProj *= this->mat_view_;
  _matWorldViewProj *= this->mat_projection_;

  this->up_d3dx_effect_->SetMatrix(this->d3dx_handle_world_view_proj_,
                                   &_matWorldViewProj);

  this->up_d3dx_effect_->SetFloat(this->d3dx_handle_scale_, 1.0f);

  this->up_d3dx_effect_->Begin(nullptr, 0);

  if (FAILED(this->up_d3dx_effect_->BeginPass(0))) {
    this->up_d3dx_effect_->End();
    BOOST_THROW_EXCEPTION(custom_exception{"Failed 'BeginPass' function."});
  }

  animation_mesh_container *_p_mesh_container_base{
      static_cast<animation_mesh_container*>(a_kp_mesh_container_base)};

  for (::DWORD i{}; i < _p_mesh_container_base->NumMaterials; ++i) {
    ::D3DXVECTOR4 _color{_p_mesh_container_base->pMaterials[i].MatD3D.Diffuse.r,
                         _p_mesh_container_base->pMaterials[i].MatD3D.Diffuse.g,
                         _p_mesh_container_base->pMaterials[i].MatD3D.Diffuse.b,
                         1.0f };
                         //this->vec_d3d_color_.at(i).a };
    this->up_d3dx_effect_->SetVector(this->d3dx_handle_diffuse_, &_color);
    this->up_d3dx_effect_->SetTexture(this->d3dx_handle_texture_,
                                      _p_mesh_container_base->vecup_texture_.at(i).get());
    this->up_d3dx_effect_->CommitChanges();
    _p_mesh_container_base->MeshData.pMesh->DrawSubset(i);
  }
  this->up_d3dx_effect_->EndPass();
  this->up_d3dx_effect_->End();
}
} /* namespace early_go */
