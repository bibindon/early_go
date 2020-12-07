#include "stdafx.hpp"

#include "mesh.hpp"

namespace early_go {

const std::string mesh::SHADER_FILENAME = "mesh_shader.fx";

mesh::mesh(
    const std::shared_ptr<::IDirect3DDevice9>& d3d_device,
    const std::string& x_filename,
    const ::D3DXVECTOR3& position,
    const ::D3DXVECTOR3& rotation,
    const float& scale)
    : base_mesh{d3d_device, SHADER_FILENAME, position, rotation},
      d3dx_mesh_{nullptr, custom_deleter{}},
      materials_count_{},
      world_view_proj_handle_{},
      colors_{},
      textures_{},
      center_coodinate_{0.0f, 0.0f, 0.0f},
      radius_{},
      scale_{}
{
  mesh_name_=x_filename;
  animation_strategy_.reset(new_crt no_animation);

  ::HRESULT result{};
  world_view_proj_handle_ =
      effect_->GetParameterByName(nullptr, "g_world_view_projection");

  ::LPD3DXBUFFER adjacency_buffer{};
  ::LPD3DXBUFFER material_buffer{};
  ::LPD3DXMESH   temp_mesh{};

  std::vector<char> buffer = get_resource(
      "SELECT DATA FROM MODEL WHERE FILENAME = '" + x_filename + "';");
  result = ::D3DXLoadMeshFromXInMemory(&buffer[0],
                                       static_cast<::DWORD>(buffer.size()),
                                       ::D3DXMESH_SYSTEMMEM,
                                       d3d_device_.get(),
                                       &adjacency_buffer,
                                       &material_buffer,
                                       nullptr,
                                       &materials_count_,
                                       &temp_mesh);
  if (FAILED(result)) {
    THROW_WITH_TRACE("Failed to load a x-file.");
  }
  d3dx_mesh_.reset(temp_mesh);

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

  result = d3dx_mesh_->CloneMesh(::D3DXMESH_MANAGED,
                                 decl,
                                 d3d_device_.get(),
                                 &temp_mesh);
  if (FAILED(result)) {
    THROW_WITH_TRACE("Failed 'CloneMesh' function.");
  }
  d3dx_mesh_.reset(temp_mesh);
  DWORD* word_buffer =
      static_cast<DWORD*>(adjacency_buffer->GetBufferPointer());

  result = ::D3DXComputeNormals(d3dx_mesh_.get(), word_buffer);

  if (FAILED(result)) {
    THROW_WITH_TRACE("Failed 'D3DXComputeNormals' function.");
  }

  result = d3dx_mesh_->OptimizeInplace(
      ::D3DXMESHOPT_COMPACT
          | ::D3DXMESHOPT_ATTRSORT
          | ::D3DXMESHOPT_VERTEXCACHE,
      word_buffer,
      nullptr,
      nullptr,
      nullptr);
  safe_release(adjacency_buffer);

  if (FAILED(result)) {
    THROW_WITH_TRACE("Failed 'OptimizeInplace' function.");
  }

  colors_.insert(std::begin(colors_), materials_count_, ::D3DCOLORVALUE{});
  std::vector<std::unique_ptr<::IDirect3DTexture9, custom_deleter> >
      temp_textures(materials_count_);
  textures_.swap(temp_textures);

  ::D3DXMATERIAL* materials =
      static_cast<::D3DXMATERIAL*>(material_buffer->GetBufferPointer());

  for (::DWORD i{}; i < materials_count_; ++i) {
    colors_.at(i) = materials[i].MatD3D.Diffuse;
    if (materials[i].pTextureFilename != nullptr) {
      std::string query{};
      query = "SELECT DATA FROM MODEL WHERE FILENAME = '";
      query += x_filename;
      query = query.erase(query.find_last_of('/')+1);
      query += materials[i].pTextureFilename;
      query += "';";

      buffer = get_resource(query);
      ::LPDIRECT3DTEXTURE9 temp_texture{};
      if (FAILED(::D3DXCreateTextureFromFileInMemory(
              d3d_device_.get(),
              &buffer[0],
              static_cast<::UINT>(buffer.size()),
              &temp_texture))) {
        THROW_WITH_TRACE("texture file is not found.");
      } else {
        textures_.at(i).reset(temp_texture);
      }
    }
  }
  safe_release(material_buffer);

  scale_ = scale;
}

void mesh::render(const ::D3DXMATRIX&  view_matrix,
                  const ::D3DXMATRIX&  projection_matrix)
{
  ::D3DXMATRIX world_view_projection_matrix{};
  ::D3DXMatrixIdentity(&world_view_projection_matrix);
  {
    ::D3DXMATRIX mat{};

    ::D3DXMatrixTranslation(&mat,
                            -center_coodinate_.x,
                            -center_coodinate_.y,
                            -center_coodinate_.z);
    world_view_projection_matrix *= mat;

    ::D3DXMatrixScaling(&mat, scale_, scale_, scale_);
    world_view_projection_matrix *= mat;

    ::D3DXMatrixRotationYawPitchRoll(
        &mat, rotation_.x, rotation_.y, rotation_.z);
    world_view_projection_matrix *= mat;

    ::D3DXMatrixTranslation(&mat, position_.x, position_.y, position_.z);
    world_view_projection_matrix *= mat;
  }
  world_view_projection_matrix *= view_matrix;
  world_view_projection_matrix *= projection_matrix;

  effect_->SetMatrix(world_view_proj_handle_, &world_view_projection_matrix);

  effect_->Begin(nullptr, 0);

  ::HRESULT result{};
  if (FAILED(result = effect_->BeginPass(0))) {
    effect_->End();
    THROW_WITH_TRACE("Failed 'BeginPass' function.");
  }

  for (::DWORD i{}; i < materials_count_; ++i) {
    // TODO : remove redundant set****. 
    ::D3DXVECTOR4 vec4_color{ colors_.at(i).r,
                              colors_.at(i).g,
                              colors_.at(i).b,
                              colors_.at(i).a };
    effect_->SetVector(diffuse_handle_, &vec4_color);
    effect_->SetTexture(mesh_texture_handle_, textures_.at(i).get());
    effect_->CommitChanges();
    d3dx_mesh_->DrawSubset(i);
  }
  effect_->EndPass();
  effect_->End();
}
} /* namespace early_go */
