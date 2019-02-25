#ifndef HUD_HPP
#define HUD_HPP
#include "stdafx.hpp"

namespace early_go {

struct message_writer;

class hud final {
public:
  hud(const std::shared_ptr<::IDirect3DDevice9>&);
  void add_image(const std::string&, const std::string&, const cv::Point&);
  void delete_image(const std::string&);
  void add_message(const std::string&, const std::string&, const cv::Rect&);
  void delete_message(const std::string&);
  void add_frame(const std::string&, const cv::Rect&, const cv::Scalar&);
  void delete_frame(const std::string&);
  void add_message_in_frame(const std::string&,
                            const std::string&, const std::string&);
  void operator()();
private:
  std::shared_ptr<::IDirect3DDevice9>  d3d_device_;
  std::shared_ptr<::ID3DXSprite>       sprite_;
  //std::shared_ptr<message_writer>      message_writer_;

  void create_round_rect(::LPDIRECT3DTEXTURE9&,
      const cv::Point&, const cv::Scalar&);

  struct texture {
    const std::string                    id_;
    std::shared_ptr<::IDirect3DTexture9> value_;
    cv::Rect                             rect_;
  };
  std::list<texture> textures_;

  struct message_info {
    decltype(hud::textures_)::iterator texture_;
    std::shared_ptr<message_writer>    message_writer_;
  };
  std::list<message_info> message_list_;

  struct frame_animator;
  struct message_frame {
    hud&                               outer_;
    decltype(hud::textures_)::iterator texture_;
    const cv::Point                    size_dest_;
    const cv::Scalar                   color_;
    std::shared_ptr<frame_animator>    frame_animator_;
  };
  std::list<message_frame> frame_list_;

  struct frame_animator {
    frame_animator();
    virtual bool operator()(message_frame&);
    virtual ~frame_animator(){};
    const static float LENGTH;
    int count_;
  };
  struct frame_animator_delete : frame_animator {
    frame_animator_delete();
    virtual bool operator()(message_frame&);
  };
  const static int EDGE_CIRCLE_RADIUS;
};
} /* namespace early_go */
#endif
