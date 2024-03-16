#include "stdafx.hpp"

#include "text_thread_ver.hpp"

namespace early_go
{
    message_writer_for_thread::message_writer_for_thread(
        const std::string &font_name, const int font_size,
        const cv::Size &canvas_size)
    {
        canvas_size_ = canvas_size;
        LOGFONT logfont = {font_size,
                           0,
                           0,
                           0,
                           FW_NORMAL,
                           0,
                           0,
                           0,
                           SHIFTJIS_CHARSET,
                           0,
                           OUT_TT_ONLY_PRECIS,
                           CLIP_DEFAULT_PRECIS,
                           PROOF_QUALITY,
                           DEFAULT_PITCH | FF_DONTCARE};
        strcpy_s(logfont.lfFaceName, font_name.c_str());
        hfont_ = CreateFontIndirect(&logfont);
        if (hfont_ == nullptr)
        {
            THROW_WITH_TRACE("Failed to CreateFontIndirect().");
        }

        hdc_ = GetDC(nullptr);
        old_font_ = static_cast<::HFONT>(::SelectObject(hdc_, hfont_));
        GetTextMetrics(hdc_, &text_metric_);

        text_image_.resize(std::size_t(canvas_size_.height),
                           std::vector<DWORD>(canvas_size_.width));
    }
    message_writer_for_thread::~message_writer_for_thread()
    {
        SelectObject(hdc_, old_font_);
        DeleteObject(hfont_);
        ReleaseDC(nullptr, hdc_);
    }
    void message_writer_for_thread::add_text(const std::string message,
                                             const cv::Point &start_point,
                                             const DWORD color)
    {
        message_ = message;
        start_point_ = start_point;
        color_ = color;
        character_index_ = 0;
        font_height_sum_ = 0;
        font_width_sum_ = 0;
        for (int i{}; i < canvas_size_.height; ++i)
        {
            std::fill(text_image_[i].begin(), text_image_[i].end(), 0);
        }

        while (write_one_character())
            ;
    }
    bool message_writer_for_thread::write_one_character()
    {
        const bool proportional = false;

        if (character_index_ >= message_.length())
        {
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
            font_width_sum_ = 0;
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
            // proportional font's space width may be 0.
            if (proportional)
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
            if (canvas_size_.width < font_width_sum_)
            {
                font_height_sum_ += text_metric_.tmHeight;
                font_width_sum_ = start_point_.x;
                return true;
            }
            ++character_index_;
            return true;
        }

        // Align with a multiple of 4.
        const UINT font_width{(glyph_metrics.gmBlackBoxX + 3) / 4 * 4};
        const UINT font_height{glyph_metrics.gmBlackBoxY};

        // If character may go outside over leftmost, terminate.
        if (static_cast<UINT>(canvas_size_.width) < font_width_sum_ + font_width)
        {
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

        DWORD origin_alpha{color_ >> 24};
        DWORD new_alpha{};
        float chara_alpha_ratio{0.0f};
        DWORD *texture_pixel{};
        DWORD current_alpha{};
        DWORD sum_alpha{};

        for (std::size_t y{}; y < font_height; ++y)
        {
            for (std::size_t x{}; x < font_width; ++x)
            {
                // If blank pixel, through.
                BYTE chara_alpha = mono_buffer.at(y)[x];
                if (chara_alpha == 0)
                {
                    continue;
                }
                // Get character alpha value(0.0 ~ 1.0)
                chara_alpha_ratio = static_cast<float>(mono_buffer.at(y)[x]) / GGO_LEVEL;
                // If user requests alpha 255 and character alpha ratio is 0.5,
                // new_alpha should be set 127.
                new_alpha = static_cast<int>(origin_alpha * chara_alpha_ratio);

                // Get target pixel pointer
                texture_pixel = &text_image_[y + offset][x + font_width_sum_];

                // Get current alpha
                current_alpha = *texture_pixel & 0xff000000UL;
                current_alpha >>= 24;

                // Add current and new alpha, and clamp 0 and 255.
                sum_alpha = std::min(std::max(current_alpha + new_alpha, 0UL), 255UL);

                *texture_pixel = (sum_alpha) << 24 | (0x00ffffffUL & color_);
            }
        }
        font_width_sum_ += glyph_metrics.gmCellIncX - 1;

        ++character_index_;
        return true;
    }

} // namespace early_go
