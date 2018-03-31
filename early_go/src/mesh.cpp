#include "stdafx.hpp"

#include "mesh.hpp"

namespace early_go {

const std::string mesh::HLSL_FILENAME = "mesh_shader.fx";
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
  ::HRESULT _hresult{};

  std::vector<char> _data = get_resource(
      "select data from shader_file where filename = '" + HLSL_FILENAME + "';");
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

  ::LPD3DXBUFFER _p_d3dx_adjacency_buffer{};
  ::LPD3DXBUFFER _p_d3dx_material_buffer{};

  ::LPD3DXMESH _temp_mesh{};
  _data = get_resource(
      "select data from x_file where filename = '" + a_krsz_xfile_name + "';");
  _hresult = ::D3DXLoadMeshFromXInMemory(&_data[0],
                                         static_cast<DWORD>(_data.size()),
                                         ::D3DXMESH_SYSTEMMEM,
                                         a_krsp_direct3d_device9.get(),
                                         &_p_d3dx_adjacency_buffer,
                                         &_p_d3dx_material_buffer,
                                         nullptr,
                                         &this->dw_materials_number_,
                                         &_temp_mesh);
  if (FAILED(_hresult)) {
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to load a x-file."});
  }
  this->up_d3dx_mesh_.reset(_temp_mesh);

  ::D3DVERTEXELEMENT9 _d3d_vertex_element9[] = {
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

  _hresult = this->up_d3dx_mesh_->CloneMesh(::D3DXMESH_MANAGED,
                                            _d3d_vertex_element9,
                                            a_krsp_direct3d_device9.get(),
                                            &_temp_mesh);
  if (FAILED(_hresult)) {
    BOOST_THROW_EXCEPTION(custom_exception{"Failed 'CloneMesh' function."});
  }
  this->up_d3dx_mesh_.reset(_temp_mesh);
  DWORD* _buffer = static_cast<DWORD*>(
      _p_d3dx_adjacency_buffer->GetBufferPointer());

  _hresult = ::D3DXComputeNormals(this->up_d3dx_mesh_.get(), _buffer);

  if (FAILED(_hresult)) {
    BOOST_THROW_EXCEPTION(
        custom_exception{"Failed 'D3DXComputeNormals' function."});
  }

  _hresult = this->up_d3dx_mesh_->OptimizeInplace(
      ::D3DXMESHOPT_COMPACT
          | ::D3DXMESHOPT_ATTRSORT
          | ::D3DXMESHOPT_VERTEXCACHE,
      _buffer,
      nullptr,
      nullptr,
      nullptr);
  safe_release(_p_d3dx_adjacency_buffer);

  if (FAILED(_hresult)) {
    BOOST_THROW_EXCEPTION(
        custom_exception{"Failed 'OptimizeInplace' function."});
  }

  this->vec_d3d_color_.insert(std::begin(this->vec_d3d_color_),
                              this->dw_materials_number_,
                              ::D3DCOLORVALUE{});
  std::vector<std::unique_ptr<::IDirect3DTexture9, custom_deleter> >
      _temp_texture(this->dw_materials_number_);
  this->vecup_mesh_texture_.swap(_temp_texture);

  ::D3DXMATERIAL* _p_d3dx_materials =
      static_cast<::D3DXMATERIAL*>(_p_d3dx_material_buffer->GetBufferPointer());

  for (::DWORD i{}; i < this->dw_materials_number_; ++i) {
    this->vec_d3d_color_.at(i) = _p_d3dx_materials[i].MatD3D.Diffuse;
    if (_p_d3dx_materials[i].pTextureFilename != nullptr) {
      std::string _query;
      _query = "select data from texture where filename = '";
      _query += _p_d3dx_materials[i].pTextureFilename;
      _query += "' and x_filename = '";
      _query += a_krsz_xfile_name + "';";

      _data = get_resource(_query);
      ::LPDIRECT3DTEXTURE9 _pp_temp_texture{};
      if (FAILED(
          ::D3DXCreateTextureFromFileInMemory(a_krsp_direct3d_device9.get(),
                                              &_data[0],
                                              static_cast<UINT>(_data.size()),
                                              &_pp_temp_texture))) {
        BOOST_THROW_EXCEPTION(custom_exception{"texture file is not found."});
      } else {
        this->vecup_mesh_texture_.at(i).reset(_pp_temp_texture);
      }
    }
  }
  safe_release(_p_d3dx_material_buffer);
}

void mesh::render()
{
  ::D3DXMATRIX _matWorldViewProj{};
  {
    ::D3DXMATRIXA16 _mat_world{};
    ::D3DXMATRIXA16 _mat_position{};
    ::D3DXMatrixIdentity(&_mat_world);
    ::D3DXMatrixTranslation(&_mat_position,
        this->vec_position_.x, this->vec_position_.y, this->vec_position_.z);

    ::D3DXMatrixMultiply(&_matWorldViewProj, &_mat_world, &_mat_position);
  }
  // TODO Integrate the camera dealing.
  {
    ::D3DXMATRIXA16 _mat_projection{};
    ::D3DXMatrixPerspectiveFovLH(
        &_mat_projection,
        D3DX_PI / 4,
        static_cast<float>(constants::WINDOW_WIDTH) / constants::WINDOW_HEIGHT,
        0.1f,
        100.0f);
    _matWorldViewProj *= _mat_projection;
  }

  this->up_d3dx_effect_->SetMatrix(this->d3dx_handle_world_view_proj_,
                                   &_matWorldViewProj);
  ::D3DXVECTOR4 _light_position{ 1.0f, 1.0f, -1.0f, 1.0f };
  this->up_d3dx_effect_->SetVector(
      this->d3dx_handle_light_position_, &_light_position);
  this->up_d3dx_effect_->SetFloat(this->d3dx_handle_brightness_, 150.0f);
  this->up_d3dx_effect_->SetFloat(this->d3dx_handle_scale_, 1.0f);

  this->up_d3dx_effect_->Begin(nullptr, 0);

  if (FAILED(this->up_d3dx_effect_->BeginPass(0))) {
    this->up_d3dx_effect_->End();
    BOOST_THROW_EXCEPTION(custom_exception{"Failed 'BeginPass' function."});
  }
  for (::DWORD i{}; i < this->dw_materials_number_; ++i) {
    ::D3DXVECTOR4 _color{this->vec_d3d_color_.at(i).r,
                         this->vec_d3d_color_.at(i).g,
                         this->vec_d3d_color_.at(i).b,
                         1.0f };
                         //this->vec_d3d_color_.at(i).a };
    this->up_d3dx_effect_->SetVector(this->d3dx_handle_diffuse_, &_color);
    this->up_d3dx_effect_->SetTexture(this->d3dx_handle_texture_,
                                      this->vecup_mesh_texture_.at(i).get());
    this->up_d3dx_effect_->CommitChanges();
    this->up_d3dx_mesh_->DrawSubset(i);
  }
  this->up_d3dx_effect_->EndPass();
  this->up_d3dx_effect_->End();
}
} /* namespace early_go */
