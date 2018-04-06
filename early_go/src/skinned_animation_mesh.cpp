#include "stdafx.hpp"

#include "skinned_animation_mesh.hpp"
#include "skinned_animation_mesh_allocator.hpp"
#include "basic_window.hpp"

namespace early_go {
// TODO Improve. attention to memory leak.
  std::vector<::LPD3DXANIMATIONSET> vecp_animation_set;
  const std::string skinned_animation_mesh::SKINNED_MESH_HLSL =
      "skinned_mesh_shader.fx";

/* A custom deleter. */
void skinned_animation_mesh::frame_root_deleter_object::operator()(
    const ::LPD3DXFRAME a_kp_d3dx_frame_root_)
{
  /* Call the recursive release function. */
  this->release_mesh_allocator(a_kp_d3dx_frame_root_);
}

/*
 * Releases recursively mesh containers owned by the
 * 'skinned_animation_mesh_frame' inheriting '::D3DXFRAME'.
 */
void skinned_animation_mesh::frame_root_deleter_object::release_mesh_allocator(
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
    this->sp_skinned_animation_mesh_allocator_->
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
  this->sp_skinned_animation_mesh_allocator_->DestroyFrame(a_kp_frame);
}

/*
 * Reads a mesh file, and sets the frame and the animation controller given to
 * member variables.
 */
skinned_animation_mesh::skinned_animation_mesh(
    const std::shared_ptr<::IDirect3DDevice9>& a_krsp_direct3d_device9,
    const std::string& a_krsz_xfile_name,
    const ::D3DXVECTOR3& a_kp_vec_position)
    : b_play_animation_{true},
      f_animation_time_{},
      sp_direct3d_device9_{a_krsp_direct3d_device9},
      sp_skinned_animation_mesh_allocator_{
          new_crt skinned_animation_mesh_allocator{a_krsz_xfile_name}},
      up_d3dx_frame_root_{nullptr,
          frame_root_deleter_object{sp_skinned_animation_mesh_allocator_}},
      up_d3dx_animation_controller_{nullptr, custom_deleter{}},
      vec_position_{a_kp_vec_position},
      mat_rotation_{::D3DMATRIX{}},
      up_d3dx_effect_{nullptr, custom_deleter{}},
      d3dx_handle_projection_{},
      d3dx_handle_light_position_{},
      d3dx_handle_brightness_{},
      d3dx_handle_scale_{},
      d3dx_handle_texture_{},
      d3dx_handle_diffuse_{}
{
  ::HRESULT _hresult{};

  std::vector<char> _data = get_resource(
      "select data from shader_file where filename = '"
      + this->SKINNED_MESH_HLSL + "';");
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

  this->d3dx_handle_projection_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "hlsl_projection");
  this->d3dx_handle_light_position_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "hlsl_light_position");
  this->d3dx_handle_brightness_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "hlsl_light_brightness");
// TODO Improve
//  this->d3dx_handle_scale_ =
//      this->up_d3dx_effect_->GetParameterByName(nullptr,
//                                                "hlsl_model_scale");
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
      this->sp_skinned_animation_mesh_allocator_.get(),
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
  this->allocate_all_bone_matrices(this->up_d3dx_frame_root_.get());

  /* TODO Improve ~ */
  vecp_animation_set.resize(
      this->up_d3dx_animation_controller_->GetNumAnimationSets());
  for (::DWORD i{};
      i < this->up_d3dx_animation_controller_->GetNumAnimationSets(); ++i) {
    up_d3dx_animation_controller_->GetAnimationSet(
        i, &vecp_animation_set.at(i));
  }

  D3DXTRACK_DESC track_desc{};

  track_desc.Weight = 1;
  track_desc.Speed  = 1;
  track_desc.Enable = 1;
  up_d3dx_animation_controller_->SetTrackDesc(3, &track_desc);
  up_d3dx_animation_controller_->SetTrackAnimationSet(
      0, vecp_animation_set.at(3));
  /* ~ TODO Improve */
}

