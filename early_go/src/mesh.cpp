#include "stdafx.hpp"

#include "mesh.hpp"

namespace early_go {

const std::string mesh::SHADER_FILENAME = "mesh_shader.fx";

mesh::mesh(
    const std::shared_ptr<::IDirect3DDevice9>& a_krsp_direct3d_device9,
    const std::string& a_krsz_xfile_name,
    const ::D3DXVECTOR3& a_kp_vec_position,
    const float& a_krf_size)
    : sp_direct3d_device9_{a_krsp_direct3d_device9},
      vec3_position_{a_kp_vec_position},
      up_d3dx_mesh_{nullptr, custom_deleter{}},
      dw_materials_number_{},
      up_d3dx_effect_{nullptr, custom_deleter{}},
      d3dx_handle_world_view_proj_{},
      d3dx_handle_light_normal_{},
      d3dx_handle_brightness_{},
      d3dx_handle_diffuse_{},
      vec_d3d_color_{},
      vecup_mesh_texture_{},
      vec3_center_coodinate_{},
      f_radius_{},
      f_scale_{},
      dynamic_texture_{}
{
  ::HRESULT hresult{};

  std::vector<char> vecc_buffer = get_resource(
      "select data from shader_file where filename = '"
      + this->SHADER_FILENAME + "';");
  ::LPD3DXEFFECT p_d3dx_temp_effect{};
  ::D3DXCreateEffect(this->sp_direct3d_device9_.get(),
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
                                                "g_world_view_projection");
  this->d3dx_handle_light_normal_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_light_normal");
  this->d3dx_handle_brightness_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_light_brightness");
  this->d3dx_handle_mesh_texture_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_mesh_texture");
  this->ar_d3dx_handle_texture_[0] =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_texture_0");
  this->ar_d3dx_handle_texture_[1] =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_texture_1");
  this->ar_d3dx_handle_texture_[2] =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_texture_2");
  this->ar_d3dx_handle_texture_[3] =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_texture_3");
  this->ar_d3dx_handle_texture_[4] =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_texture_4");
  this->ar_d3dx_handle_texture_[5] =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_texture_5");
  this->ar_d3dx_handle_texture_[6] =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_texture_6");
  this->ar_d3dx_handle_texture_[7] =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_texture_7");
  this->d3dx_handle_diffuse_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_diffuse");
  this->d3dx_handle_texture_position_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr, "g_position");
  this->d3dx_handle_texture_opacity_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr, "g_opacity");

  ::LPD3DXBUFFER p_d3dx_adjacency_buffer{};
  ::LPD3DXBUFFER p_d3dx_material_buffer{};

  ::LPD3DXMESH temp_mesh{};
  vecc_buffer = get_resource(
      "select data from x_file where filename = '" + a_krsz_xfile_name + "';");
  hresult = ::D3DXLoadMeshFromXInMemory(
      &vecc_buffer[0],
      static_cast<::DWORD>(vecc_buffer.size()),
      ::D3DXMESH_SYSTEMMEM,
      this->sp_direct3d_device9_.get(),
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
                                            this->sp_direct3d_device9_.get(),
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

  for (int i{}; i < 8; ++i){
    if (::LPDIRECT3DTEXTURE9 p_temp_texture{};
        FAILED(::D3DXCreateTexture(this->sp_direct3d_device9_.get(),
                                   512,
                                   512,
                                   1,
                                   D3DUSAGE_DYNAMIC,
                                   ::D3DFMT_A8B8G8R8,
                                   ::D3DPOOL_DEFAULT,
                                   &p_temp_texture))) {
      BOOST_THROW_EXCEPTION(custom_exception{"texture file is not found."});
    } else {
      ::D3DLOCKED_RECT locked_rect{};
      p_temp_texture->LockRect(0, &locked_rect, nullptr, D3DLOCK_DISCARD);

      std::fill(static_cast<int*>(locked_rect.pBits),
                static_cast<int*>(locked_rect.pBits)
                    + locked_rect.Pitch*512/sizeof(int),
                0x00000000);

      p_temp_texture->UnlockRect(0);

      this->dynamic_texture_.arup_texture_.at(i).reset(p_temp_texture);
      this->up_d3dx_effect_->SetTexture(
          this->ar_d3dx_handle_texture_.at(i),
          this->dynamic_texture_.arup_texture_.at(i).get());
    }
  }

  /* Calculate model size.  */
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
mesh::~mesh()
{
  ::SelectObject(this->hdc_, this->hfont_);
  ::ReleaseDC(nullptr, this->hdc_);
}

void mesh::set_dynamic_texture(const std::string& akrsz_filename,
                               const int& akri_layer_number,
                               const combine_type& /* akri_combine_type */)
{
  std::string sz_query{};
  sz_query = "select data from texture where filename = '";
  sz_query += akrsz_filename;
  sz_query += "';";

  ::LPDIRECT3DDEVICE9 p_temp_direct3d_device9{nullptr};
  this->up_d3dx_mesh_->GetDevice(&p_temp_direct3d_device9);
  std::vector<char> vecc_buffer = get_resource(sz_query);
  if (::LPDIRECT3DTEXTURE9 p_temp_texture{};
      FAILED(::D3DXCreateTextureFromFileInMemory(
          p_temp_direct3d_device9,
          &vecc_buffer[0],
          static_cast<::UINT>(vecc_buffer.size()),
          &p_temp_texture))) {
    BOOST_THROW_EXCEPTION(custom_exception{"texture file is not found."});
  } else {
    this->dynamic_texture_.arup_texture_.at(akri_layer_number).reset(p_temp_texture);
    this->dynamic_texture_.arf_opacity_.at(akri_layer_number) = 1.0f;

    this->up_d3dx_effect_->SetTexture(
        this->ar_d3dx_handle_texture_.at(akri_layer_number),
        this->dynamic_texture_.arup_texture_.at(akri_layer_number).get());
  }
}

void mesh::set_dynamic_texture_position(const int& akri_layer_number,
                                        const ::D3DXVECTOR2& akrvec2_position)
{
  this->dynamic_texture_.arvec2_position_.at(akri_layer_number).x =
      akrvec2_position.x;
  this->dynamic_texture_.arvec2_position_.at(akri_layer_number).y =
      akrvec2_position.y;
}

void mesh::set_dynamic_texture_opacity(const int& akri_layer_number,
                                       const float& akrf_opacity)
{
  this->dynamic_texture_.arf_opacity_.at(akri_layer_number) = akrf_opacity;
}

void mesh::set_dynamic_message(const int& akri_layer_number,
                               const std::string& akrsz_message,
                               const ::RECT& akr_rect,
                               const int& akri_color,
                               const std::string& akrsz_fontname,
                               const int& akri_size,
                               const int& akri_weight,
                               const bool& akrf_animation)
{
  ::LPDIRECT3DTEXTURE9 p_temp_texture{};
  if (FAILED(::D3DXCreateTexture(this->sp_direct3d_device9_.get(),
                                 512,
                                 512,
                                 1,
                                 D3DUSAGE_DYNAMIC,
                                 ::D3DFMT_A8B8G8R8,
                                 ::D3DPOOL_DEFAULT,
                                 &p_temp_texture))) {
    BOOST_THROW_EXCEPTION(custom_exception{"texture file is not found."});
  }
  ::LOGFONT logfont = {akri_size,
                       0,
                       0,
                       0,
                       akri_weight,
                       0,
                       0,
                       0,
                       SHIFTJIS_CHARSET,
                       OUT_TT_ONLY_PRECIS,
                       CLIP_DEFAULT_PRECIS,
                       PROOF_QUALITY,
                       DEFAULT_PITCH | FF_MODERN};
  ::strcpy_s(logfont.lfFaceName, akrsz_fontname.c_str());
  this->hfont_= ::CreateFontIndirect(&logfont);
  if (this->hfont_ == nullptr) {
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to create font."});
  }

  this->hdc_ = ::GetDC(nullptr);
  this->hfont_ = static_cast<::HFONT>(::SelectObject(this->hdc_, this->hfont_));

  ::TEXTMETRIC text_metric{};
  ::GetTextMetrics(this->hdc_, &text_metric);

  ::D3DLOCKED_RECT locked_rect{};
  p_temp_texture->LockRect(0, &locked_rect, nullptr, 0);
  std::fill(static_cast<int*>(locked_rect.pBits),
            static_cast<int*>(locked_rect.pBits)
                + locked_rect.Pitch*512/sizeof(int),
            0x00000000);

  ::DWORD *pTexBuf = (::DWORD*)locked_rect.pBits;   // テクスチャメモリへのポインタ
  std::vector<std::vector<::DWORD*> > vvw_tex_buffer(
      512, std::vector<::DWORD*>(512));
  for (std::size_t y{}; y < vvw_tex_buffer.size(); ++y) {
    for (std::size_t x{}; x < vvw_tex_buffer.at(y).size(); ++x) {
      vvw_tex_buffer.at(y).at(x) = &pTexBuf[y*512 + x];
    }
  }

  int font_width_sum{};
  int font_height_sum{};
  for (std::size_t i{}; i < akrsz_message.length(); ++i) {
    unsigned char c[2]{};
    if (akrsz_message.begin()+i+1 != akrsz_message.end()) {
      c[0] = (unsigned char)akrsz_message.at(i);
      c[1] = (unsigned char)akrsz_message.at(i+1); 
    } else {
      c[0] = (unsigned char)akrsz_message.at(i);
    }

    ::UINT char_code{};
    if (   0x81 <= c[0] && c[0] <= 0x9f
        || 0xe0 <= c[0] && c[0] <= 0xef) {
      char_code = (c[0] << 8) + c[1];
      ++i;
    } else if (c[0] == '\n') {
      font_height_sum+=text_metric.tmHeight;
      font_width_sum = 0;
      continue;
    } else {
      char_code = c[0];
    }

    const ::UINT GGO_FLAG{GGO_GRAY4_BITMAP};
    const ::DWORD GGO_LEVEL{16};

    ::GLYPHMETRICS glyph_metrics{};
    const ::MAT2 mat{{0, 1}, {0, 0}, {0, 0}, {0, 1}};
    ::DWORD mono_font_data_size = ::GetGlyphOutline(this->hdc_,
                                                    char_code,
                                                    GGO_FLAG,
                                                    &glyph_metrics,
                                                    0,
                                                    nullptr,
                                                    &mat);
    std::unique_ptr<::BYTE[]> up_mono{new_crt ::BYTE[mono_font_data_size]};
    ::GetGlyphOutline(this->hdc_,
                      char_code,
                      GGO_FLAG,
                      &glyph_metrics,
                      mono_font_data_size,
                      up_mono.get(),
                      &mat);

    const ::UINT font_width = (glyph_metrics.gmBlackBoxX + 3) / 4 * 4;
    const ::UINT font_height = glyph_metrics.gmBlackBoxY;

    if (static_cast<::UINT>(akr_rect.right - akr_rect.left)
        < font_width_sum + font_width) {
      font_height_sum += text_metric.tmHeight;
      font_width_sum = 0;
      --i;
      continue;
    } else if (static_cast<::UINT>(akr_rect.bottom - akr_rect.top)
        < font_height_sum + font_height) {
      // TODO
    }

    std::vector<std::vector<::BYTE> > vvc_tex_buffer(
        font_height, std::vector<::BYTE>(font_width));
    for (std::size_t y{}; y < vvc_tex_buffer.size(); ++y) {
      for (std::size_t x{}; x < vvc_tex_buffer.at(y).size(); ++x) {
        vvc_tex_buffer.at(y).at(x) = up_mono[y*font_width + x];
      }
    }

    font_width_sum += glyph_metrics.gmptGlyphOrigin.x;
    int offset{font_height_sum};
    offset += text_metric.tmAscent - glyph_metrics.gmptGlyphOrigin.y;
    for (::UINT y{}; y < font_height; y++) {
      for (::UINT x{}; x < font_width; x++) {
        ::DWORD new_alpha{ vvc_tex_buffer[y][x] * 255 / GGO_LEVEL };

        ::DWORD* texture_pixel = vvw_tex_buffer[y + offset][x + font_width_sum];

        ::DWORD current_alpha{ *texture_pixel & 0xff000000UL };
        current_alpha >>= 24;
        ::DWORD sum_alpha = std::clamp(current_alpha+new_alpha, 0UL, 255UL);

        *texture_pixel = (sum_alpha << 24) | akri_color;
      }
    }
    font_width_sum += glyph_metrics.gmBlackBoxX;
  }

  p_temp_texture->UnlockRect(0);
  this->dynamic_texture_.arup_texture_.at(akri_layer_number).reset(p_temp_texture);
  this->dynamic_texture_.arf_opacity_.at(akri_layer_number) = 1.0f;

  this->up_d3dx_effect_->SetTexture(
      this->ar_d3dx_handle_texture_.at(akri_layer_number), 
      this->dynamic_texture_.arup_texture_.at(akri_layer_number).get());
}

void mesh::set_dynamic_message_color(const int& akri_layer_number,
                                    const ::D3DXVECTOR4& akrvec4_color)
{
  this->dynamic_texture_.arvec4_color_.at(akri_layer_number) = akrvec4_color;
}

void mesh::render(const ::D3DXMATRIX& a_kr_mat_view,
                  const ::D3DXMATRIX& a_kr_mat_projection,
                  const::D3DXVECTOR4 & a_kr_normal_light,
                  const float& a_kr_brightness)
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
        this->vec3_position_.x, this->vec3_position_.y, this->vec3_position_.z);
    mat_world_view_projection *= mat;
  }
  mat_world_view_projection *= a_kr_mat_view;
  mat_world_view_projection *= a_kr_mat_projection;

  this->up_d3dx_effect_->SetMatrix(this->d3dx_handle_world_view_proj_,
                                   &mat_world_view_projection);
  this->up_d3dx_effect_->SetVector(this->d3dx_handle_light_normal_,
                                   &a_kr_normal_light);
  this->up_d3dx_effect_->SetFloat(this->d3dx_handle_brightness_,
                                  a_kr_brightness);

  // dynamic texture
  this->update_texture();

  this->up_d3dx_effect_->Begin(nullptr, 0);

  ::HRESULT h{};
  if (FAILED(h = this->up_d3dx_effect_->BeginPass(0))) {
    this->up_d3dx_effect_->End();
    BOOST_THROW_EXCEPTION(custom_exception{"Failed 'BeginPass' function."});
  }
  for (::DWORD i{}; i < this->dw_materials_number_; ++i) {
    ::D3DXVECTOR4 vec4_color{this->vec_d3d_color_.at(i).r,
                             this->vec_d3d_color_.at(i).g,
                             this->vec_d3d_color_.at(i).b,
                             this->vec_d3d_color_.at(i).a};
    this->up_d3dx_effect_->SetVector(this->d3dx_handle_diffuse_, &vec4_color);
    this->up_d3dx_effect_->SetTexture(this->d3dx_handle_mesh_texture_,
                                      this->vecup_mesh_texture_.at(i).get());
    this->up_d3dx_effect_->CommitChanges();
    this->up_d3dx_mesh_->DrawSubset(i);
  }
  this->up_d3dx_effect_->EndPass();
  this->up_d3dx_effect_->End();
}

void mesh::update_texture()
{
  this->up_d3dx_effect_->SetVectorArray(
      this->d3dx_handle_texture_position_,
      &this->dynamic_texture_.arvec2_position_[0], 8);

  this->up_d3dx_effect_->SetFloatArray(
      this->d3dx_handle_texture_opacity_,
      &this->dynamic_texture_.arf_opacity_[0], 8);

  return;
}
} /* namespace early_go */
