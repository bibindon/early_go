#ifndef TEXTURE_ANIMATOR_HPP
#define TEXTURE_ANIMATOR_HPP
#include "stdafx.hpp"

namespace early_go {

struct message_writer {
  message_writer(std::shared_ptr<::IDirect3DDevice9>,
                      std::shared_ptr<::IDirect3DTexture9>&,
                      const std::string,
                      const bool,
                      const cv::Rect,
                      const int,
                      const std::string&,
                      const int&,
                      const int&);
  ~message_writer();
  void operator()();
  bool write_character(::D3DLOCKED_RECT&);

private:
  std::shared_ptr<::IDirect3DTexture9>& texture_;
  const std::string                     message_;
  const bool                            is_message_animated_;
  const cv::Rect                        rect_;
  const cv::Point                       texture_size_;
  const int                             color_;
  ::LONG                                font_width_sum_;
  ::LONG                                font_height_sum_;
  ::TEXTMETRIC                          text_metric_;
  ::HDC                                 hdc_;
  ::HFONT                               hfont_;
  std::size_t                           character_index_;
  std::vector<::DWORD*>                 texture_buffer_;
  static constexpr int                  MESSAGE_SPEED{3};
};
} /* namespace early_go */
#endif
