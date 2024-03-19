#ifndef TEXT_THREAD_VER_HPP
#define TEXT_THREAD_VER_HPP
#include "stdafx.hpp"

namespace early_go
{
struct message_writer_for_thread
{
    DWORD color_{};
    size_t character_index_{};
    HDC hdc_{};
    HFONT hfont_{};
    HFONT old_font_{};
    cv::Point start_point_{};
    TEXTMETRIC text_metric_{};
    cv::Size canvas_size_{};
    LONG font_width_sum_{};
    LONG font_height_sum_{};
    std::vector<std::vector<DWORD>> text_image_{};
    std::string message_{};
    message_writer_for_thread(const std::string &, const int, const cv::Size &);
    ~message_writer_for_thread();
    void add_text(const std::string, const cv::Point &, const DWORD);
    bool write_one_character();
};
} // namespace early_go 
#endif
