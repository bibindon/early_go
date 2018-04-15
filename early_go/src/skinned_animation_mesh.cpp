#include "stdafx.hpp"

#include "skinned_animation_mesh.hpp"
#include "skinned_animation_mesh_allocator.hpp"
#include "basic_window.hpp"

namespace early_go {
  const std::string skinned_animation_mesh::SHADER_FILENAME =
      "skinned_animation_mesh_shader.fx";

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
    const ::D3DXVECTOR3& a_kp_vec_position,
    const float& a_krf_size)
    : vecup_animation_set_{},
      b_play_animation_{true},
      f_animation_time_{},
      sp_direct3d_device9_{a_krsp_direct3d_device9},
      sp_skinned_animation_mesh_allocator_{
          new_crt skinned_animation_mesh_allocator{a_krsz_xfile_name}},
      up_d3dx_frame_root_{nullptr,
          frame_root_deleter_object{sp_skinned_animation_mesh_allocator_}},
      up_d3dx_animation_controller_{nullptr, custom_deleter{}},
      vec3_position_{a_kp_vec_position},
      mat_rotation_{::D3DMATRIX{}},
      up_d3dx_effect_{nullptr, custom_deleter{}},
      d3dx_handle_view_projection_{},
      d3dx_handle_light_normal_{},
      d3dx_handle_brightness_{},
      d3dx_handle_scale_{},
      d3dx_handle_texture_{},
      d3dx_handle_diffuse_{}
{
  ::HRESULT hresult{};

  std::vector<char> vecc_buffer = get_resource(
      "select data from shader_file where filename = '"
      + this->SHADER_FILENAME + "';");
  ::LPD3DXEFFECT temp_p_d3dx_effect{};
  ::D3DXCreateEffect(a_krsp_direct3d_device9.get(),
                     &vecc_buffer[0],
                     static_cast<::UINT>(vecc_buffer.size()),
                     nullptr,
                     nullptr,
                     0,
                     nullptr,
                     &temp_p_d3dx_effect,
                     nullptr);
  this->up_d3dx_effect_.reset(temp_p_d3dx_effect);
  if (FAILED(hresult)) {
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to create an effect file."});
  }

  this->d3dx_handle_view_projection_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "hlsl_view_projection");
  this->d3dx_handle_light_normal_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "hlsl_light_normal");

  this->d3dx_handle_brightness_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "hlsl_light_brightness");
  this->d3dx_handle_texture_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "hlsl_texture");
  this->d3dx_handle_diffuse_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "hlsl_diffuse");

  ::LPD3DXFRAME p_temp_d3dx_frame_root{nullptr};
  ::LPD3DXANIMATIONCONTROLLER p_temp_d3dx_animation_controller{nullptr};

  vecc_buffer = get_resource(
      "select data from x_file where filename = '" + a_krsz_xfile_name + "';");
  if (FAILED(::D3DXLoadMeshHierarchyFromXInMemory(
      &vecc_buffer[0],
      static_cast<DWORD>(vecc_buffer.size()),
      ::D3DXMESH_MANAGED,
      this->sp_direct3d_device9_.get(),
      this->sp_skinned_animation_mesh_allocator_.get(),
      nullptr,
      &p_temp_d3dx_frame_root,
      &p_temp_d3dx_animation_controller))) {
    ::MessageBox(nullptr, constants::FAILED_TO_READ_X_FILE_MESSAGE.c_str(),
        a_krsz_xfile_name.c_str(), MB_OK);
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to load a x-file."});
  }
  /* lazy initialization */
  this->up_d3dx_frame_root_.reset(p_temp_d3dx_frame_root);
  this->up_d3dx_animation_controller_.reset(p_temp_d3dx_animation_controller);
  this->allocate_all_bone_matrices(this->up_d3dx_frame_root_.get());

  ::DWORD dw_animation_number{
      this->up_d3dx_animation_controller_->GetNumAnimationSets()};

  std::vector<std::unique_ptr<::ID3DXAnimationSet, custom_deleter> >
      temp_vecup(dw_animation_number);

  this->vecup_animation_set_.swap(temp_vecup);

  for (::DWORD i{}; i < dw_animation_number; ++i) {
    ::LPD3DXANIMATIONSET p_temp{};
    this->up_d3dx_animation_controller_->GetAnimationSet(i, &p_temp);
    this->vecup_animation_set_.at(i).reset(p_temp);
  }

  ::D3DXFrameCalculateBoundingSphere(this->up_d3dx_frame_root_.get(),
                                     &this->vec3_center_coodinate_,
                                     &this->f_radius_);
  this->f_scale_ = a_krf_size / this->f_radius_;
}