/* Renders its own animation mesh. */
void skinned_animation_mesh::render(const ::D3DXMATRIX& a_kr_mat_view,
                                    const ::D3DXMATRIX& a_kr_mat_projection,
                                    const ::D3DXVECTOR3& a_kr_light_position,
                                    const float& a_kr_brightness)
{
  this->mat_view_ = a_kr_mat_view;

  this->up_d3dx_effect_->SetMatrix(this->d3dx_handle_projection_,
                                   &a_kr_mat_projection);

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
  ::D3DXMatrixTranslation(&_mat_world,
      this->vec_position_.x, this->vec_position_.y, this->vec_position_.z);
  this->update_frame_matrix(this->up_d3dx_frame_root_.get(), &_mat_world);
  this->render_frame(this->up_d3dx_frame_root_.get());

  std::stringstream ss{};
  ss << std::fixed << std::setprecision(2)
      << "animation time: " << this->f_animation_time_ << std::endl;
  basic_window::render_string_object::render_string(ss.str(), 10, 10);
}

bool skinned_animation_mesh::get_play_animation() const
{
  return this->b_play_animation_;
}

void skinned_animation_mesh::set_play_animation(
    const bool& a_krb_play_animation)
{
  this->b_play_animation_ = a_krb_play_animation;
}

float skinned_animation_mesh::get_animation_time() const
{
  return this->f_animation_time_;
}

/*
 * Updates a world-transformation-matrix each the mesh in the frame. Also, this
 * is a recursive function.
 */
void skinned_animation_mesh::update_frame_matrix(
    const ::LPD3DXFRAME a_kp_frame_base,
    const ::LPD3DXMATRIX a_kp_parent_matrix)
{
  skinned_animation_mesh_frame *_skinned_animation_mesh_frame{
      static_cast<skinned_animation_mesh_frame*>(a_kp_frame_base)};
  /*
   * Multiply its own transformation matrix by the parent transformation
   * matrix.
   */
  if (a_kp_parent_matrix != nullptr) {
    _skinned_animation_mesh_frame->combined_transformation_matrix_ =
        _skinned_animation_mesh_frame->TransformationMatrix
            * (*a_kp_parent_matrix);
  } else {
    _skinned_animation_mesh_frame->combined_transformation_matrix_ =
        _skinned_animation_mesh_frame->TransformationMatrix;
  }

  /* Call oneself. */
  if (_skinned_animation_mesh_frame->pFrameSibling != nullptr) {
    this->update_frame_matrix(_skinned_animation_mesh_frame->pFrameSibling,
        a_kp_parent_matrix);
  }
  /* Call oneself. */
  if (_skinned_animation_mesh_frame->pFrameFirstChild != nullptr) {
    this->update_frame_matrix(_skinned_animation_mesh_frame->pFrameFirstChild,
        &_skinned_animation_mesh_frame->combined_transformation_matrix_);
  }
}

