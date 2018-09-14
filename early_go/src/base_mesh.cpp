#include "stdafx.hpp"

#include "base_mesh.hpp"

namespace early_go {
base_mesh::base_mesh(
    const std::shared_ptr<::IDirect3DDevice9>& a_krsp_direct3d_device9,
    const std::string& akrsz_shader_filename)
    : sp_direct3d_device9_{a_krsp_direct3d_device9},
      up_d3dx_effect_{nullptr, custom_deleter{}},
      dynamic_texture_{}
{
  ::HRESULT hresult{};
  std::vector<char> vecc_buffer = get_resource(
      "select data from shader where filename = 'shader/"
      + akrsz_shader_filename + "';");
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
  this->d3dx_handle_texture_position_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_position");
  this->d3dx_handle_texture_opacity_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_opacity");
  this->d3dx_handle_light_normal_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_light_normal");
  this->d3dx_handle_brightness_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_light_brightness");
  this->d3dx_handle_mesh_texture_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_mesh_texture");
  this->d3dx_handle_diffuse_ =
      this->up_d3dx_effect_->GetParameterByName(nullptr,
                                                "g_diffuse");

  for (int i{}; i < dynamic_texture::LAYER_NUMBER; ++i){
    if (::LPDIRECT3DTEXTURE9 p_temp_texture{};
        FAILED(::D3DXCreateTexture(this->sp_direct3d_device9_.get(),
                                   dynamic_texture::PIXEL_NUMBER,
                                   dynamic_texture::PIXEL_NUMBER,
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
                static_cast<int*>(locked_rect.pBits) + locked_rect.Pitch
                    * dynamic_texture::PIXEL_NUMBER / sizeof(int),
                0x00000000);

      p_temp_texture->UnlockRect(0);

      this->dynamic_texture_.arsp_texture_.at(i).reset(
          p_temp_texture, custom_deleter{});

      this->up_d3dx_effect_->SetTexture(
          this->ar_d3dx_handle_texture_.at(i),
          this->dynamic_texture_.arsp_texture_.at(i).get());
    }
  }
}
void base_mesh::set_dynamic_texture(const std::string& akrsz_filename,
                               const int&         akri_layer_number,
                               const combine_type& /* akri_combine_type */)
{
  std::string sz_query{};
  sz_query = "select data from texture where filename = '";
  sz_query += akrsz_filename;
  sz_query += "';";

  std::vector<char> vecc_buffer = get_resource(sz_query);
  if (::LPDIRECT3DTEXTURE9 p_temp_texture{};
      FAILED(::D3DXCreateTextureFromFileInMemory(
          this->sp_direct3d_device9_.get(),
          &vecc_buffer[0],
          static_cast<::UINT>(vecc_buffer.size()),
          &p_temp_texture))) {
    BOOST_THROW_EXCEPTION(custom_exception{"texture file is not found."});
  } else {
    this->dynamic_texture_.arsp_texture_.at(akri_layer_number).reset(
        p_temp_texture, custom_deleter{});
    this->dynamic_texture_.arf_opacity_.at(akri_layer_number) = 1.0f;

    this->up_d3dx_effect_->SetTexture(
        this->ar_d3dx_handle_texture_.at(akri_layer_number),
        this->dynamic_texture_.arsp_texture_.at(akri_layer_number).get());
  }
}

void base_mesh::set_dynamic_texture_position(const int&           akri_layer_number,
                                        const ::D3DXVECTOR2& akrvec2_position)
{
  this->dynamic_texture_.arvec2_position_.at(akri_layer_number).x =
      akrvec2_position.x;
  this->dynamic_texture_.arvec2_position_.at(akri_layer_number).y =
      akrvec2_position.y;
}

void base_mesh::set_dynamic_texture_opacity(const int&   akri_layer_number,
                                       const float& akrf_opacity)
{
  this->dynamic_texture_.arf_opacity_.at(akri_layer_number) = akrf_opacity;
}

base_mesh::dynamic_texture::text_message_writer::~text_message_writer()
{
  ::SelectObject(this->hdc_, this->hfont_);
  ::ReleaseDC(nullptr, this->hdc_);
}

void base_mesh::dynamic_texture::text_message_writer::operator()()
{
  ::D3DLOCKED_RECT locked_rect{};
  this->sp_texture_->LockRect(0, &locked_rect, nullptr, 0);

  while (write_character() && !this->krb_animation_);

  this->sp_texture_->UnlockRect(0);
}
bool base_mesh::dynamic_texture::text_message_writer::write_character()
{
  if (this->character_index_ >= this->krsz_message_.length()) {
    return false;
  }
  unsigned char c[2]{};
  if (this->krsz_message_.begin()+this->character_index_+1
      != this->krsz_message_.end()) {
    c[0] = (unsigned char)this->krsz_message_.at(this->character_index_);
    c[1] = (unsigned char)this->krsz_message_.at(this->character_index_+1); 
  } else {
    c[0] = (unsigned char)this->krsz_message_.at(this->character_index_);
  }

  ::UINT char_code{};
  bool b_ascii{};
  if (   0x81 <= c[0] && c[0] <= 0x9f
      || 0xe0 <= c[0] && c[0] <= 0xef) {
    char_code = (c[0] << 8) + c[1];
    ++this->character_index_;
    b_ascii = true;
  } else if (c[0] == '\n') {
    this->font_height_sum_ += text_metric_.tmHeight;
    this->font_width_sum_ = this->kr_rect_.left;
    ++this->character_index_;
    return true;
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

  const ::UINT font_width{(glyph_metrics.gmBlackBoxX + 3) / 4 * 4};
  const ::UINT font_height{glyph_metrics.gmBlackBoxY};

  if (static_cast<::UINT>(this->kr_rect_.right)
      < this->font_width_sum_ + font_width) {
    this->font_height_sum_ += this->text_metric_.tmHeight;
    this->font_width_sum_ = this->kr_rect_.left;
    b_ascii ? this->character_index_ -= 2 : --this->character_index_;
    ++this->character_index_;
    return true;
  } else if (static_cast<::UINT>(this->kr_rect_.bottom)
      < this->font_height_sum_ + font_height) {
    // TODO : page ejection
    ++this->character_index_;
    return false;
  }

  std::vector<::BYTE*> vpw_mono_buffer(font_height);
  for (std::size_t y{}; y < vpw_mono_buffer.size(); ++y) {
    vpw_mono_buffer.at(y) = &up_mono[y * font_width];
  }

  this->font_width_sum_ += glyph_metrics.gmptGlyphOrigin.x;
  int offset{this->font_height_sum_};
  offset += this->text_metric_.tmAscent - glyph_metrics.gmptGlyphOrigin.y;
  
  ::DWORD  new_alpha{};
  ::DWORD* texture_pixel{};
  ::DWORD  current_alpha{};
  ::DWORD  sum_alpha{};

  for (::UINT y{}; y < font_height; ++y) {
    for (::UINT x{}; x < font_width; ++x) {
      new_alpha     = vpw_mono_buffer[y][x] * 255 / GGO_LEVEL;
      texture_pixel =
          &this->vpw_tex_buffer_[y + offset][x + this->font_width_sum_];

      current_alpha = *texture_pixel & 0xff000000UL;
      current_alpha >>= 24;
      sum_alpha     = std::clamp(current_alpha+new_alpha, 0UL, 255UL);

      *texture_pixel = (sum_alpha << 24) | this->kri_color_;
    }
  }
  this->font_width_sum_ += glyph_metrics.gmBlackBoxX;

  ++this->character_index_;
  return true;
}

base_mesh::dynamic_texture::text_message_writer::text_message_writer(
    std::shared_ptr<::IDirect3DDevice9>   asp_direct3d_device9_,
    std::shared_ptr<::IDirect3DTexture9>& asp_texture,
    const std::string                     akrsz_message,
    const bool                            akrb_animation,
    const ::RECT                          akr_rect,
    const int                             akri_color,
    int                                   afont_width_sum,
    int                                   afont_height_sum,
    const std::string&                    akrsz_fontname,
    const int&                            akri_size,
    const int&                            akri_weight)
    : sp_texture_{asp_texture},
      krsz_message_{akrsz_message},
      krb_animation_{akrb_animation},
      kr_rect_{akr_rect},
      kri_color_{akri_color},
      font_width_sum_{afont_width_sum},
      font_height_sum_{afont_height_sum},
      text_metric_{},
      hdc_{},
      hfont_{},
      character_index_{},
      vpw_tex_buffer_{dynamic_texture::PIXEL_NUMBER}
{
  ::LPDIRECT3DTEXTURE9 p_temp_texture{};
  if (FAILED(::D3DXCreateTexture(asp_direct3d_device9_.get(),
                                 dynamic_texture::PIXEL_NUMBER,
                                 dynamic_texture::PIXEL_NUMBER,
                                 1,
                                 D3DUSAGE_DYNAMIC,
                                 ::D3DFMT_A8B8G8R8,
                                 ::D3DPOOL_DEFAULT,
                                 &p_temp_texture))) {
    BOOST_THROW_EXCEPTION(custom_exception{"texture file is not found."});
  }

  this->sp_texture_.reset(p_temp_texture, custom_deleter{});

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
  this->hfont_ = ::CreateFontIndirect(&logfont);
  if (this->hfont_ == nullptr) {
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to create message writer."});
  }

  this->hdc_ = ::GetDC(nullptr);
  this->hfont_ = static_cast<::HFONT>(::SelectObject(this->hdc_, this->hfont_));

  ::GetTextMetrics(this->hdc_, &this->text_metric_);

  ::D3DLOCKED_RECT locked_rect{};
  this->sp_texture_->LockRect(0, &locked_rect, nullptr, 0);

  ::DWORD *pTexBuf = (::DWORD*)locked_rect.pBits;
  if (pTexBuf == nullptr) {
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to create message writer."});
  }
  std::fill(pTexBuf,
            pTexBuf + locked_rect.Pitch
                * dynamic_texture::PIXEL_NUMBER / sizeof(int),
            0x00000000);
  for (int y{}; y < dynamic_texture::PIXEL_NUMBER; ++y) {
    this->vpw_tex_buffer_.at(y) = &pTexBuf[y * dynamic_texture::PIXEL_NUMBER];
  }

  this->sp_texture_->UnlockRect(0);
}

void base_mesh::set_dynamic_message(const int&         akri_layer_number,
                               const std::string& akrsz_message,
                               const bool&        akrf_animation,
                               const ::RECT&      akr_rect,
                               const int&         akri_color,
                               const std::string& akrsz_fontname,
                               const int&         akri_size,
                               const int&         akri_weight)
{
  this->dynamic_texture_.arf_opacity_.at(akri_layer_number) = 1.0f;
  dynamic_texture::text_message_writer* sp_writer{
      new_crt dynamic_texture::text_message_writer{
          this->sp_direct3d_device9_,
          this->dynamic_texture_.arsp_texture_.at(akri_layer_number),
          akrsz_message,
          akrf_animation,
          akr_rect,
          akri_color,
          akr_rect.left,
          akr_rect.top,
          akrsz_fontname,
          akri_size,
          akri_weight }};

  this->up_d3dx_effect_->SetTexture(
          this->ar_d3dx_handle_texture_.at(akri_layer_number),
          this->dynamic_texture_.arsp_texture_.at(akri_layer_number).get());

  this->dynamic_texture_.arsp_writer.at(akri_layer_number).reset(sp_writer);
}

void base_mesh::set_dynamic_message_color(const int& akri_layer_number,
                                          const ::D3DXVECTOR4& akrvec4_color)
{
  this->dynamic_texture_.arvec4_color_.at(akri_layer_number) = akrvec4_color;
}

void base_mesh::render(const ::D3DXMATRIX& a_kr_mat_view,
                       const ::D3DXMATRIX& a_kr_mat_projection,
                       const ::D3DXVECTOR4& a_kr_normal_light,
                       const float&         a_kr_brightness)
{
  this->up_d3dx_effect_->SetVector(
      this->d3dx_handle_light_normal_, &a_kr_normal_light);

  this->up_d3dx_effect_->SetFloat(
      this->d3dx_handle_brightness_, a_kr_brightness);

  this->up_d3dx_effect_->SetVectorArray(
      this->d3dx_handle_texture_position_,
      &this->dynamic_texture_.arvec2_position_[0],
      dynamic_texture::LAYER_NUMBER);

  this->up_d3dx_effect_->SetFloatArray(
      this->d3dx_handle_texture_opacity_,
      &this->dynamic_texture_.arf_opacity_[0], dynamic_texture::LAYER_NUMBER);

  for (std::size_t i{}; i < dynamic_texture::LAYER_NUMBER; ++i) {
    if (this->dynamic_texture_.arsp_writer.at(i)) {
      (*this->dynamic_texture_.arsp_writer.at(i))();
    }
  }
  this->do_render(a_kr_mat_view, a_kr_mat_projection);
}

}