void skinned_animation_mesh::play_animation_set(
    const std::size_t& a_kr_animation_set)
{
  if (a_kr_animation_set >= this->vecup_animation_set_.size()) {
    BOOST_THROW_EXCEPTION(
        custom_exception{"An illegal animation set was sent."});
  }
  this->up_d3dx_animation_controller_->SetTrackAnimationSet(
      0, this->vecup_animation_set_.at(a_kr_animation_set).get());
}

void skinned_animation_mesh::play_animation_set(
    const std::string& a_kr_animation_set)
{
  std::vector<
      std::unique_ptr<
          ::ID3DXAnimationSet, custom_deleter
      >
  >::const_iterator kit;

  kit = std::find_if(
      this->vecup_animation_set_.cbegin(),
      this->vecup_animation_set_.cend(),
      [&](const std::unique_ptr<::ID3DXAnimationSet, custom_deleter>& a){
        return a_kr_animation_set == a->GetName();
  });

  if (this->vecup_animation_set_.cend() == kit) {
    BOOST_THROW_EXCEPTION(
        custom_exception{"An illegal animation set was sent."});
  }

  this->up_d3dx_animation_controller_->SetTrackAnimationSet(0, kit->get());
}

/* Renders its own animation mesh. */
void skinned_animation_mesh::render(const ::D3DXMATRIX& a_kr_mat_view,
                                    const ::D3DXMATRIX& a_kr_mat_projection,
                                    const ::D3DXVECTOR4 & a_kr_normal_light,
                                    const float& a_kr_brightness)
{
  ::D3DXMATRIX mat_view_projection{a_kr_mat_view * a_kr_mat_projection};

  this->up_d3dx_effect_->SetMatrix(
      this->d3dx_handle_view_projection_, &mat_view_projection);

  this->up_d3dx_effect_->SetVector(
      this->d3dx_handle_light_normal_, &a_kr_normal_light);

  this->up_d3dx_effect_->SetFloat(
      this->d3dx_handle_brightness_, a_kr_brightness);

  if (this->b_play_animation_) {
    this->f_animation_time_ += constants::ANIMATION_SPEED;
    this->up_d3dx_animation_controller_->AdvanceTime(
        constants::ANIMATION_SPEED, nullptr);
  }

  ::D3DXMATRIX mat_world{};
  ::D3DXMatrixIdentity(&mat_world);
  {
    ::D3DXMATRIX mat{};
    ::D3DXMatrixTranslation(&mat,
                            -this->vec3_center_coodinate_.x,
                            -this->vec3_center_coodinate_.y,
                            -this->vec3_center_coodinate_.z);
    mat_world *= mat;

    ::D3DXMatrixScaling(&mat, this->f_scale_, this->f_scale_, this->f_scale_);
    mat_world *= mat;

    ::D3DXMatrixTranslation(&mat,
                            this->vec3_position_.x,
                            this->vec3_position_.y,
                            this->vec3_position_.z);
    mat_world *= mat;
  }

  this->update_frame_matrix(this->up_d3dx_frame_root_.get(), &mat_world);
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
  skinned_animation_mesh_frame *p_skinned_animation_mesh_frame{
      static_cast<skinned_animation_mesh_frame*>(a_kp_frame_base)};
  /*
   * Multiply its own transformation matrix by the parent transformation
   * matrix.
   */
  if (a_kp_parent_matrix != nullptr) {
    p_skinned_animation_mesh_frame->combined_transformation_matrix_ =
        p_skinned_animation_mesh_frame->TransformationMatrix
            * (*a_kp_parent_matrix);
  } else {
    p_skinned_animation_mesh_frame->combined_transformation_matrix_ =
        p_skinned_animation_mesh_frame->TransformationMatrix;
  }

  /* Call oneself. */
  if (p_skinned_animation_mesh_frame->pFrameSibling != nullptr) {
    this->update_frame_matrix(p_skinned_animation_mesh_frame->pFrameSibling,
        a_kp_parent_matrix);
  }
  /* Call oneself. */
  if (p_skinned_animation_mesh_frame->pFrameFirstChild != nullptr) {
    this->update_frame_matrix(p_skinned_animation_mesh_frame->pFrameFirstChild,
        &p_skinned_animation_mesh_frame->combined_transformation_matrix_);
  }
}

