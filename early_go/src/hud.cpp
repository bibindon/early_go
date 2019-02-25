#include "hud.hpp"
#include "texture_animator.hpp"

namespace early_go {

const int hud::EDGE_CIRCLE_RADIUS{3};
hud::hud(const std::shared_ptr<::IDirect3DDevice9>& d3d_device)
  : d3d_device_{d3d_device}
{
  ::LPD3DXSPRITE temp_sprite;
  if (FAILED(::D3DXCreateSprite(d3d_device_.get(), &temp_sprite))) {
    THROW_WITH_TRACE("Failed to create a sprite.");
  }
  sprite_.reset(temp_sprite, custom_deleter{});
}

void hud::create_round_rect(::LPDIRECT3DTEXTURE9& texture,
    const cv::Point& size, const cv::Scalar& color)
{
  int width_next_pow_2{get_next_pow_2(size.x)};
  int height_next_pow_2{get_next_pow_2(size.y)};
  cv::Mat round_rect{cv::Mat::zeros(width_next_pow_2,
                                    width_next_pow_2,
                                    CV_8UC4)};

  cv::circle(round_rect,
             cv::Point(EDGE_CIRCLE_RADIUS, EDGE_CIRCLE_RADIUS),
             EDGE_CIRCLE_RADIUS,
             color, -1, CV_AA);
  cv::circle(round_rect,
             cv::Point(size.x - EDGE_CIRCLE_RADIUS,
                       EDGE_CIRCLE_RADIUS),
             EDGE_CIRCLE_RADIUS,
             color, -1, CV_AA);
  cv::circle(round_rect,
             cv::Point(EDGE_CIRCLE_RADIUS,
                       size.y - EDGE_CIRCLE_RADIUS),
             EDGE_CIRCLE_RADIUS,
             color, -1, CV_AA);
  cv::circle(round_rect,
             cv::Point(size.x - EDGE_CIRCLE_RADIUS,
                       size.y - EDGE_CIRCLE_RADIUS),
             EDGE_CIRCLE_RADIUS,
             color, -1, CV_AA);

  cv::rectangle(round_rect,
                cv::Rect(EDGE_CIRCLE_RADIUS, 0,
                         size.x - EDGE_CIRCLE_RADIUS*2 + 1, size.y + 1),
                color, -1, CV_AA);

  cv::rectangle(round_rect,
                cv::Rect(0, EDGE_CIRCLE_RADIUS,
                         size.x + 1, size.y - EDGE_CIRCLE_RADIUS*2 + 1),
                color, -1, CV_AA);

  std::vector<::uchar> cv_buffer{};
  cv::imencode(".bmp", round_rect, cv_buffer);
  if (FAILED(::D3DXCreateTextureFromFileInMemory(
      d3d_device_.get(),
      &cv_buffer[0],
      static_cast<::UINT>(cv_buffer.size()),
      &texture))) {
    THROW_WITH_TRACE("Failed create round rect.");
  }
}

void hud::add_image(const std::string& id,
                    const std::string& filename,
                    const cv::Point& position)
{
  auto it{std::find_if(textures_.begin(), textures_.end(),
      [&](auto&& x){return x.id_ == id;})};

  if (it != textures_.end()) {
    return;
  }

  std::vector<char> buffer = get_resource(
      "SELECT DATA FROM IMAGE WHERE FILENAME = '" + filename + "';");
  ::LPDIRECT3DTEXTURE9 temp_texture{};
  if (FAILED(::D3DXCreateTextureFromFileInMemory(
      d3d_device_.get(),
      &buffer[0],
      static_cast<::UINT>(buffer.size()), &temp_texture))) {
    THROW_WITH_TRACE("Failed to create a texture.");
  }

  ::D3DSURFACE_DESC desc{};
  if (FAILED(temp_texture->GetLevelDesc(0, &desc))) {
    THROW_WITH_TRACE("Failed to create a texture.");
  }
  int width_next_pow_2{get_next_pow_2(desc.Width)};
  int height_next_pow_2{get_next_pow_2(desc.Height)};

  textures_.emplace_back(
      texture{
          id,
          std::shared_ptr<::IDirect3DTexture9>(temp_texture, custom_deleter{}),
          cv::Rect(position.x, position.y,
                   width_next_pow_2, height_next_pow_2)});
}

void hud::delete_image(const std::string& id)
{
  decltype(textures_)::iterator it{
      std::find_if(textures_.begin(), textures_.end(),
                   [&](auto&& x){return x.id_ == id;})};
  if (it != textures_.end()) {
    textures_.erase(it);
  }
}

void hud::add_message_in_frame(const std::string& message_id,
                               const std::string& frame_id,
                               const std::string& message)
{
  auto it{std::find_if(textures_.begin(), textures_.end(),
      [&](auto&& x){return x.id_ == frame_id;})};

  if (it == textures_.end()) {
    return;
  }

  add_message(message_id, message,
              cv::Rect(it->rect_.x+10, it->rect_.y+10,
                       it->rect_.width-20, it->rect_.height-20));
}

void hud::add_message(const std::string& id,
                      const std::string& message, 
                      const cv::Rect& rect)
{
  auto it{std::find_if(textures_.begin(), textures_.end(),
      [&](auto&& x){return x.id_ == id;})};

  if (it != textures_.end()) {
    return;
  }

  texture temp_texture{id,
                       std::shared_ptr<::IDirect3DTexture9>(),
                       rect};
  textures_.emplace_back(temp_texture);

  std::shared_ptr<message_writer> writer(
      std::make_shared<message_writer>(
          d3d_device_,
          textures_.back().value_,
          message,
          true,
          cv::Rect(0, 0, rect.width, rect.height),
          D3DCOLOR_ARGB(0, 190, 190, 210),
          "ü‡ÉSÉVÉbÉN",
          //"consolas",
          //"Ricty Diminished",
          //"ÇlÇrÅ@ÇoÉSÉVÉbÉN",
          20,
          0));
          //FW_EXTRALIGHT);

  message_list_.emplace_back(message_info{--textures_.end(), writer});
}

void hud::delete_message(const std::string& id)
{
  decltype(textures_)::iterator it{
      std::find_if(textures_.begin(), textures_.end(),
                   [&](auto&& x){return x.id_ == id;})};
  if (it != textures_.end()) {
    decltype(message_list_)::iterator message{
        std::find_if(message_list_.begin(), message_list_.end(),
                     [&](auto&& x){return x.texture_ == it;})};
    message_list_.erase(message);
    textures_.erase(it);
  }
}

void hud::add_frame(
    const std::string& id, const cv::Rect& rect, const cv::Scalar& color)
{
  auto it{std::find_if(textures_.begin(), textures_.end(),
      [&](auto&& x){return x.id_ == id;})};

  if (it != textures_.end()) {
    return;
  }

  ::LPDIRECT3DTEXTURE9 temp_texture{};
  create_round_rect(temp_texture,
      cv::Point(hud::EDGE_CIRCLE_RADIUS*2, hud::EDGE_CIRCLE_RADIUS*2),
      color);

  textures_.emplace_back(
      texture{
          id,
          std::shared_ptr<::IDirect3DTexture9>(temp_texture, custom_deleter{}),
          rect});
    
  frame_list_.emplace_back(
      message_frame{*this,
                    --textures_.end(),
                    cv::Point(rect.width, rect.height),
                    color,
                    std::make_shared<frame_animator>(frame_animator{})});
}

void hud::delete_frame(const std::string& id)
{
  decltype(textures_)::iterator it{
      std::find_if(textures_.begin(), textures_.end(),
                   [&](auto&& x){return x.id_ == id;})};

  if (it != textures_.end()) {
    decltype(frame_list_)::iterator frame{
        std::find_if(frame_list_.begin(), frame_list_.end(),
                     [&](auto&& x){return x.texture_ == it;})};
    frame->frame_animator_.reset(new_crt frame_animator_delete{});
  }
}

hud::frame_animator::frame_animator() : count_{0}
{
}

const float hud::frame_animator::LENGTH = 0.2f;

bool hud::frame_animator::operator()(message_frame& frame)
{
  ++count_;

  float width_progress{};
  float height_progress{};
  if (count_*constants::ANIMATION_SPEED < LENGTH/2) {
    width_progress = (count_*constants::ANIMATION_SPEED)/(LENGTH/2);
    width_progress = std::min(width_progress, 1.0f);
  } else if (LENGTH/2 <= count_*constants::ANIMATION_SPEED) {
    width_progress = 1.0f;
    height_progress = (count_*constants::ANIMATION_SPEED-LENGTH/2)/(LENGTH/2);
    height_progress = std::min(height_progress, 1.0f);
  } else {
    return false;
  }

  float width = hud::EDGE_CIRCLE_RADIUS*2 +
      (frame.size_dest_.x - hud::EDGE_CIRCLE_RADIUS*2)*width_progress;
  float height = hud::EDGE_CIRCLE_RADIUS*2 +
      (frame.size_dest_.y - hud::EDGE_CIRCLE_RADIUS*2)*height_progress;

  ::LPDIRECT3DTEXTURE9 temp_texture{};
  frame.outer_.create_round_rect(temp_texture,
      cv::Point(static_cast<int>(width), static_cast<int>(height)),
      frame.color_);
  frame.texture_->value_.reset(temp_texture, custom_deleter{});
  return true;
}

hud::frame_animator_delete::frame_animator_delete()
  : frame_animator()
{
}

bool hud::frame_animator_delete::operator()(message_frame& frame)
{
  ++count_;

  float progress{};
  if (count_*constants::ANIMATION_SPEED < LENGTH) {
    progress = (count_*constants::ANIMATION_SPEED)/LENGTH;
    progress = 1.0f - progress;
    progress = std::max(0.0f, progress);
  } else {
    decltype(frame.outer_.frame_list_)::iterator it{
        std::find_if(frame.outer_.frame_list_.begin(),
                     frame.outer_.frame_list_.end(),
                     [&](auto&& x){return x.texture_ == frame.texture_;})};
    frame.outer_.frame_list_.erase(it);
    frame.outer_.textures_.erase(frame.texture_);
    return false;
  }

  float width = hud::EDGE_CIRCLE_RADIUS*2 +
      (frame.size_dest_.x - hud::EDGE_CIRCLE_RADIUS*2)*progress;
  float height = hud::EDGE_CIRCLE_RADIUS*2 +
      (frame.size_dest_.y - hud::EDGE_CIRCLE_RADIUS*2)*progress;

  ::LPDIRECT3DTEXTURE9 temp_texture{};
  frame.outer_.create_round_rect(temp_texture,
      cv::Point(static_cast<int>(width), static_cast<int>(height)),
      frame.color_);
  frame.texture_->value_.reset(temp_texture, custom_deleter{});
  return true;
}

void hud::operator()()
{
  for (auto&& frame : frame_list_) {
    if (frame.frame_animator_ != nullptr) {
      (*frame.frame_animator_)(frame);
    }
  }
  for (auto&& message : message_list_) {
    if (message.message_writer_ != nullptr) {
      (*message.message_writer_)();
    }
  }
  sprite_->Begin(D3DXSPRITE_ALPHABLEND);
  for (decltype(textures_)::iterator it = textures_.begin();
       it != textures_.end(); ++it) {
    ::RECT rect = {0, 0, it->rect_.width, it->rect_.height};
    ::D3DXVECTOR3 vec3Center(0, 0, 0);
    ::D3DXVECTOR3 vec3Position(static_cast<float>(it->rect_.x),
                               static_cast<float>(it->rect_.y), 0);
    sprite_->Draw(it->value_.get(),
                  nullptr, &vec3Center, &vec3Position,
                  D3DCOLOR_ARGB(255, 255, 255, 255));
  }
  sprite_->End();
}
}