/* Calls the 'render_mesh_container' function recursively. */
void skinned_animation_mesh::render_frame(const ::LPD3DXFRAME a_kp_frame)
{
  {
    ::LPD3DXMESHCONTAINER _p_d3dx_mesh_container{a_kp_frame->pMeshContainer};
    while (_p_d3dx_mesh_container != nullptr) {
      this->render_mesh_container(_p_d3dx_mesh_container);
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

void skinned_animation_mesh::render_mesh_container(
    const ::LPD3DXMESHCONTAINER a_kp_mesh_container_base)
{
// TODO later.
//  this->up_d3dx_effect_->SetFloat(this->d3dx_handle_scale_, 1.0f);

  skinned_animation_mesh_container *_p_mesh_container{
      static_cast<skinned_animation_mesh_container*>(a_kp_mesh_container_base)};

  ::LPD3DXBONECOMBINATION p_bone_combination{};

  p_bone_combination =
      static_cast<::LPD3DXBONECOMBINATION>(
          _p_mesh_container->up_bone_buffer_->GetBufferPointer());

  const ::DWORD dw_palette_size = _p_mesh_container->dw_palette_size_;

  for (::DWORD i{}; i < _p_mesh_container->dw_bone_amount_; i++) {
    for (::DWORD k{}; k < dw_palette_size; ++k) {
      ::DWORD dw_bone_id = p_bone_combination[i].BoneId[k];
      if (dw_bone_id == UINT_MAX) {
        continue;
      }
      ::D3DXMATRIX mat =
          _p_mesh_container->vec_bone_offset_matrices_[dw_bone_id]
              * (*_p_mesh_container->vecp_frame_combined_matrix_[dw_bone_id]);

      vecmat_world_matrix_array[k] = mat * this->mat_view_;
    }
    this->up_d3dx_effect_->SetMatrixArray("hlsl_world_matrix_array",
        &vecmat_world_matrix_array[0], dw_palette_size);

    ::DWORD bone_id = p_bone_combination[i].AttribId;
    ::D3DXVECTOR4 _color{
        _p_mesh_container->pMaterials[bone_id].MatD3D.Diffuse.r,
        _p_mesh_container->pMaterials[bone_id].MatD3D.Diffuse.g,
        _p_mesh_container->pMaterials[bone_id].MatD3D.Diffuse.b,
        _p_mesh_container->pMaterials[bone_id].MatD3D.Diffuse.a
    };
    this->up_d3dx_effect_->SetVector(this->d3dx_handle_diffuse_, &_color);
    this->up_d3dx_effect_->SetTexture(this->d3dx_handle_texture_,
        _p_mesh_container->vecup_texture_.at(bone_id).get());

    this->up_d3dx_effect_->Begin(nullptr, D3DXFX_DONOTSAVESTATE);

    if (FAILED(this->up_d3dx_effect_->BeginPass(0))) {
      this->up_d3dx_effect_->End();
      BOOST_THROW_EXCEPTION(custom_exception{"Failed 'BeginPass' function."});
    }
    this->up_d3dx_effect_->CommitChanges();
    _p_mesh_container->MeshData.pMesh->DrawSubset(i);
    this->up_d3dx_effect_->EndPass();
    this->up_d3dx_effect_->End();
  }
}
::HRESULT skinned_animation_mesh::allocate_bone_matrix(
    ::LPD3DXMESHCONTAINER a_p_mesh_container)
{
  skinned_animation_mesh_frame *p_frame{};

  skinned_animation_mesh_container *p_mesh_container =
      static_cast<skinned_animation_mesh_container*>(a_p_mesh_container);

  ::DWORD dw_bone_amount = p_mesh_container->pSkinInfo->GetNumBones();
  p_mesh_container->vecp_frame_combined_matrix_.resize(dw_bone_amount);

  // TODO Improve.
  ::UINT MAX_MATRICES = 26;
  this->vecmat_world_matrix_array.resize(min(MAX_MATRICES, dw_bone_amount));

  this->up_d3dx_effect_->SetInt("current_bone_numbers",
      p_mesh_container->dw_influence_number_ - 1);

  for (::DWORD i = 0; i < dw_bone_amount; ++i) {
    ::LPD3DXFRAME p = ::D3DXFrameFind(this->up_d3dx_frame_root_.get(),
        p_mesh_container->pSkinInfo->GetBoneName(i));

    p_frame = static_cast<skinned_animation_mesh_frame*>(p);

    if (p_frame == nullptr) {
      return E_FAIL;
    }
    LPD3DXMATRIX p_matrix = &p_frame->combined_transformation_matrix_;
    p_mesh_container->vecp_frame_combined_matrix_.at(i) = p_matrix;
  }
  return S_OK;
}
::HRESULT skinned_animation_mesh::allocate_all_bone_matrices(
    ::LPD3DXFRAME a_p_frame)
{
  if (a_p_frame->pMeshContainer != nullptr) {
    // TODO why a_p_frame->pMeshContainer->pNextMeshContainer is unnecessary?
    if (FAILED(allocate_bone_matrix(a_p_frame->pMeshContainer))) {
      return E_FAIL;
    }
  }
  if (a_p_frame->pFrameSibling != nullptr) {
    if (FAILED(allocate_all_bone_matrices(a_p_frame->pFrameSibling))) {
      return E_FAIL;
    }
  }
  if (a_p_frame->pFrameFirstChild != nullptr) {
    if (FAILED(allocate_all_bone_matrices(a_p_frame->pFrameFirstChild))) {
      return E_FAIL;
    }
  }
  return S_OK;
}
} /* namespace early_go */