/* Calls the 'render_mesh_container' function recursively. */
void skinned_animation_mesh::render_frame(const ::LPD3DXFRAME a_kp_frame)
{
  {
    ::LPD3DXMESHCONTAINER p_d3dx_mesh_container{a_kp_frame->pMeshContainer};
    while (p_d3dx_mesh_container != nullptr) {
      this->render_mesh_container(p_d3dx_mesh_container);
      p_d3dx_mesh_container = p_d3dx_mesh_container->pNextMeshContainer;
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

  skinned_animation_mesh_container *p_mesh_container{
      static_cast<skinned_animation_mesh_container*>(a_kp_mesh_container_base)};

  ::LPD3DXBONECOMBINATION p_bone_combination{};

  p_bone_combination =
      static_cast<::LPD3DXBONECOMBINATION>(
          p_mesh_container->up_bone_buffer_->GetBufferPointer());

  const ::DWORD dw_palette_size = p_mesh_container->dw_palette_size_;

  for (::DWORD i{}; i < p_mesh_container->dw_bone_amount_; ++i) {
    for (::DWORD k{}; k < dw_palette_size; ++k) {
      ::DWORD dw_bone_id = p_bone_combination[i].BoneId[k];
      if (dw_bone_id == UINT_MAX) {
        continue;
      }
      vecmat_world_matrix_array_[k] =
          p_mesh_container->vec_bone_offset_matrices_[dw_bone_id]
              * (*p_mesh_container->vecp_frame_combined_matrix_[dw_bone_id]);
    }
    this->up_d3dx_effect_->SetMatrixArray("hlsl_world_matrix_array",
        &vecmat_world_matrix_array_[0], dw_palette_size);

    ::DWORD bone_id = p_bone_combination[i].AttribId;
    ::D3DXVECTOR4 vec4_color{
        p_mesh_container->pMaterials[bone_id].MatD3D.Diffuse.r,
        p_mesh_container->pMaterials[bone_id].MatD3D.Diffuse.g,
        p_mesh_container->pMaterials[bone_id].MatD3D.Diffuse.b,
        p_mesh_container->pMaterials[bone_id].MatD3D.Diffuse.a
    };
    this->up_d3dx_effect_->SetVector(this->d3dx_handle_diffuse_, &vec4_color);
    this->up_d3dx_effect_->SetTexture(this->d3dx_handle_texture_,
        p_mesh_container->vecup_texture_.at(bone_id).get());

    this->up_d3dx_effect_->Begin(nullptr, D3DXFX_DONOTSAVESTATE);

    if (FAILED(this->up_d3dx_effect_->BeginPass(0))) {
      this->up_d3dx_effect_->End();
      BOOST_THROW_EXCEPTION(custom_exception{"Failed 'BeginPass' function."});
    }
    this->up_d3dx_effect_->CommitChanges();
    p_mesh_container->MeshData.pMesh->DrawSubset(i);
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
  this->vecmat_world_matrix_array_.resize(min(MAX_MATRICES, dw_bone_amount));

  this->up_d3dx_effect_->SetInt("current_bone_numbers",
      p_mesh_container->dw_influence_number_ - 1);

  for (::DWORD i{}; i < dw_bone_amount; ++i) {
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
