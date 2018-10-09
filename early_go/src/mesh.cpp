#include "stdafx.hpp"

#include "mesh.hpp"

namespace early_go {

const std::string mesh::SHADER_FILENAME = "mesh_shader.fx";

mesh::mesh(
    const std::shared_ptr<::IDirect3DDevice9>& a_krsp_direct3d_device9,
    const std::string& a_krsz_xfile_name,
    const ::D3DXVECTOR3& a_kp_vec_position,
    const float& a_krf_size)
    : base_mesh{a_krsp_direct3d_device9, SHADER_FILENAME, a_kp_vec_position},
      up_d3dx_mesh_{nullptr, custom_deleter{}},
      dw_materials_number_{},
      d3dx_handle_world_view_proj_{},
      vec_d3d_color_{},
      vecup_mesh_texture_{},
      vec3_center_coodinate_{},
      f_radius_{},
      f_scale_{}
{
  this->up_animation_strategy_.reset(new_crt no_animation);

  ::HRESULT hresult{};
  this->d3dx_handle_world_view_proj_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_world_view_projection");

  ::LPD3DXBUFFER p_d3dx_adjacency_buffer{};
  ::LPD3DXBUFFER p_d3dx_material_buffer{};
  ::LPD3DXMESH   p_temp_mesh{};

  std::vector<char> vecc_buffer = get_resource(
      "select data from model where filename = '" + a_krsz_xfile_name + "';");
  hresult = ::D3DXLoadMeshFromXInMemory(
      &vecc_buffer[0],
      static_cast<::DWORD>(vecc_buffer.size()),
      ::D3DXMESH_SYSTEMMEM,
      this->sp_direct3d_device9_.get(),
      &p_d3dx_adjacency_buffer,
      &p_d3dx_material_buffer,
      nullptr,
      &this->dw_materials_number_,
      &p_temp_mesh);
  if (FAILED(hresult)) {
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to load a x-file."});
  }
  this->up_d3dx_mesh_.reset(p_temp_mesh);

  ::D3DVERTEXELEMENT9 decl[] = {
      {
          0,
          0,
          ::D3DDECLTYPE_FLOAT3,
          ::D3DDECLMETHOD_DEFAULT,
          ::D3DDECLUSAGE_POSITION,
          0
      },
      {
          0,
          12,
          ::D3DDECLTYPE_FLOAT3,
          ::D3DDECLMETHOD_DEFAULT,
          ::D3DDECLUSAGE_NORMAL,
          0
      },
      {
          0,
          24,
          ::D3DDECLTYPE_FLOAT2,
          ::D3DDECLMETHOD_DEFAULT,
          ::D3DDECLUSAGE_TEXCOORD,
          0
      },
      D3DDECL_END(),
  };

  hresult = this->up_d3dx_mesh_->CloneMesh(::D3DXMESH_MANAGED,
                                           decl,
                                           this->sp_direct3d_device9_.get(),
                                           &p_temp_mesh);
  if (FAILED(hresult)) {
    BOOST_THROW_EXCEPTION(custom_exception{"Failed 'CloneMesh' function."});
  }
  this->up_d3dx_mesh_.reset(p_temp_mesh);
  DWORD* pdw_buffer = static_cast<DWORD*>(
      p_d3dx_adjacency_buffer->GetBufferPointer());

  hresult = ::D3DXComputeNormals(this->up_d3dx_mesh_.get(), pdw_buffer);

  if (FAILED(hresult)) {
    BOOST_THROW_EXCEPTION(
        custom_exception{"Failed 'D3DXComputeNormals' function."});
  }

  hresult = this->up_d3dx_mesh_->OptimizeInplace(
      ::D3DXMESHOPT_COMPACT
          | ::D3DXMESHOPT_ATTRSORT
          | ::D3DXMESHOPT_VERTEXCACHE,
      pdw_buffer,
      nullptr,
      nullptr,
      nullptr);
  safe_release(p_d3dx_adjacency_buffer);

  if (FAILED(hresult)) {
    BOOST_THROW_EXCEPTION(
        custom_exception{"Failed 'OptimizeInplace' function."});
  }

  this->vec_d3d_color_.insert(std::begin(this->vec_d3d_color_),
                              this->dw_materials_number_,
                              ::D3DCOLORVALUE{});
  std::vector<std::unique_ptr<::IDirect3DTexture9, custom_deleter> >
      temp_textures(this->dw_materials_number_);
  this->vecup_mesh_texture_.swap(temp_textures);

  ::D3DXMATERIAL* p_d3dx_materials =
      static_cast<::D3DXMATERIAL*>(p_d3dx_material_buffer->GetBufferPointer());

  for (::DWORD i{}; i < this->dw_materials_number_; ++i) {
    this->vec_d3d_color_.at(i) = p_d3dx_materials[i].MatD3D.Diffuse;
    if (p_d3dx_materials[i].pTextureFilename != nullptr) {
      std::string sz_query{};
      sz_query = "select data from model where filename = '";
      sz_query += a_krsz_xfile_name;
      sz_query = sz_query.erase(sz_query.find_last_of('/')+1);
      sz_query += p_d3dx_materials[i].pTextureFilename;
      sz_query += "';";

      vecc_buffer = get_resource(sz_query);
      
      if (::LPDIRECT3DTEXTURE9 p_temp_texture{};
          FAILED(::D3DXCreateTextureFromFileInMemory(
              this->sp_direct3d_device9_.get(),
              &vecc_buffer[0],
              static_cast<::UINT>(vecc_buffer.size()),
              &p_temp_texture))) {
        BOOST_THROW_EXCEPTION(custom_exception{"texture file is not found."});
      } else {
        this->vecup_mesh_texture_.at(i).reset(p_temp_texture);
      }
    }
  }
  safe_release(p_d3dx_material_buffer);

  /* Calculate model size. */
  ::LPVOID pv_buffer{};
  this->up_d3dx_mesh_->LockVertexBuffer(D3DLOCK_READONLY, &pv_buffer);
  ::D3DXComputeBoundingSphere(static_cast<::D3DXVECTOR3 *>(pv_buffer),
      this->up_d3dx_mesh_->GetNumVertices(),
      this->up_d3dx_mesh_->GetNumBytesPerVertex(),
      &this->vec3_center_coodinate_,
      &this->f_radius_);
  this->up_d3dx_mesh_->UnlockVertexBuffer();

  this->f_scale_ = a_krf_size / this->f_radius_;
}

void mesh::do_render(const ::D3DXMATRIX&  a_kr_mat_view,
                     const ::D3DXMATRIX&  a_kr_mat_projection)
{
  ::D3DXMATRIX mat_world_view_projection{};
  ::D3DXMatrixIdentity(&mat_world_view_projection);
  {
    ::D3DXMATRIX mat{};

    ::D3DXMatrixTranslation(&mat,
                            -this->vec3_center_coodinate_.x,
                            -this->vec3_center_coodinate_.y,
                            -this->vec3_center_coodinate_.z);
    mat_world_view_projection *= mat;

    ::D3DXMatrixScaling(&mat,
        this->f_scale_, this->f_scale_, this->f_scale_);
    mat_world_view_projection *= mat;

    ::D3DXMatrixTranslation(&mat,
        this->position_.x, this->position_.y, this->position_.z);
    mat_world_view_projection *= mat;
  }
  mat_world_view_projection *= a_kr_mat_view;
  mat_world_view_projection *= a_kr_mat_projection;

  this->up_d3dx_effect_->SetMatrix(this->d3dx_handle_world_view_proj_,
                                   &mat_world_view_projection);

  this->up_d3dx_effect_->Begin(nullptr, 0);

  ::HRESULT h{};
  if (FAILED(h = this->up_d3dx_effect_->BeginPass(0))) {
    this->up_d3dx_effect_->End();
    BOOST_THROW_EXCEPTION(custom_exception{"Failed 'BeginPass' function."});
  }

  for (::DWORD i{}; i < this->dw_materials_number_; ++i) {
    // TODO : remove redundant set****. 
    ::D3DXVECTOR4 vec4_color{ this->vec_d3d_color_.at(i).r,
                              this->vec_d3d_color_.at(i).g,
                              this->vec_d3d_color_.at(i).b,
                              this->vec_d3d_color_.at(i).a };
    this->up_d3dx_effect_->SetVector(this->d3dx_handle_diffuse_, &vec4_color);
    this->up_d3dx_effect_->SetTexture(this->d3dx_handle_mesh_texture_,
                                      this->vecup_mesh_texture_.at(i).get());
    this->up_d3dx_effect_->CommitChanges();
    this->up_d3dx_mesh_->DrawSubset(i);
  }
  this->up_d3dx_effect_->EndPass();
  this->up_d3dx_effect_->End();
}
} /* namespace early_go */
