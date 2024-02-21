#ifndef TEXT_HPP
#define TEXT_HPP
#include "stdafx.hpp"

namespace early_go {

struct message_writer {
  message_writer(std::shared_ptr<::IDirect3DTexture9>&,
                 const std::string,
                 const bool,
                 const cv::Rect,
                 const ::DWORD,
                 const std::string&,
                 const int&,
                 const int&,
                 const ::BYTE&,
                 const bool&);
  message_writer(std::shared_ptr<::IDirect3DDevice9>,
                 std::shared_ptr<::IDirect3DTexture9>&,
                 const std::string,
                 const bool,
                 const cv::Rect,
                 const ::DWORD,
                 const std::string&,
                 const int&,
                 const int&,
                 const ::BYTE&,
                 const bool&);
  message_writer(std::shared_ptr<::IDirect3DDevice9>,
                 std::shared_ptr<::IDirect3DTexture9>&,
                 const std::string,
                 const bool,
                 const cv::Rect,
                 const cv::Size,
                 const ::DWORD,
                 const std::string&,
                 const int&,
                 const int&,
                 const ::BYTE&,
                 const bool&);
  ~message_writer();
  bool operator()(); // heavy
  bool operator()(::D3DLOCKED_RECT&); // light
  bool write_character(::D3DLOCKED_RECT&);

private:
  std::shared_ptr<::IDirect3DTexture9>& texture_;
  const std::string                     message_;
  const bool                            is_message_animated_;
  const cv::Rect                        rect_;
  const cv::Size                        texture_size_;
  const ::DWORD                         color_;
  ::LONG                                font_width_sum_;
  ::LONG                                font_height_sum_;
  ::TEXTMETRIC                          text_metric_;
  ::HDC                                 hdc_;
  ::HFONT                               hfont_;
  ::HFONT                               old_font_;
  std::size_t                           character_index_;
  std::vector<::DWORD*>                 texture_buffer_;
  const bool                            proportional_;
  static constexpr int                  MESSAGE_SPEED{1};
  static constexpr int                  LINE_SPACING{12};
  bool                                  is_finished_{false};

  void initialize(const int&, const int&, const ::BYTE&, const std::string&);
};

void add_text(std::shared_ptr<::IDirect3DTexture9>&,
              const std::string&,
              const cv::Rect&,
              const ::DWORD&,
              const std::string&,
              const int&,
              const int&,
              ::D3DLOCKED_RECT&,
              const ::BYTE&,
              const bool&);

} /* namespace early_go */
#endif
