#include "stdafx.hpp"

#include "mesh.hpp"

namespace early_go {

mesh::mesh(
    const std::shared_ptr<::IDirect3DDevice9>& a_krsp_direct3d_device9,
    const std::string& a_krsz_xfile_name,
    const ::D3DXVECTOR3& a_kp_vec_position)
    : vec_position_{a_kp_vec_position},
      up_d3dx_mesh_{nullptr, custom_deleter{}},
      dw_materials_number_{},
      up_d3dx_effect_{nullptr, custom_deleter{}},
      d3dx_handle_world_view_proj_{},
      d3dx_handle_light_position_{},
      d3dx_handle_brightness_{},
      d3dx_handle_scale_{},
      d3dx_handle_texture_{},
      d3dx_handle_diffuse_{},
      vec_d3d_color_{},
      vecup_mesh_texture_{}
{
  ::HRESULT hresult{};

  std::vector<char> vecc_buffer = get_resource(
      "select data from shader_file where filename = '"
      + constants::MESH_HLSL + "';");
  ::LPD3DXEFFECT p_d3dx_temp_effect{};
  ::D3DXCreateEffect(a_krsp_direct3d_device9.get(),
                     &vecc_buffer[0],
                     static_cast<::UINT>(vecc_buffer.size()),
                     nullptr,
                     nullptr,
                     0,
                     nullptr,
                     &p_d3dx_temp_effect,
                     nullptr);
  this->up_d3dx_effect_.reset(p_d3dx_temp_effect);
  if (FAILED(hresult)) {
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

  ::LPD3DXBUFFER p_d3dx_adjacency_buffer{};
  ::LPD3DXBUFFER p_d3dx_material_buffer{};

  ::LPD3DXMESH temp_mesh{};
  vecc_buffer = get_resource(
      "select data from x_file where filename = '" + a_krsz_xfile_name + "';");
  hresult = ::D3DXLoadMeshFromXInMemory(&vecc_buffer[0],
                                         static_cast<DWORD>(vecc_buffer.size()),
                                         ::D3DXMESH_SYSTEMMEM,
                                         a_krsp_direct3d_device9.get(),
                                         &p_d3dx_adjacency_buffer,
                                         &p_d3dx_material_buffer,
                                         nullptr,
                                         &this->dw_materials_number_,
                                         &temp_mesh);
  if (FAILED(hresult)) {
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to load a x-file."});
  }
  this->up_d3dx_mesh_.reset(temp_mesh);

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
                                            a_krsp_direct3d_device9.get(),
                                            &temp_mesh);
  if (FAILED(hresult)) {
    BOOST_THROW_EXCEPTION(custom_exception{"Failed 'CloneMesh' function."});
  }
  this->up_d3dx_mesh_.reset(temp_mesh);
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
      temp_texture(this->dw_materials_number_);
  this->vecup_mesh_texture_.swap(temp_texture);

  ::D3DXMATERIAL* p_d3dx_materials =
      static_cast<::D3DXMATERIAL*>(p_d3dx_material_buffer->GetBufferPointer());

  for (::DWORD i{}; i < this->dw_materials_number_; ++i) {
    this->vec_d3d_color_.at(i) = p_d3dx_materials[i].MatD3D.Diffuse;
    if (p_d3dx_materials[i].pTextureFilename != nullptr) {
      std::string sz_query;
      sz_query = "select data from texture where filename = '";
      sz_query += p_d3dx_materials[i].pTextureFilename;
      sz_query += "' and x_filename = '";
      sz_query += a_krsz_xfile_name + "';";

      vecc_buffer = get_resource(sz_query);
      ::LPDIRECT3DTEXTURE9 p_temp_texture{};
      if (FAILED(
          ::D3DXCreateTextureFromFileInMemory(a_krsp_direct3d_device9.get(),
                                              &vecc_buffer[0],
                                              static_cast<UINT>(vecc_buffer.size()),
                                              &p_temp_texture))) {
        BOOST_THROW_EXCEPTION(custom_exception{"texture file is not found."});
      } else {
        this->vecup_mesh_texture_.at(i).reset(p_temp_texture);
      }
    }
  }
  safe_release(p_d3dx_material_buffer);
}

void mesh::render(const ::D3DXMATRIX& a_kr_mat_view,
            const ::D3DXMATRIX& a_kr_mat_projection,
            const ::D3DXVECTOR3& a_kr_light_position,
            const float& a_kr_brightness)
{
  ::D3DXMATRIX mat_world_view_projection{};
  {
    ::D3DXMATRIX mat_world{};
    ::D3DXMatrixIdentity(&mat_world);
    ::D3DXMatrixTranslation(&mat_world,
        this->vec_position_.x, this->vec_position_.y, this->vec_position_.z);

    mat_world_view_projection = mat_world;
  }
  mat_world_view_projection *= a_kr_mat_view;
  mat_world_view_projection *= a_kr_mat_projection;

  this->up_d3dx_effect_->SetMatrix(this->d3dx_handle_world_view_proj_,
                                   &mat_world_view_projection);
  ::D3DXVECTOR4 vec4_light_position{a_kr_light_position, 1.0f};
  this->up_d3dx_effect_->SetVector(
      this->d3dx_handle_light_position_, &vec4_light_position);
  this->up_d3dx_effect_->SetFloat(
      this->d3dx_handle_brightness_, a_kr_brightness);
  this->up_d3dx_effect_->SetFloat(this->d3dx_handle_scale_, 1.0f);

  this->up_d3dx_effect_->Begin(nullptr, 0);

  if (FAILED(this->up_d3dx_effect_->BeginPass(0))) {
    this->up_d3dx_effect_->End();
    BOOST_THROW_EXCEPTION(custom_exception{"Failed 'BeginPass' function."});
  }
  for (::DWORD i{}; i < this->dw_materials_number_; ++i) {
    ::D3DXVECTOR4 vec4_color{this->vec_d3d_color_.at(i).r,
                             this->vec_d3d_color_.at(i).g,
                             this->vec_d3d_color_.at(i).b,
                             this->vec_d3d_color_.at(i).a};
    this->up_d3dx_effect_->SetVector(this->d3dx_handle_diffuse_, &vec4_color);
    this->up_d3dx_effect_->SetTexture(this->d3dx_handle_texture_,
                                      this->vecup_mesh_texture_.at(i).get());
    this->up_d3dx_effect_->CommitChanges();
    this->up_d3dx_mesh_->DrawSubset(i);
  }
  this->up_d3dx_effect_->EndPass();
  this->up_d3dx_effect_->End();
}
} /* namespace early_go */
