#include "texture_animator.hpp"

namespace early_go {

message_writer::message_writer(std::shared_ptr<::IDirect3DDevice9> d3d_device,
                               std::shared_ptr<::IDirect3DTexture9>& texture,
                               const std::string message,
                               const bool is_message_animated,
                               const cv::Rect rect, const int color,
                               const std::string& fontname, const int& size,
                               const int& weight)
    : texture_{texture},
      message_{message},
      is_message_animated_{is_message_animated},
      rect_{rect},
      texture_size_{get_next_pow_2(rect.width), get_next_pow_2(rect.height)},
      color_{color},
      font_width_sum_{rect.x},
      font_height_sum_{rect.y},
      text_metric_{},
      hdc_{},
      hfont_{},
      character_index_{},
      texture_buffer_{texture_size_.y} {
  ::LPDIRECT3DTEXTURE9 p_temp_texture{};
  if (FAILED(::D3DXCreateTexture(d3d_device.get(),
                                 texture_size_.x,
                                 texture_size_.y,
                                 1,
                                 D3DUSAGE_DYNAMIC,
                                 ::D3DFMT_A8R8G8B8,
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
                       0,
                       OUT_TT_ONLY_PRECIS,
                       CLIP_DEFAULT_PRECIS,
                       PROOF_QUALITY,
                       DEFAULT_PITCH | FF_DONTCARE};
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

  ::DWORD* pTexBuf = (::DWORD*)locked_rect.pBits;
  if (pTexBuf == nullptr) {
    THROW_WITH_TRACE("Failed to create message writer.");
  }
  std::fill(pTexBuf,
            pTexBuf + static_cast<std::size_t>(locked_rect.Pitch) *
                          texture_size_.y / sizeof(int),
            0x00000000);
  for (int y{}; y < texture_size_.y; ++y) {
    texture_buffer_.at(y) = &pTexBuf[y * texture_size_.x];
  }

  texture_->UnlockRect(0);
}
message_writer::~message_writer() {
  ::SelectObject(hdc_, hfont_);
  ::ReleaseDC(nullptr, hdc_);
}

void message_writer::operator()() {
  ::D3DLOCKED_RECT locked_rect{};
  texture_->LockRect(0, &locked_rect, nullptr, 0);

  if (is_message_animated_) {
    for (int i{}; i < MESSAGE_SPEED; ++i) {
      if (!write_character(locked_rect)) {
        break;
      }
    }
  } else {
    while(write_character(locked_rect));
  }

  texture_->UnlockRect(0);
}

bool message_writer::write_character(::D3DLOCKED_RECT& locked_rect) {
  ::DWORD* pTexBuf = (::DWORD*)locked_rect.pBits;
  for (int y{}; y < texture_size_.y; ++y) {
    texture_buffer_.at(y) = &pTexBuf[y * texture_size_.x];
  }

  if (character_index_ >= message_.length()) {
    return false;
  }
  unsigned char c[2]{};
  if (message_.begin() + character_index_ + 1 != message_.end()) {
    c[0] = (unsigned char)message_.at(character_index_);
    c[1] = (unsigned char)message_.at(character_index_ + 1);
  } else {
    c[0] = (unsigned char)message_.at(character_index_);
  }

  ::UINT char_code{};
  bool b_ascii{};
  if ((0x81 <= c[0] && c[0] <= 0x9f) || (0xe0 <= c[0] && c[0] <= 0xef)) {
    char_code = (c[0] << 8) + c[1];
    ++character_index_;
    b_ascii = true;
  } else if (c[0] == '\n') {
    font_height_sum_ += text_metric_.tmHeight;
    font_width_sum_ = rect_.x;
    ++character_index_;
    return true;
  } else {
    char_code = c[0];
  }

  const ::UINT GGO_FLAG{GGO_GRAY4_BITMAP};
  const ::DWORD GGO_LEVEL{GGO_FLAG == GGO_GRAY2_BITMAP ? 4
                          : GGO_FLAG == GGO_GRAY4_BITMAP ? 16
                          : GGO_FLAG == GGO_GRAY8_BITMAP ? 64
                          : 16};

  ::GLYPHMETRICS glyph_metrics{};
  const ::MAT2 mat{{0, 1}, {0, 0}, {0, 0}, {0, 1}};

  ::DWORD mono_font_data_size = ::GetGlyphOutline(
      hdc_, char_code, GGO_FLAG, &glyph_metrics, 0, nullptr, &mat);
  std::unique_ptr<::BYTE[]> letter{new_crt ::BYTE[mono_font_data_size]};
  ::GetGlyphOutline(hdc_, char_code, GGO_FLAG, &glyph_metrics,
                    mono_font_data_size, letter.get(), &mat);

  if (char_code == 0x20) {
    font_width_sum_ += glyph_metrics.gmCellIncX;
    if (rect_.width < font_width_sum_) {
      font_height_sum_ += text_metric_.tmHeight;
      font_width_sum_ = rect_.x;
      return true;
    }
    ++character_index_;
    return true;
  }

  const ::UINT font_width{(glyph_metrics.gmBlackBoxX + 3) / 4 * 4};
  const ::UINT font_height{glyph_metrics.gmBlackBoxY};

  if (static_cast<::UINT>(rect_.width) < font_width_sum_ + font_width) {
    font_height_sum_ += text_metric_.tmHeight;
    font_width_sum_ = rect_.x;
    b_ascii ? character_index_ -= 2 : --character_index_;
    ++character_index_;
    return true;
  } else if (rect_.height < font_height_sum_ + text_metric_.tmHeight) {
    for (std::size_t y{0}; y < rect_.height; ++y) {
      for (std::size_t x{0}; x < rect_.width; ++x) {
        if (y+text_metric_.tmHeight < rect_.height) {
          texture_buffer_.at(y)[x] =
              texture_buffer_.at(y+text_metric_.tmHeight)[x];
        } else {
          texture_buffer_.at(y)[x] = 0x00000000;
        }
      }
    }
    font_height_sum_ -= text_metric_.tmHeight;
    ++character_index_;
    return false;
  }

  std::vector<::BYTE*> mono_buffer(font_height);
  for (std::size_t y{}; y < mono_buffer.size(); ++y) {
    mono_buffer.at(y) = &letter[y * font_width];
  }

  font_width_sum_ += glyph_metrics.gmptGlyphOrigin.x;
  ::LONG offset{font_height_sum_};
  offset += text_metric_.tmAscent - glyph_metrics.gmptGlyphOrigin.y;

  ::DWORD new_alpha{};
  ::DWORD* texture_pixel{};
  ::DWORD current_alpha{};
  ::DWORD sum_alpha{};

  for (std::size_t y{}; y < font_height; ++y) {
    for (std::size_t x{}; x < font_width; ++x) {
      new_alpha = mono_buffer.at(y)[x] * 255 / GGO_LEVEL;
      texture_pixel = &texture_buffer_.at(y + offset)[x + font_width_sum_];

      current_alpha = *texture_pixel & 0xff000000UL;
      current_alpha >>= 24;
      // C++17
      // sum_alpha     = std::clamp(current_alpha+new_alpha, 0UL, 255UL);
      sum_alpha = std::min(std::max(current_alpha + new_alpha, 0UL), 255UL);

      *texture_pixel = (sum_alpha << 24) | color_;
    }
  }
  font_width_sum_ += glyph_metrics.gmCellIncX - 1;

  ++character_index_;
  return true;
}

}  // namespace early_go
