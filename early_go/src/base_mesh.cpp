#include "stdafx.hpp"

#include "base_mesh.hpp"

namespace early_go {

const int64_t base_mesh::TEXTURE_PIXEL_SIZE = 2048;

base_mesh::base_mesh(
    const std::shared_ptr<::IDirect3DDevice9>& d3d_device,
    const std::string& shader_filename,
    const ::D3DXVECTOR3& position,
    const ::D3DXVECTOR3& rotation)
    : d3d_device_{d3d_device},
      effect_{nullptr, custom_deleter{}},
      animation_strategy_{nullptr},
      dynamic_texture_{},
      position_{position},
      rotation_{rotation}
{
  ::HRESULT result{};
  std::vector<char> buffer = get_resource(
      "SELECT DATA FROM SHADER WHERE FILENAME = 'shader/"
      + shader_filename + "';");
  ::LPD3DXEFFECT effect{};
  ::D3DXCreateEffect(d3d_device_.get(),
                     &buffer[0],
                     static_cast<::UINT>(buffer.size()),
                     nullptr,
                     nullptr,
                     0,
                     nullptr,
                     &effect,
                     nullptr);
  effect_.reset(effect);
  if (FAILED(result)) {
    THROW_WITH_TRACE("Failed to create an effect file.");
  }

  texture_handle_[0] = effect_->GetParameterByName(nullptr, "g_texture_0");
  texture_handle_[1] = effect_->GetParameterByName(nullptr, "g_texture_1");
  texture_handle_[2] = effect_->GetParameterByName(nullptr, "g_texture_2");
  texture_handle_[3] = effect_->GetParameterByName(nullptr, "g_texture_3");
  texture_handle_[4] = effect_->GetParameterByName(nullptr, "g_texture_4");
  texture_handle_[5] = effect_->GetParameterByName(nullptr, "g_texture_5");
  texture_handle_[6] = effect_->GetParameterByName(nullptr, "g_texture_6");
  texture_handle_[7] = effect_->GetParameterByName(nullptr, "g_texture_7");

  texture_position_handle_ = effect_->GetParameterByName(nullptr, "g_position");
  texture_opacity_handle_  = effect_->GetParameterByName(nullptr, "g_opacity");
  light_normal_handle_     =
      effect_->GetParameterByName(nullptr, "g_light_normal");
  brightness_handle_       =
      effect_->GetParameterByName(nullptr, "g_light_brightness");
  mesh_texture_handle_     =
      effect_->GetParameterByName(nullptr, "g_mesh_texture");
  diffuse_handle_          = effect_->GetParameterByName(nullptr, "g_diffuse");

  for (int i{}; i < dynamic_texture::LAYER_NUMBER; ++i){
    if (::LPDIRECT3DTEXTURE9 temp_texture{};
        FAILED(::D3DXCreateTexture(d3d_device_.get(),
                                   TEXTURE_PIXEL_SIZE,
                                   TEXTURE_PIXEL_SIZE,
                                   1,
                                   D3DUSAGE_DYNAMIC,
                                   ::D3DFMT_A8B8G8R8,
                                   ::D3DPOOL_DEFAULT,
                                   &temp_texture))) {
      THROW_WITH_TRACE("texture file is not found.");
    } else {
      ::D3DLOCKED_RECT locked_rect{};
      temp_texture->LockRect(0, &locked_rect, nullptr, D3DLOCK_DISCARD);

      std::fill(static_cast<int*>(locked_rect.pBits),
                static_cast<int*>(locked_rect.pBits) + locked_rect.Pitch
                    * TEXTURE_PIXEL_SIZE / sizeof(int),
                0x00000000);

      temp_texture->UnlockRect(0);

      dynamic_texture_.textures_.at(i).reset(temp_texture, custom_deleter{});

      effect_->SetTexture(texture_handle_.at(i),
          dynamic_texture_.textures_.at(i).get());
    }
  }
}

void base_mesh::set_dynamic_texture(const std::string& filename,
                                    const int&         layer_number,
                                    const combine_type&)
{
  std::string query{};
  query = "SELECT DATA FROM IMAGE WHERE FILENAME = '" + filename + "';";

  std::vector<char> buffer = get_resource(query);
  if (::LPDIRECT3DTEXTURE9 temp_texture{};
      FAILED(::D3DXCreateTextureFromFileInMemory(
          d3d_device_.get(),
          &buffer[0],
          static_cast<::UINT>(buffer.size()),
          &temp_texture))) {
    THROW_WITH_TRACE("texture file is not found.");
  } else {
    dynamic_texture_.textures_.at(layer_number).reset(
        temp_texture, custom_deleter{});
    dynamic_texture_.opacities_.at(layer_number) = 1.0f;

    effect_->SetTexture(texture_handle_.at(layer_number),
                        dynamic_texture_.textures_.at(layer_number).get());
  }
}

void base_mesh::set_dynamic_texture_position(const int&           layer_number,
                                             const ::D3DXVECTOR2& position)
{
  dynamic_texture_.positions_.at(layer_number).x = position.x;
  dynamic_texture_.positions_.at(layer_number).y = position.y;
}

void base_mesh::set_dynamic_texture_opacity(const int&   layer_number,
                                            const float& opacity)
{
  dynamic_texture_.opacities_.at(layer_number) = opacity;
}

base_mesh::dynamic_texture::text_message_writer::~text_message_writer()
{
  ::SelectObject(hdc_, hfont_);
  ::ReleaseDC(nullptr, hdc_);
}

void base_mesh::dynamic_texture::text_message_writer::operator()()
{
  ::D3DLOCKED_RECT locked_rect{};
  texture_->LockRect(0, &locked_rect, nullptr, 0);

  while (write_character() && !is_message_animated_);

  texture_->UnlockRect(0);
}
bool base_mesh::dynamic_texture::text_message_writer::write_character()
{
  if (character_index_ >= message_.length()) {
    return false;
  }
  unsigned char c[2]{};
  if (message_.begin()+character_index_+1
      != message_.end()) {
    c[0] = (unsigned char)message_.at(character_index_);
    c[1] = (unsigned char)message_.at(character_index_+1); 
  } else {
    c[0] = (unsigned char)message_.at(character_index_);
  }

  ::UINT char_code{};
  bool b_ascii{};
  if (   0x81 <= c[0] && c[0] <= 0x9f
      || 0xe0 <= c[0] && c[0] <= 0xef) {
    char_code = (c[0] << 8) + c[1];
    ++character_index_;
    b_ascii = true;
  } else if (c[0] == '\n') {
    font_height_sum_ += text_metric_.tmHeight;
    font_width_sum_ = rect_.left;
    ++character_index_;
    return true;
  } else {
    char_code = c[0];
  }

  const ::UINT GGO_FLAG{GGO_GRAY4_BITMAP};
  const ::DWORD GGO_LEVEL{16};

  ::GLYPHMETRICS glyph_metrics{};
  const ::MAT2 mat{{0, 1}, {0, 0}, {0, 0}, {0, 1}};

  ::DWORD mono_font_data_size = ::GetGlyphOutline(hdc_,
                                                  char_code,
                                                  GGO_FLAG,
                                                  &glyph_metrics,
                                                  0,
                                                  nullptr,
                                                  &mat);
  std::unique_ptr<::BYTE[]> letter{new_crt ::BYTE[mono_font_data_size]};
  ::GetGlyphOutline(hdc_,
                    char_code,
                    GGO_FLAG,
                    &glyph_metrics,
                    mono_font_data_size,
                    letter.get(),
                    &mat);

  const ::UINT font_width{(glyph_metrics.gmBlackBoxX + 3) / 4 * 4};
  const ::UINT font_height{glyph_metrics.gmBlackBoxY};

  if (static_cast<::UINT>(rect_.right) < font_width_sum_ + font_width) {
    font_height_sum_ += text_metric_.tmHeight;
    font_width_sum_ = rect_.left;
    b_ascii ? character_index_ -= 2 : --character_index_;
    ++character_index_;
    return true;
  } else if (static_cast<::UINT>(rect_.bottom)
      < font_height_sum_ + font_height) {
    // TODO : page ejection
    ++character_index_;
    return false;
  }

  std::vector<::BYTE*> mono_buffer(font_height);
  for (std::size_t y{}; y < mono_buffer.size(); ++y) {
    mono_buffer.at(y) = &letter[y * font_width];
  }

  font_width_sum_ += glyph_metrics.gmptGlyphOrigin.x;
  ::UINT offset{font_height_sum_};
  offset += text_metric_.tmAscent - glyph_metrics.gmptGlyphOrigin.y;
  
  ::DWORD  new_alpha{};
  ::DWORD* texture_pixel{};
  ::DWORD  current_alpha{};
  ::DWORD  sum_alpha{};

  for (::ULONG y{}; y < font_height; ++y) {
    for (::ULONG x{}; x < font_width; ++x) {
      new_alpha     = mono_buffer[y][x] * 255 / GGO_LEVEL;
      texture_pixel = &texture_buffer_[y + offset][x + font_width_sum_];

      current_alpha = *texture_pixel & 0xff000000UL;
      current_alpha >>= 24;
      sum_alpha     = std::clamp(current_alpha+new_alpha, 0UL, 255UL);

      *texture_pixel = (sum_alpha << 24) | color_;
    }
  }
  font_width_sum_ += glyph_metrics.gmBlackBoxX;

  ++character_index_;
  return true;
}

base_mesh::dynamic_texture::text_message_writer::text_message_writer(
    std::shared_ptr<::IDirect3DDevice9>   d3d_device,
    std::shared_ptr<::IDirect3DTexture9>& texture,
    const std::string                     message,
    const bool                            is_message_animated,
    const ::RECT                          rect,
    const int                             color,
    ::UINT                                font_width_sum,
    ::UINT                                font_height_sum,
    const std::string&                    fontname,
    const int&                            size,
    const int&                            weight)
    : texture_{texture},
      message_{message},
      is_message_animated_{is_message_animated},
      rect_{rect},
      color_{color},
      font_width_sum_{font_width_sum},
      font_height_sum_{font_height_sum},
      text_metric_{},
      hdc_{},
      hfont_{},
      character_index_{},
      texture_buffer_{TEXTURE_PIXEL_SIZE}
{
  ::LPDIRECT3DTEXTURE9 p_temp_texture{};
  if (FAILED(::D3DXCreateTexture(d3d_device.get(),
                                 TEXTURE_PIXEL_SIZE,
                                 TEXTURE_PIXEL_SIZE,
                                 1,
                                 D3DUSAGE_DYNAMIC,
                                 ::D3DFMT_A8B8G8R8,
                                 ::D3DPOOL_DEFAULT,
                                 &p_temp_texture))) {
    THROW_WITH_TRACE("texture file is not found.");
  }

  texture_.reset(p_temp_texture, custom_deleter{});

  ::LOGFONT logfont = {size,
                       0,
                       0,
                       0,
                       weight,
                       0,
                       0,
                       0,
                       SHIFTJIS_CHARSET,
                       OUT_TT_ONLY_PRECIS,
                       CLIP_DEFAULT_PRECIS,
                       PROOF_QUALITY,
                       DEFAULT_PITCH | FF_MODERN};
  ::strcpy_s(logfont.lfFaceName, fontname.c_str());
  hfont_ = ::CreateFontIndirect(&logfont);
  if (hfont_ == nullptr) {
    THROW_WITH_TRACE("Failed to create message writer.");
  }

  hdc_ = ::GetDC(nullptr);
  hfont_ = static_cast<::HFONT>(::SelectObject(hdc_, hfont_));

  ::GetTextMetrics(hdc_, &text_metric_);

  ::D3DLOCKED_RECT locked_rect{};
  texture_->LockRect(0, &locked_rect, nullptr, 0);

  ::DWORD *pTexBuf = (::DWORD*)locked_rect.pBits;
  if (pTexBuf == nullptr) {
    THROW_WITH_TRACE("Failed to create message writer.");
  }
  std::fill(pTexBuf,
            pTexBuf + locked_rect.Pitch * TEXTURE_PIXEL_SIZE / sizeof(int),
            0x00000000);
  for (int y{}; y < TEXTURE_PIXEL_SIZE; ++y) {
    texture_buffer_.at(y) = &pTexBuf[y * TEXTURE_PIXEL_SIZE];
  }

  texture_->UnlockRect(0);
}

void base_mesh::set_dynamic_message(const int&         layer_number,
                                    const std::string& message,
                                    const bool&        is_animated,
                                    const ::RECT&      rect,
                                    const int&         color,
                                    const std::string& fontname,
                                    const int&         size,
                                    const int&         weight)
{
  dynamic_texture_.opacities_.at(layer_number) = 1.0f;
  dynamic_texture::text_message_writer* writer{
      new_crt dynamic_texture::text_message_writer{
          d3d_device_,
          dynamic_texture_.textures_.at(layer_number),
          message,
          is_animated,
          rect,
          color,
          static_cast<::UINT>(rect.left),
          static_cast<::UINT>(rect.top),
          fontname,
          size,
          weight }};

  effect_->SetTexture(texture_handle_.at(layer_number),
                      dynamic_texture_.textures_.at(layer_number).get());

  dynamic_texture_.writer_.at(layer_number).reset(writer);
}

void base_mesh::set_dynamic_message_color(const int& layer_number,
                                          const ::D3DXVECTOR4& color)
{
  dynamic_texture_.colors_.at(layer_number) = color;
}

void base_mesh::set_position(const ::D3DXVECTOR3& position)
{
  position_ = position;
}

void base_mesh::set_rotation(const ::D3DXVECTOR3& rotation)
{
  rotation_ = rotation;
}

void base_mesh::set_animation(const std::string& animation_set)
{
  animation_strategy_->set_animation(animation_set);
}

void base_mesh::set_default_animation(const std::string& animation_name)
{
  animation_strategy_->set_default_animation(animation_name);
}

void base_mesh::set_animation_config(const std::string& animation_name,
                                     const bool&        loop,
                                     const float&       duration)
{
  animation_strategy_->set_animation_config(animation_name, loop, duration);
}

void base_mesh::render(const ::D3DXMATRIX&  view_matrix,
                       const ::D3DXMATRIX&  projection_matrix,
                       const ::D3DXVECTOR4& light_normal,
                       const float&         brightness)
{
  effect_->SetVector(light_normal_handle_, &light_normal);
  effect_->SetFloat(brightness_handle_, brightness);

  if (dynamic_texture_.texture_shaker_) {
    (*dynamic_texture_.texture_shaker_)(*this);
  }

  effect_->SetVectorArray(texture_position_handle_,
                          &dynamic_texture_.positions_[0],
                          dynamic_texture::LAYER_NUMBER);
  effect_->SetFloatArray(texture_opacity_handle_,
                         &dynamic_texture_.opacities_[0],
                         dynamic_texture::LAYER_NUMBER);

  for (std::size_t i{}; i < dynamic_texture::LAYER_NUMBER; ++i) {
    if (dynamic_texture_.writer_.at(i)) {
      (*dynamic_texture_.writer_.at(i))();
    }
  }
  render(view_matrix, projection_matrix);
}

void base_mesh::set_shake_texture()
{
  dynamic_texture_.texture_shaker_.reset(
      new_crt base_mesh::dynamic_texture::texture_shaker());
}

const std::array<
    ::D3DXVECTOR2,
    base_mesh::dynamic_texture::texture_shaker::SHAKE_POSITIONS_SIZE>
        base_mesh::dynamic_texture::texture_shaker::SHAKING_POSITIONS = {
            ::D3DXVECTOR2{0.0f,  0.0f},
            ::D3DXVECTOR2{0.02f, 0.02f},
            ::D3DXVECTOR2{0.02f, -0.01f},
            ::D3DXVECTOR2{-0.02f, 0.02f},
            ::D3DXVECTOR2{-0.01f, -0.02f},
            ::D3DXVECTOR2{0.01f, 0.02f},
            ::D3DXVECTOR2{0.02f, -0.02f},
            ::D3DXVECTOR2{0.0f, -0.02f},
            ::D3DXVECTOR2{0.0f, 0.0f},
            ::D3DXVECTOR2{0.02f, 0.02f},
            ::D3DXVECTOR2{0.02f, -0.01f},
            ::D3DXVECTOR2{-0.02f, 0.02f},
            ::D3DXVECTOR2{-0.01f, -0.02f},
            ::D3DXVECTOR2{0.01f, 0.02f},
            ::D3DXVECTOR2{0.02f, -0.02f},
            ::D3DXVECTOR2{0.0f, -0.02f},
};

const int base_mesh::dynamic_texture::texture_shaker::SHAKE_FRAME = 4;
const int base_mesh::dynamic_texture::texture_shaker::SHAKE_DURATION = 30;


base_mesh::dynamic_texture::texture_shaker::texture_shaker()
  : count_{0},
  current_position_{0.0f, 0.0f},
  previous_position_{0.0f, 0.0f} {}

void base_mesh::dynamic_texture::texture_shaker::operator()(
    base_mesh& base_mesh)
{
  if (count_ > SHAKE_DURATION) {
    return;
  } else if (count_ == SHAKE_DURATION) {
    for (int i{}; i < LAYER_NUMBER; ++i) {
      base_mesh.set_dynamic_texture_position(i, ::D3DXVECTOR2{0.0f, 0.0f});
    }
    count_ = SHAKE_DURATION+1;
    return;
  }

  if (count_ % SHAKE_FRAME == 0) {
    int shaking_positions_index{count_/SHAKE_FRAME%(SHAKE_POSITIONS_SIZE-1)};
    previous_position_ = SHAKING_POSITIONS.at(shaking_positions_index);
    current_position_ = SHAKING_POSITIONS.at(shaking_positions_index+1);
  }

  // 1/4 -> 2/4 -> 3/4 -> 4/4 -> 1/4 -> 2/4 ->...
  float loop_counter{static_cast<float>(count_%SHAKE_FRAME+1)/SHAKE_FRAME};
  for (int i{}; i < LAYER_NUMBER; ++i) {
    base_mesh.set_dynamic_texture_position(i,
        previous_position_ +
        (current_position_ - previous_position_)*loop_counter);
  }

  ++count_;
}

}
