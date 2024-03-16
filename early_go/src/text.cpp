#include "stdafx.hpp"

#include "text.hpp"

namespace early_go
{

    message_writer::message_writer(std::shared_ptr<IDirect3DTexture9> &texture,
                                   const std::string message,
                                   const bool is_message_animated,
                                   const cv::Rect rect,
                                   const DWORD color,
                                   const std::string &fontname,
                                   const int &size,
                                   const int &weight,
                                   const BYTE &charset,
                                   const bool &proportional)
        : texture_{texture},
          message_{message},
          is_message_animated_{is_message_animated},
          rect_{rect},
          texture_size_{get_next_pow_2(rect.width),
                        get_next_pow_2(rect.height)},
          color_{color},
          font_width_sum_{rect.x},
          font_height_sum_{rect.y},
          text_metric_{},
          hdc_{},
          hfont_{},
          character_index_{},
          texture_buffer_{texture_size_.height},
          proportional_{proportional}
    {
        initialize(size, weight, charset, fontname);
    }

    message_writer::message_writer(std::shared_ptr<IDirect3DDevice9> d3d_device,
                                   std::shared_ptr<IDirect3DTexture9> &texture,
                                   const std::string message,
                                   const bool is_message_animated,
                                   const cv::Rect rect,
                                   const DWORD color,
                                   const std::string &fontname,
                                   const int &size,
                                   const int &weight,
                                   const BYTE &charset,
                                   const bool &proportional)
        : message_writer{d3d_device, texture, message, is_message_animated, rect,
                         cv::Size{constants::TEXTURE_PIXEL_SIZE,
                                  constants::TEXTURE_PIXEL_SIZE},
                         color, fontname, size, weight, charset, proportional} {}

    message_writer::message_writer(std::shared_ptr<IDirect3DDevice9> d3d_device,
                                   std::shared_ptr<IDirect3DTexture9> &texture,
                                   const std::string message,
                                   const bool is_message_animated,
                                   const cv::Rect rect,
                                   const cv::Size texture_size,
                                   const DWORD color,
                                   const std::string &fontname,
                                   const int &size,
                                   const int &weight,
                                   const BYTE &charset,
                                   const bool &proportional)
        : texture_{texture},
          message_{message},
          is_message_animated_{is_message_animated},
          rect_{rect},
          texture_size_{texture_size},
          color_{color},
          font_width_sum_{rect.x},
          font_height_sum_{rect.y},
          text_metric_{},
          hdc_{},
          hfont_{},
          character_index_{},
          texture_buffer_{texture_size_.height},
          proportional_{proportional}
    {
        LPDIRECT3DTEXTURE9 p_temp_texture{};

        if (FAILED(D3DXCreateTexture(d3d_device.get(),
                                     texture_size.height,
                                     texture_size.width,
                                     1,
                                     D3DUSAGE_DYNAMIC,
                                     D3DFMT_A8R8G8B8,
                                     D3DPOOL_DEFAULT,
                                     &p_temp_texture)))
        {
            THROW_WITH_TRACE("texture file is not found.");
        }

        texture_.reset(p_temp_texture, custom_deleter{});

        initialize(size, weight, charset, fontname);

        D3DLOCKED_RECT locked_rect{};
        texture_->LockRect(0, &locked_rect, nullptr, 0);

        DWORD *pTexBuf = (DWORD *)locked_rect.pBits;
        if (pTexBuf == nullptr)
        {
            THROW_WITH_TRACE("Failed to create message writer.");
        }
        std::fill(pTexBuf,
                  pTexBuf + static_cast<std::size_t>(locked_rect.Pitch) *
                                texture_size_.height / sizeof(DWORD),
                  0x00000000UL);

        texture_->UnlockRect(0);
    }
    void message_writer::initialize(const int &size,
                                    const int &weight,
                                    const BYTE &charset,
                                    const std::string &fontname)
    {
        LOGFONT logfont = {size,
                           0,
                           0,
                           0,
                           weight,
                           0,
                           0,
                           0,
                           charset,
                           0,
                           OUT_TT_ONLY_PRECIS,
                           CLIP_DEFAULT_PRECIS,
                           PROOF_QUALITY,
                           DEFAULT_PITCH | FF_DONTCARE};
        strcpy_s(logfont.lfFaceName, fontname.c_str());
        hfont_ = CreateFontIndirect(&logfont);
        if (hfont_ == nullptr)
        {
            THROW_WITH_TRACE("Failed to create message writer.");
        }

        hdc_ = GetDC(nullptr);
        old_font_ = static_cast<::HFONT>(::SelectObject(hdc_, hfont_));

        GetTextMetrics(hdc_, &text_metric_);
    }

    message_writer::~message_writer()
    {
        SelectObject(hdc_, old_font_);
        DeleteObject(hfont_);
        ReleaseDC(nullptr, hdc_);
    }

    bool message_writer::operator()()
    {
        D3DLOCKED_RECT locked_rect{};
        texture_->LockRect(0, &locked_rect, nullptr, 0);

        if (is_message_animated_)
        {
            for (int i{}; i < MESSAGE_SPEED; ++i)
            {
                if (!write_character(locked_rect))
                {
                    log_liner{} << "message_finished";
                    break;
                }
            }
        }
        else
        {
            while (write_character(locked_rect))
                ;
        }

        texture_->UnlockRect(0);
        return is_finished_;
    }

    bool message_writer::operator()(D3DLOCKED_RECT &locked_rect)
    {
        if (is_message_animated_)
        {
            for (int i{}; i < MESSAGE_SPEED; ++i)
            {
                if (!write_character(locked_rect))
                {
                    break;
                }
            }
        }
        else
        {
            while (write_character(locked_rect))
                ;
        }
        return is_finished_;
    }

    bool message_writer::write_character(D3DLOCKED_RECT &locked_rect)
    {
        DWORD *pTexBuf = (DWORD *)locked_rect.pBits;
        if (pTexBuf != texture_buffer_.at(0))
        {
            for (int y{}; y < texture_size_.height; ++y)
            {
                // texture_buffer_.at(y) = &pTexBuf[y * texture_size_.width];
                texture_buffer_.at(y) = &pTexBuf[y * locked_rect.Pitch / 4];
            }
        }

        if (character_index_ >= message_.length())
        {
            is_finished_ = true;
            return false;
        }
        unsigned char c[2]{};
        // Copy 2 bytes because shiftjis chara code.
        // But if the read position is last index, do not read 2nd character because
        // it is outside of "message_". So at that time, read 1 byte.
        if (message_.begin() + character_index_ + 1 != message_.end())
        {
            c[0] = (unsigned char)message_.at(character_index_);
            c[1] = (unsigned char)message_.at(character_index_ + 1);
        }
        else
        {
            c[0] = (unsigned char)message_.at(character_index_);
        }

        UINT char_code{};
        bool b_ascii{};
        // If 1st byte is not ascii, combine c[0] and c[1].
        if ((0x81 <= c[0] && c[0] <= 0x9f) || (0xe0 <= c[0] && c[0] <= 0xef))
        {
            char_code = (c[0] << 8) + c[1];
            ++character_index_;
        }
        else if (c[0] == '\n')
        {
            font_height_sum_ += text_metric_.tmHeight;
            font_height_sum_ += LINE_SPACING;
            font_width_sum_ = rect_.x;
            ++character_index_;
            return true;
        }
        else
        {
            char_code = c[0];
            b_ascii = true;
        }

        const UINT GGO_FLAG{GGO_GRAY4_BITMAP};
        const DWORD GGO_LEVEL{GGO_FLAG == GGO_GRAY2_BITMAP   ? 4
                              : GGO_FLAG == GGO_GRAY4_BITMAP ? 16
                              : GGO_FLAG == GGO_GRAY8_BITMAP ? 64
                                                             : 16};

        GLYPHMETRICS glyph_metrics{};
        const MAT2 mat{{0, 1}, {0, 0}, {0, 0}, {0, 1}};

        DWORD mono_font_data_size = GetGlyphOutline(
            hdc_, char_code, GGO_FLAG, &glyph_metrics, 0, nullptr, &mat);

        std::unique_ptr<::BYTE[]> letter{new_crt BYTE[mono_font_data_size]};
        GetGlyphOutline(hdc_, char_code, GGO_FLAG, &glyph_metrics,
                        mono_font_data_size, letter.get(), &mat);

        // If char_code is space, add font_width_sum_ some width and return.
        if (char_code == 0x20)
        {
            if (proportional_)
            {
                font_width_sum_ += glyph_metrics.gmCellIncX - 1;
            }
            else
            {
                if (text_metric_.tmHeight % 2 == 1)
                {
                    font_width_sum_ += (text_metric_.tmHeight + 1) / 2;
                }
                else
                {
                    font_width_sum_ += text_metric_.tmHeight / 2;
                }
            }
            if (rect_.width < font_width_sum_)
            {
                font_height_sum_ += text_metric_.tmHeight;
                font_height_sum_ += LINE_SPACING;
                font_width_sum_ = rect_.x;
                return true;
            }
            ++character_index_;
            return true;
        }

        // Align with a multiple of 4.
        const UINT font_width{(glyph_metrics.gmBlackBoxX + 3) / 4 * 4};
        const UINT font_height{glyph_metrics.gmBlackBoxY};

        // If character may go outside over leftmost, return.
        if (static_cast<UINT>(rect_.width) < font_width_sum_ + font_width)
        {
            font_height_sum_ += text_metric_.tmHeight;
            font_height_sum_ += LINE_SPACING;
            font_width_sum_ = rect_.x;
            b_ascii ? --character_index_ : character_index_ -= 2;
            ++character_index_;
            return true;
        }
        // If character may go down over bottom, scroll one line? and return.
        else if (rect_.height < font_height_sum_ + text_metric_.tmHeight)
        {
            for (std::size_t y{0}; y < static_cast<std::size_t>(rect_.height); ++y)
            {
                for (std::size_t x{0}; x < static_cast<std::size_t>(rect_.width); ++x)
                {
                    if (y + text_metric_.tmHeight < rect_.height)
                    {
                        texture_buffer_.at(y)[x] =
                            texture_buffer_.at(y + text_metric_.tmHeight)[x];
                    }
                    else
                    {
                        texture_buffer_.at(y)[x] = 0x00000000;
                    }
                }
            }
            font_height_sum_ -= text_metric_.tmHeight;
            return false;
        }

        std::vector<::BYTE *> mono_buffer(font_height);
        for (std::size_t y{}; y < mono_buffer.size(); ++y)
        {
            mono_buffer.at(y) = &letter[y * font_width];
        }

        font_width_sum_ += glyph_metrics.gmptGlyphOrigin.x;
        LONG offset{font_height_sum_};
        offset += text_metric_.tmAscent - glyph_metrics.gmptGlyphOrigin.y;

        DWORD new_alpha{};
        float chara_alpha{0.0f};
        DWORD *texture_pixel{};
        DWORD current_alpha{};
        DWORD sum_alpha{};

        for (std::size_t y{}; y < font_height; ++y)
        {
            for (std::size_t x{}; x < font_width; ++x)
            {
                new_alpha = color_ >> 24;
                chara_alpha = static_cast<float>(mono_buffer.at(y)[x]) / GGO_LEVEL;
                new_alpha = static_cast<int>(new_alpha * chara_alpha);

                texture_pixel = &texture_buffer_.at(y + offset)[x + font_width_sum_];

                current_alpha = *texture_pixel & 0xff000000UL;
                current_alpha >>= 24;

                sum_alpha = std::min(std::max(current_alpha + new_alpha, 0UL), 255UL);

                *texture_pixel =
                    (static_cast<int>(sum_alpha) << 24) | (0x00ffffffUL & color_);
            }
        }
        font_width_sum_ += glyph_metrics.gmCellIncX - 1;

        ++character_index_;
        return true;
    }

    void add_text(std::shared_ptr<IDirect3DTexture9> &texture,
                  const std::string &text,
                  const cv::Rect &rect,
                  const DWORD &color,
                  const std::string &fontname,
                  const int &size,
                  const int &weight,
                  D3DLOCKED_RECT &locked_rect,
                  const BYTE &charset,
                  const bool &proportional)
    {
        message_writer writer{texture, text, false, rect, color,
                              fontname, size, weight, charset, proportional};
        writer(locked_rect);
    }

} // namespace early_go
