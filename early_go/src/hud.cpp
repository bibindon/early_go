#include "stdafx.hpp"

#include "hud.hpp"
#include "text.hpp"
#include "character.hpp"

#include <boost/lexical_cast.hpp>
#include <thread>

namespace early_go {

const int hud::EDGE_CIRCLE_RADIUS{3};
hud::hud(const std::shared_ptr<::IDirect3DDevice9>& d3d_device)
  : d3d_device_{d3d_device}
{
  ::LPD3DXSPRITE temp_sprite{nullptr};
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
  cv_buffer.reserve(round_rect.rows * round_rect.cols * 4);
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
          D3DCOLOR_ARGB(128, 190, 190, 210),
          "游ゴシック",
//          "Consolas",
//          "Ricty Diminished",
//          "ＭＳ　Ｐゴシック",
          20,
          FW_NORMAL,
//          SHIFTJIS_CHARSET,
          ANSI_CHARSET,
          true));

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

const cv::Size hud::HP_info::TEXTURE_SIZE{1024, 1024};

void hud::show_HP_info()
{
  ::LPDIRECT3DTEXTURE9 temp_texture{};
  ::HRESULT result{::D3DXCreateTexture(
          d3d_device_.get(),
          HP_info::TEXTURE_SIZE.width, HP_info::TEXTURE_SIZE.height,
          1, D3DUSAGE_DYNAMIC,
          ::D3DFMT_A8B8G8R8, ::D3DPOOL_DEFAULT, &temp_texture)};
  if (FAILED(result)) {
    const std::string str{DXGetErrorString(result)};
    THROW_WITH_TRACE("Failed to create texture.: " + str);
  } else {
    ::D3DLOCKED_RECT locked_rect{};
    temp_texture->LockRect(0, &locked_rect, nullptr, D3DLOCK_DISCARD);

    std::fill(static_cast<int*>(locked_rect.pBits),
              static_cast<int*>(locked_rect.pBits) +
                  static_cast<std::size_t>(locked_rect.Pitch) *
                  HP_info::TEXTURE_SIZE.height / sizeof(int),
              0x00000000);

    temp_texture->UnlockRect(0);
  }

  textures_.emplace_back(
      texture{
          "early_HP",
          std::shared_ptr<::IDirect3DTexture9>(temp_texture, custom_deleter{}),
          cv::Rect(cv::Point(0), HP_info::TEXTURE_SIZE)});
    
  HP_info_.reset(new_crt HP_info{*this, --textures_.end(),
                 std::make_shared<HP_info_animator>(HP_info_animator{})});
}

// TODO animation
void hud::remove_HP_info()
{
  decltype(textures_)::iterator it{
      std::find_if(textures_.begin(), textures_.end(),
                   [&](auto&& x){return x.id_ == "early_HP";})};
  if (it != textures_.end()) {
    textures_.erase(it);
  }

  HP_info_.reset();
}

hud::HP_info_animator::HP_info_animator()
  : count_{0},
    type_{type::APPEARING}
{
}

void create_bezier_curve(cv::Mat* image,
                         const std::array<cv::Point, 3>& base_point,
                         const cv::Scalar& color,
                         const int& line_thickness)
{
  const std::array<cv::Point2f, 3>& base_pointf{
      static_cast<cv::Point2f>(base_point.at(0)),
      static_cast<cv::Point2f>(base_point.at(1)),
      static_cast<cv::Point2f>(base_point.at(2)) };

  const int FINENESS {8};
  std::vector<cv::Point2f> point(FINENESS);

  for (int i {0}; i < FINENESS; ++i) {
    double d {1.0f - static_cast<float>(i) / FINENESS};
    point.at(i) =     base_pointf.at(0) * d          * d          +
                  2 * base_pointf.at(1) * d          * (1.0f - d) +
                      base_pointf.at(2) * (1.0f - d) * (1.0f - d);
  }

  point.push_back(base_point.at(2));

  for (int i {0}; i < FINENESS; ++i) {
    cv::line(*image, point.at(i), point.at(i + 1),
             color, line_thickness, CV_AA);
  }
}

void hud::HP_info_animator::rotate(const type& direction, HP_info& hp_info)
{
  cv::Mat image {cv::Mat::zeros(HP_info::TEXTURE_SIZE.width,
                                HP_info::TEXTURE_SIZE.height,
                                CV_8UC4)};

  const cv::Scalar BASIC_COLOR {200, 240, 255, 60};
  const cv::Point2f  CENTER {
      static_cast<float>(HP_info::TEXTURE_SIZE.width/2),
      static_cast<float>(HP_info::TEXTURE_SIZE.height/2)};

  const int LINE_THICKNESS {2};
  const int CURVE_RADIUS {30};

  // center circle animation
  const int CENTER_CIRCLE_RADIUS_IN {5};
  const int CENTER_CIRCLE_RADIUS_OUT {12};
  const int CENTER_CIRCLE_ANIMATION_TIME {20};
  {
    cv::circle(image,
               CENTER,
               CENTER_CIRCLE_RADIUS_IN,
               BASIC_COLOR, -1, CV_AA);
    cv::circle(image,
               CENTER,
               CENTER_CIRCLE_RADIUS_OUT,
               BASIC_COLOR, LINE_THICKNESS, CV_AA);
  }

  // line animation 1
  const cv::Point2f LINE_1_DEST {-80.0f, -80.0f};
  const int LINE_1_ANIMATION_TIME {60};
  const float LINE_1_ANGLE_START {D3DX_PI/4};
  const float LINE_1_ANGLE_DEST {D3DX_PI*5/4};

  float line_length {static_cast<float>(LINE_1_DEST.x) * LINE_1_DEST.x +
                     static_cast<float>(LINE_1_DEST.y) * LINE_1_DEST.y };
  line_length = std::sqrt(line_length);

  cv::Point2f line_1 {};
  if (count_ < LINE_1_ANIMATION_TIME) {
    float line_1_angle_delta {LINE_1_ANGLE_DEST - LINE_1_ANGLE_START};
    line_1_angle_delta = line_1_angle_delta * count_ / LINE_1_ANIMATION_TIME;
    line_1_angle_delta += LINE_1_ANGLE_START;

    line_1.x = line_length * std::cos(line_1_angle_delta);
    line_1.y = line_length * std::sin(line_1_angle_delta);

    cv::line(image,
             CENTER,
             CENTER + line_1,
             BASIC_COLOR, LINE_THICKNESS, CV_AA);
  } else {
    line_1 = LINE_1_DEST;
    cv::line(image,
             CENTER,
             CENTER + LINE_1_DEST,
             BASIC_COLOR, LINE_THICKNESS, CV_AA);
  }

  // line animation 2
//  const cv::Point2f LINE_2_DEST {LINE_1_DEST.x - 100, LINE_1_DEST.y};
  const int LINE_2_ANIMATION_TIME {60};
  const float LINE_2_ANGLE_START {0};
  const float LINE_2_ANGLE_DEST {D3DX_PI};

  if (count_ < LINE_2_ANIMATION_TIME/2) {
    float line_2_angle_delta {LINE_2_ANGLE_DEST - LINE_2_ANGLE_START};
    line_2_angle_delta =
        line_2_angle_delta * count_ / (LINE_2_ANIMATION_TIME/2);
    line_2_angle_delta += LINE_2_ANGLE_START;

    line_length = 100.0f;
    cv::Point2f line_2 {line_length * std::cos(line_2_angle_delta),
                        line_length * std::sin(line_2_angle_delta)};

    cv::line(image,
             CENTER + line_1,
             CENTER + line_1 + line_2,
             BASIC_COLOR, LINE_THICKNESS, CV_AA);
  } else if (LINE_2_ANIMATION_TIME/2 <= count_) {
    cv::line(image,
             CENTER + line_1,
             CENTER + line_1 + cv::Point2f{-100.0f, 0.0f}, //LINE_2_DEST,
             BASIC_COLOR, LINE_THICKNESS, CV_AA);
  }

  std::vector<::uchar> cv_buffer {};
  cv_buffer.reserve(HP_info::TEXTURE_SIZE.width *
                    HP_info::TEXTURE_SIZE.height * sizeof(::DWORD));
  cv::imencode(".bmp", image, cv_buffer);

  ::D3DLOCKED_RECT locked_rect{};
  hp_info.texture_->value_->LockRect(0, &locked_rect, nullptr, 0);

  ::BYTE* pTexBuf = (::BYTE*)locked_rect.pBits;

  for (int j{0}; j < 1024; ++j) {
    for (int i{0}; i < 1024; ++i) {
      pTexBuf[j*1024*4 + (i*4)+0] = image.data[j*1024*4 + (i*4)+0];
      pTexBuf[j*1024*4 + (i*4)+1] = image.data[j*1024*4 + (i*4)+1];
      pTexBuf[j*1024*4 + (i*4)+2] = image.data[j*1024*4 + (i*4)+2];
      pTexBuf[j*1024*4 + (i*4)+3] = image.data[j*1024*4 + (i*4)+3];
    }
  }

  hp_info.texture_->value_->UnlockRect(0);
}

void hud::HP_info_animator::operator()(HP_info& hp_info, basic_window& window)
{
  ++count_;
  if (count_ > 90) {
    type_ = type::APPEARED;
  }

  const cv::Scalar BASIC_COLOR {200, 240, 255, 60};
  const cv::Point  CENTER {HP_info::TEXTURE_SIZE.width/2,
                           HP_info::TEXTURE_SIZE.height/2};

  const int LINE_THICKNESS {2};
  const int BEZIER_LENGTH {20};

  const std::shared_ptr<character>& main_chara {window.get_main_character()};
  const std::shared_ptr<character>& enemy {window.get_enemy_character()};

  ::D3DXVECTOR3 position {main_chara->get_position()};
  position.y += 1.0f;
  const cv::Point coodinate {window.get_screen_coodinate(position)};
  hp_info.texture_->rect_.x = coodinate.x - CENTER.x;
  hp_info.texture_->rect_.y = coodinate.y - CENTER.y;

  ::D3DXVECTOR3 enemy_position {enemy->get_position()};
  enemy_position.y += 1.0f;
  const cv::Point enemy_coodinate {window.get_screen_coodinate(enemy_position)};

  if (coodinate.x < enemy_coodinate.x) {
    // main chara is to the left of the enemy.
    if (type_ == type::APPEARED) {
      type_ = type::ROTATING_LEFT;
      count_ = 1;
    }
    rotate(type::ROTATING_LEFT, hp_info);
    return ;
  } else {
    // 
  }

  cv::Mat image {cv::Mat::zeros(
      HP_info::TEXTURE_SIZE.width, HP_info::TEXTURE_SIZE.height, CV_8UC4)};

  const std::vector<cv::Point> BASE_POINT {
      {CENTER},
      {CENTER + cv::Point{80, 80}},
      {CENTER + cv::Point{80, 80} + cv::Point{100,   0}},
      {CENTER + cv::Point{80, 80} + cv::Point{100,   0}
                                  + cv::Point{ 60, -60}},
      {CENTER + cv::Point{80, 80} + cv::Point{100,   0}
                                  + cv::Point{ 60, -60}
                                  + cv::Point{150,   0}},
      {CENTER + cv::Point{80, 80} + cv::Point{100,   0}
                                  + cv::Point{ 80,  80}}
  };

  // center circle animation
  const int CENTER_CIRCLE_RADIUS_IN {5};
  const int CENTER_CIRCLE_RADIUS_OUT {12};
  const int CENTER_CIRCLE_ANIMATION_TIME {20};
  if (count_ < CENTER_CIRCLE_ANIMATION_TIME) {
    cv::circle(image,
               CENTER,
               CENTER_CIRCLE_RADIUS_IN * count_ / CENTER_CIRCLE_ANIMATION_TIME,
               BASIC_COLOR, -1, CV_AA);
    cv::ellipse(image,
                CENTER,
                cv::Size(CENTER_CIRCLE_RADIUS_OUT, CENTER_CIRCLE_RADIUS_OUT),
                45, 0, 360 * count_ / CENTER_CIRCLE_ANIMATION_TIME,
                BASIC_COLOR, LINE_THICKNESS, CV_AA);
  } else {
    cv::circle(image,
               CENTER,
               CENTER_CIRCLE_RADIUS_IN,
               BASIC_COLOR, -1, CV_AA);
    cv::circle(image,
               CENTER,
               CENTER_CIRCLE_RADIUS_OUT,
               BASIC_COLOR, LINE_THICKNESS, CV_AA);
  }

  // line animation 1
  const cv::Point LINE_1_DEST {
      BASE_POINT.at(1) - cv::Point{BEZIER_LENGTH, BEZIER_LENGTH}};

  const int LINE_1_ANIMATION_TIME {10};

  if (count_ < LINE_1_ANIMATION_TIME) {
    const cv::Point LINE_1_DELTA {
        (LINE_1_DEST - BASE_POINT.at(0)) * count_ / LINE_1_ANIMATION_TIME};

    cv::line(image,
             BASE_POINT.at(0),
             BASE_POINT.at(0) + LINE_1_DELTA,
             BASIC_COLOR, LINE_THICKNESS, CV_AA);
  } else {
    cv::line(image,
             BASE_POINT.at(0),
             LINE_1_DEST,
             BASIC_COLOR, LINE_THICKNESS, CV_AA);
  }

  // curve 1
  const std::array<cv::Point, 3> BEZIER_COOD_1{
      BASE_POINT.at(1) - cv::Point{BEZIER_LENGTH, BEZIER_LENGTH},
      BASE_POINT.at(1),
      BASE_POINT.at(1) + cv::Point{BEZIER_LENGTH, 0}};

  if (LINE_1_ANIMATION_TIME <= count_) {
    create_bezier_curve(&image, BEZIER_COOD_1, BASIC_COLOR, LINE_THICKNESS);
  }

  // line animation 2
  const cv::Point LINE_2_START {BEZIER_COOD_1.at(2)};
  const cv::Point LINE_2_DEST {BASE_POINT.at(2)};
  const int LINE_2_ANIMATION_START {11};
  const int LINE_2_ANIMATION_END {20};
  const int LINE_2_ANIMATION_LENGTH {
      LINE_2_ANIMATION_END - LINE_2_ANIMATION_START + 1};

  if (LINE_2_ANIMATION_START <= count_ && count_ < LINE_2_ANIMATION_END) {
    const int count {count_ - LINE_2_ANIMATION_START};
    const cv::Point LINE_2_LENGTH {LINE_2_DEST - LINE_2_START};
    const cv::Point LINE_2_LENGTH_DELTA {
        LINE_2_LENGTH * count / LINE_2_ANIMATION_LENGTH};

    cv::line(image,
             LINE_2_START,
             LINE_2_START + LINE_2_LENGTH_DELTA,
             BASIC_COLOR, LINE_THICKNESS, CV_AA);
  } else if (LINE_2_ANIMATION_END <= count_) {
    cv::line(image,
             LINE_2_START,
             LINE_2_DEST,
             BASIC_COLOR, LINE_THICKNESS, CV_AA);
  }

  // line animation 3
  const cv::Point LINE_3_START {BASE_POINT.at(2)};
  const cv::Point LINE_3_DEST {
      BASE_POINT.at(3) + cv::Point(-BEZIER_LENGTH, BEZIER_LENGTH)};

  const int LINE_3_ANIMATION_START {21};
  const int LINE_3_ANIMATION_END {30};
  const int LINE_3_ANIMATION_LENGTH {
      LINE_3_ANIMATION_END - LINE_3_ANIMATION_START + 1};

  if (LINE_3_ANIMATION_START <= count_ && count_ < LINE_3_ANIMATION_END) {
    const int count {count_ - LINE_3_ANIMATION_START};
    const cv::Point LINE_3_LENGTH {LINE_3_DEST - LINE_3_START};
    const cv::Point LINE_3_LENGTH_DELTA {
        LINE_3_LENGTH * count / LINE_3_ANIMATION_LENGTH};
    cv::line(image,
             LINE_3_START,
             LINE_3_START + LINE_3_LENGTH_DELTA,
             BASIC_COLOR, LINE_THICKNESS, CV_AA);
  } else if (LINE_3_ANIMATION_END <= count_) {
    cv::line(image,
             LINE_3_START,
             LINE_3_DEST,
             BASIC_COLOR, LINE_THICKNESS, CV_AA);
  }

  // curve 2
  const std::array<cv::Point, 3> BEZIER_COOD_2{
      BASE_POINT.at(3) + cv::Point{-BEZIER_LENGTH, BEZIER_LENGTH},
      BASE_POINT.at(3),
      BASE_POINT.at(3) + cv::Point{BEZIER_LENGTH, 0}};

  if (LINE_3_ANIMATION_END <= count_) {
    create_bezier_curve(&image, BEZIER_COOD_2, BASIC_COLOR, LINE_THICKNESS);
  }


  // line animation 4
  const cv::Point LINE_4_START {BEZIER_COOD_2.at(2)};
  const cv::Point LINE_4_DEST {BASE_POINT.at(4)};
  const int LINE_4_ANIMATION_START {31};
  const int LINE_4_ANIMATION_END {40};
  const int LINE_4_ANIMATION_LENGTH {
      LINE_4_ANIMATION_END - LINE_4_ANIMATION_START + 1};

  if (LINE_4_ANIMATION_START <= count_ && count_ < LINE_4_ANIMATION_END) {
    const int count {count_ - LINE_4_ANIMATION_START};
    const cv::Point LINE_4_LENGTH {LINE_4_DEST - LINE_4_START};
    const cv::Point LINE_4_LENGTH_DELTA {
        LINE_4_LENGTH * count / LINE_4_ANIMATION_LENGTH};
    cv::line(image,
             LINE_4_START,
             LINE_4_START + LINE_4_LENGTH_DELTA,
             BASIC_COLOR, LINE_THICKNESS, CV_AA);
  } else if (LINE_4_ANIMATION_END <= count_) {
    cv::line(image,
             LINE_4_START,
             LINE_4_DEST,
             BASIC_COLOR, LINE_THICKNESS, CV_AA);
  }

  // line animation 5
  const cv::Point LINE_5_START {BASE_POINT.at(2)};
  const cv::Point LINE_5_DEST {BASE_POINT.at(5)};
  const int LINE_5_ANIMATION_START {21};
  const int LINE_5_ANIMATION_END {30};
  const int LINE_5_ANIMATION_LENGTH{
      LINE_5_ANIMATION_END - LINE_5_ANIMATION_START + 1};

  if (LINE_5_ANIMATION_START <= count_ && count_ < LINE_5_ANIMATION_END) {
    const int count {count_ - LINE_3_ANIMATION_START};
    const cv::Point LINE_5_LENGTH {LINE_5_DEST - LINE_5_START};
    const cv::Point LINE_5_LENGTH_DELTA {
        LINE_5_LENGTH * count / LINE_5_ANIMATION_LENGTH};

    cv::line(image,
             LINE_5_START,
             LINE_5_START + LINE_5_LENGTH_DELTA,
             BASIC_COLOR, LINE_THICKNESS, CV_AA);
  } else if (LINE_5_ANIMATION_END <= count_) {
    cv::line(image,
             LINE_5_START,
             LINE_5_DEST,
             BASIC_COLOR, LINE_THICKNESS, CV_AA);
  }

  // circle 2
  const int CIRCLE_2_RADIUS_OUT {15};
  const int CIRCLE_2_OUT_ANIMATION_START {11};
  const int CIRCLE_2_OUT_ANIMATION_LENGTH {20};
  const int CIRCLE_2_OUT_ANIMATION_END {
      CIRCLE_2_OUT_ANIMATION_START + CIRCLE_2_OUT_ANIMATION_LENGTH - 1};

  const int CIRCLE_2_RADIUS_IN {CIRCLE_2_RADIUS_OUT - (LINE_THICKNESS*2 - 1)};
  const int CIRCLE_2_IN_ANIMATION_START {21};
  const int CIRCLE_2_IN_ANIMATION_LENGTH {20};
  const int CIRCLE_2_IN_ANIMATION_END {
      CIRCLE_2_IN_ANIMATION_START + CIRCLE_2_IN_ANIMATION_LENGTH - 1};

  if (11 <= count_ && count_ <= 30) {
    const int count{count_ - CIRCLE_2_OUT_ANIMATION_START};
    cv::circle(image,
               BASE_POINT.at(2),
               CIRCLE_2_RADIUS_OUT * count / CIRCLE_2_OUT_ANIMATION_LENGTH,
               BASIC_COLOR, -1, CV_AA);
  } else if (30 < count_){
    cv::circle(image,
               BASE_POINT.at(2),
               CIRCLE_2_RADIUS_OUT,
               BASIC_COLOR, -1, CV_AA);
  }
  if (21 <= count_ && count_ <= 40) {
    int count = count_ - CIRCLE_2_IN_ANIMATION_START;
    cv::circle(image,
               BASE_POINT.at(2),
               CIRCLE_2_RADIUS_IN * count / CIRCLE_2_IN_ANIMATION_LENGTH,
               cv::Scalar(0), -1, CV_AA);
  } else if (40 < count_){
    cv::circle(image,
               BASE_POINT.at(2),
               CIRCLE_2_RADIUS_IN,
               cv::Scalar(0), -1, CV_AA);
  }

  // HP bar
  const int health {main_chara->get_health()};
  const int max_health {main_chara->get_max_health()};

  const int HP_BAR_RADIUS_IN {50};
  const int HP_BAR_RADIUS_OUT {60};
  const cv::Point HP_BAR_CENTER {BASE_POINT.at(4)};

  // HP bar background
  const int HP_BAR_BACK_ANIMATION_START {11};
  const int HP_BAR_BACK_ANIMATION_END {40};
  const int HP_BAR_BACK_ANIMATION_LENGTH {
      HP_BAR_BACK_ANIMATION_END - HP_BAR_BACK_ANIMATION_START + 1};
  const cv::Scalar HP_BAR_BACK {BASIC_COLOR[0], BASIC_COLOR[1],
                                BASIC_COLOR[2], BASIC_COLOR[3]/2};
  if (HP_BAR_BACK_ANIMATION_START <= count_ &&
      count_ <= HP_BAR_BACK_ANIMATION_END) {
    const int count {count_ - HP_BAR_BACK_ANIMATION_START};
    cv::ellipse(image,
                HP_BAR_CENTER,
                cv::Size(HP_BAR_RADIUS_OUT, HP_BAR_RADIUS_OUT),
                270, 0, 360 * count / HP_BAR_BACK_ANIMATION_LENGTH,
                HP_BAR_BACK, -1, CV_AA);
    cv::ellipse(image,
                HP_BAR_CENTER,
                cv::Size(HP_BAR_RADIUS_IN, HP_BAR_RADIUS_IN),
                0, 0, 360,
                cv::Scalar(0), -1, CV_AA);
  } else if (HP_BAR_BACK_ANIMATION_END < count_) {
    cv::ellipse(image,
                HP_BAR_CENTER,
                cv::Size(HP_BAR_RADIUS_OUT, HP_BAR_RADIUS_OUT),
                0, 0, 360,
                HP_BAR_BACK, -1, CV_AA);
    cv::ellipse(image,
                HP_BAR_CENTER,
                cv::Size(HP_BAR_RADIUS_IN, HP_BAR_RADIUS_IN),
                0, 0, 360,
                cv::Scalar(0), -1, CV_AA);
  }

  // HP bar foreground
  const int HP_BAR_FORE_ANIMATION_START {31};
  const int HP_BAR_FORE_ANIMATION_END {60};
  const int HP_BAR_FORE_ANIMATION_LENGTH {
      HP_BAR_FORE_ANIMATION_END - HP_BAR_FORE_ANIMATION_START + 1};
  const cv::Scalar HP_BAR_FORE {BASIC_COLOR};
  if (HP_BAR_FORE_ANIMATION_START <= count_ &&
      count_ <= HP_BAR_FORE_ANIMATION_END) {
    const int count {count_ - HP_BAR_FORE_ANIMATION_START};
    cv::ellipse(image,
                HP_BAR_CENTER,
                cv::Size{HP_BAR_RADIUS_OUT, HP_BAR_RADIUS_OUT},
                270, 0, 360 * count / HP_BAR_FORE_ANIMATION_LENGTH,
                HP_BAR_FORE, -1, CV_AA);
    cv::ellipse(image,
                HP_BAR_CENTER,
                cv::Size{HP_BAR_RADIUS_IN, HP_BAR_RADIUS_IN},
                0, 0, 360,
                cv::Scalar(0), -1, CV_AA);
  } else if (HP_BAR_FORE_ANIMATION_END < count_) {
    cv::ellipse(image,
                HP_BAR_CENTER,
                cv::Size{HP_BAR_RADIUS_OUT, HP_BAR_RADIUS_OUT},
                -90, 0, 360 * health / max_health,
                HP_BAR_FORE, -1, CV_AA);
    cv::ellipse(image,
                HP_BAR_CENTER,
                cv::Size{HP_BAR_RADIUS_IN, HP_BAR_RADIUS_IN},
                0, 0, 360,
                cv::Scalar(0), -1, CV_AA);
  }

  if (45 <= count_ && count_ < 75) {
    int count {count_-45};
    cv::ellipse(image,
                HP_BAR_CENTER,
                cv::Size{HP_BAR_RADIUS_IN - 5, HP_BAR_RADIUS_IN - 5},
                270, 0, 360 * count / 30,
                HP_BAR_FORE, LINE_THICKNESS, CV_AA);
  } else if (75 <= count_) {
    cv::ellipse(image,
                HP_BAR_CENTER,
                cv::Size{HP_BAR_RADIUS_IN - 5, HP_BAR_RADIUS_IN - 5},
                270, 0, 360,
                HP_BAR_FORE, LINE_THICKNESS, CV_AA);
  }

  // Time bar
  const int TIME_BAR_RADIUS_IN {30};
  const int TIME_BAR_RADIUS_OUT {40};
  const cv::Point TIME_BAR_CENTER {BASE_POINT.at(5)};

  const int TIME_BAR_BACK_ANIMATION_START {LINE_5_ANIMATION_START};
  const int TIME_BAR_BACK_ANIMATION_LENGTH {30};
  const int TIME_BAR_BACK_ANIMATION_END {
      TIME_BAR_BACK_ANIMATION_START+TIME_BAR_BACK_ANIMATION_LENGTH-1};

  const int TIME_BAR_FORE_ANIMATION_START {TIME_BAR_BACK_ANIMATION_START+20};
  const int TIME_BAR_FORE_ANIMATION_LENGTH {30};
  const int TIME_BAR_FORE_ANIMATION_END {
      TIME_BAR_FORE_ANIMATION_START+TIME_BAR_FORE_ANIMATION_LENGTH-1};

  // Strength bar
  if (TIME_BAR_BACK_ANIMATION_START <= count_ &&
      count_ <= TIME_BAR_BACK_ANIMATION_END) {
    const int count = count_ - TIME_BAR_BACK_ANIMATION_START;

    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                45 + 5 - 1, 0,
                (90-10)* count / TIME_BAR_BACK_ANIMATION_LENGTH,
                HP_BAR_BACK, -1, CV_AA);
    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                90 + 45 + 5 - 3, 0,
                (90-10)* count / TIME_BAR_BACK_ANIMATION_LENGTH,
                HP_BAR_BACK, -1, CV_AA);
    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                180 + 45 + 5 + 3, 0,
                (90-10)* count / TIME_BAR_BACK_ANIMATION_LENGTH,
                HP_BAR_BACK, -1, CV_AA);
    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                270 + 45 + 5 + 1, 0,
                (90-10)* count / TIME_BAR_BACK_ANIMATION_LENGTH,
                HP_BAR_BACK, -1, CV_AA);

    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size{TIME_BAR_RADIUS_IN, TIME_BAR_RADIUS_IN},
                0, 0, 360,
                cv::Scalar{0}, -1, CV_AA);
  } else if (TIME_BAR_BACK_ANIMATION_END < count_) {
    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                45 + 5 - 1, 0, 90-10,
                HP_BAR_BACK, -1, CV_AA);
    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                90 + 45 + 5 - 3, 0, 90-10,
                HP_BAR_BACK, -1, CV_AA);
    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                180 + 45 + 5 + 3, 0, 90-10,
                HP_BAR_BACK, -1, CV_AA);
    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                270 + 45 + 5 + 1, 0, 90-10,
                HP_BAR_BACK, -1, CV_AA);

    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size{TIME_BAR_RADIUS_IN, TIME_BAR_RADIUS_IN},
                0, 0, 360,
                cv::Scalar{0}, -1, CV_AA);
  }

  std::vector<std::string> normal_move_name {main_chara->get_normal_move()};
  std::pair<int, int> power {
      main_chara->get_normal_move_power(normal_move_name.at(0))};

  if (TIME_BAR_FORE_ANIMATION_START <= count_ &&
      count_ <= TIME_BAR_FORE_ANIMATION_END) {
    const int count {count_ - TIME_BAR_FORE_ANIMATION_START};

    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                45 + 5 - 1, 0,
                (90-10)* count / TIME_BAR_FORE_ANIMATION_LENGTH,
                HP_BAR_FORE, -1, CV_AA);
    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                90 + 45 + 5 - 3, 0,
                (90-10)* count / TIME_BAR_FORE_ANIMATION_LENGTH,
                HP_BAR_FORE, -1, CV_AA);
    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                180 + 45 + 5 + 3, 0,
                (90-10)* count / TIME_BAR_FORE_ANIMATION_LENGTH,
                HP_BAR_FORE, -1, CV_AA);
    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                270 + 45 + 5 + 1, 0,
                (90-10)* count / TIME_BAR_FORE_ANIMATION_LENGTH,
                HP_BAR_FORE, -1, CV_AA);

    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size(TIME_BAR_RADIUS_IN, TIME_BAR_RADIUS_IN),
                0, 0, 360,
                cv::Scalar(0), -1, CV_AA);
  } else if (TIME_BAR_FORE_ANIMATION_END < count_) {
    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                45 + 5 - 1, 0, (90-10)*power.first/power.second,
                HP_BAR_FORE, -1, CV_AA);
    power = main_chara->get_normal_move_power(normal_move_name.at(1));
    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                90 + 45 + 5 - 3, 0, (90-10)*power.first/power.second,
                HP_BAR_FORE, -1, CV_AA);
    power = main_chara->get_normal_move_power(normal_move_name.at(2));
    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                180 + 45 + 5 + 3, 0, (90-10)*power.first/power.second,
                HP_BAR_FORE, -1, CV_AA);
    power = main_chara->get_normal_move_power(normal_move_name.at(3));
    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                270 + 45 + 5 + 1, 0, (90-10)*power.first/power.second,
                HP_BAR_FORE, -1, CV_AA);

    cv::ellipse(image,
                TIME_BAR_CENTER,
                cv::Size(TIME_BAR_RADIUS_IN, TIME_BAR_RADIUS_IN),
                0, 0, 360,
                cv::Scalar(0), -1, CV_AA);
  }

//  std::vector<::uchar> cv_buffer {};
//  cv_buffer.reserve(HP_info::TEXTURE_SIZE.width *
//                    HP_info::TEXTURE_SIZE.height * sizeof(::DWORD));
//  cv::imencode(".bmp", image, cv_buffer);

//  ::LPDIRECT3DTEXTURE9 temp_texture {};
//  if (FAILED(::D3DXCreateTextureFromFileInMemory(
//      hp_info.outer_.d3d_device_.get(),
//      &cv_buffer[0],
//      static_cast<::UINT>(cv_buffer.size()),
//      &temp_texture))) {
//    THROW_WITH_TRACE("Failed create round rect.");
//  }
//
//  hp_info.texture_->value_.reset(temp_texture, custom_deleter{});

  ::D3DLOCKED_RECT locked_rect{};
  hp_info.texture_->value_->LockRect(0, &locked_rect, nullptr, 0);

  ::BYTE* pTexBuf = (::BYTE*)locked_rect.pBits;

  for (int j{0}; j < 1024; ++j) {
    for (int i{0}; i < 1024; ++i) {
      pTexBuf[j*1024*4 + (i*4)+0] = image.data[j*1024*4 + (i*4)+0];
      pTexBuf[j*1024*4 + (i*4)+1] = image.data[j*1024*4 + (i*4)+1];
      pTexBuf[j*1024*4 + (i*4)+2] = image.data[j*1024*4 + (i*4)+2];
      pTexBuf[j*1024*4 + (i*4)+3] = image.data[j*1024*4 + (i*4)+3];
    }
  }

  const ::DWORD LETTER_COLOR {
      D3DCOLOR_ARGB(static_cast<int>(BASIC_COLOR[3]),
                    static_cast<int>(BASIC_COLOR[2]),
                    static_cast<int>(BASIC_COLOR[1]),
                    static_cast<int>(BASIC_COLOR[0]))};

  const int LETTER_FADE_DURATION {10};

  if (CIRCLE_2_IN_ANIMATION_END <= count_ &&
      count_ < CIRCLE_2_IN_ANIMATION_END + LETTER_FADE_DURATION) {
    const int count {count_ - CIRCLE_2_IN_ANIMATION_END};
    ::DWORD letter_color {D3DCOLOR_ARGB(
        static_cast<int>(BASIC_COLOR[3] * count/LETTER_FADE_DURATION),
        static_cast<int>(BASIC_COLOR[2]),
        static_cast<int>(BASIC_COLOR[1]),
        static_cast<int>(BASIC_COLOR[0]))};

    // stage number
    add_text(hp_info.texture_->value_, "Stage No",
             cv::Rect(BASE_POINT.at(2) - cv::Point(87, 21),
                      hp_info.texture_->rect_.size()),
                      letter_color, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);

    add_text(hp_info.texture_->value_, "13",
             cv::Rect(BASE_POINT.at(2) - cv::Point(8, 9),
                      hp_info.texture_->rect_.size()),
                      letter_color, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);
  } else if (CIRCLE_2_IN_ANIMATION_END + LETTER_FADE_DURATION <= count_) {
    // stage number
    add_text(hp_info.texture_->value_, "Stage No",
             cv::Rect(BASE_POINT.at(2) - cv::Point(87, 21),
                      hp_info.texture_->rect_.size()),
                      LETTER_COLOR, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);

    add_text(hp_info.texture_->value_, "13",
             cv::Rect(BASE_POINT.at(2) - cv::Point(8, 9),
                      hp_info.texture_->rect_.size()),
                      LETTER_COLOR, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);
  }

  // Time
  if (TIME_BAR_BACK_ANIMATION_END <= count_ &&
      count_ < TIME_BAR_BACK_ANIMATION_END + LETTER_FADE_DURATION) {
    const int count = count_ - TIME_BAR_BACK_ANIMATION_END;
    ::DWORD letter_color {D3DCOLOR_ARGB(
        static_cast<int>(BASIC_COLOR[3] * count/LETTER_FADE_DURATION),
        static_cast<int>(BASIC_COLOR[2]),
        static_cast<int>(BASIC_COLOR[1]),
        static_cast<int>(BASIC_COLOR[0]))};

    add_text(hp_info.texture_->value_, "Time",
             cv::Rect(BASE_POINT.at(5) + cv::Point(-18, -19),
                      hp_info.texture_->rect_.size()),
                      letter_color, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);

    add_text(hp_info.texture_->value_, "99",
             cv::Rect(BASE_POINT.at(5) + cv::Point(-12, -2),
                      hp_info.texture_->rect_.size()),
                      letter_color, "游ゴシック", 30, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);
  } else if (TIME_BAR_BACK_ANIMATION_END + LETTER_FADE_DURATION <= count_) {
    add_text(hp_info.texture_->value_, "Time",
             cv::Rect(BASE_POINT.at(5) + cv::Point(-18, -19),
                      hp_info.texture_->rect_.size()),
                      LETTER_COLOR, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);

    add_text(hp_info.texture_->value_, "99",
             cv::Rect(BASE_POINT.at(5) + cv::Point(-12, -2),
                      hp_info.texture_->rect_.size()),
                      LETTER_COLOR, "游ゴシック", 30, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);
  }

  // strength 1
  std::string text {""};
  text = normal_move_name.at(0);
  text += ":";
  text += std::to_string(
      main_chara->get_normal_move_power(normal_move_name.at(0)).first);
  if (TIME_BAR_BACK_ANIMATION_START+5 <= count_ &&
      count_ < TIME_BAR_BACK_ANIMATION_START+5 + LETTER_FADE_DURATION) {
    const int count {count_ - TIME_BAR_BACK_ANIMATION_START-5};
    ::DWORD letter_color {D3DCOLOR_ARGB(
        static_cast<int>(BASIC_COLOR[3] * count/LETTER_FADE_DURATION),
        static_cast<int>(BASIC_COLOR[2]),
        static_cast<int>(BASIC_COLOR[1]),
        static_cast<int>(BASIC_COLOR[0]))};

    add_text(hp_info.texture_->value_, text,
             cv::Rect(BASE_POINT.at(5) + cv::Point(-30, -60 -9),
                      hp_info.texture_->rect_.size()),
                      letter_color, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);
  } else if (TIME_BAR_BACK_ANIMATION_START+5 + LETTER_FADE_DURATION <= count_) {
    add_text(hp_info.texture_->value_, text,
             cv::Rect(BASE_POINT.at(5) + cv::Point(-30, -60 -9),
                      hp_info.texture_->rect_.size()),
                      LETTER_COLOR, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);
  }

  // strength 2
  text = normal_move_name.at(1);
  text += ":";
  text += std::to_string(
      main_chara->get_normal_move_power(normal_move_name.at(1)).first);
  if (TIME_BAR_BACK_ANIMATION_START+10 <= count_ &&
      count_ < TIME_BAR_BACK_ANIMATION_START+10 + LETTER_FADE_DURATION) {
    const int count{count_ - TIME_BAR_BACK_ANIMATION_START-10};
    ::DWORD letter_color = D3DCOLOR_ARGB(
        static_cast<int>(BASIC_COLOR[3] * count/LETTER_FADE_DURATION),
        static_cast<int>(BASIC_COLOR[2]),
        static_cast<int>(BASIC_COLOR[1]),
        static_cast<int>(BASIC_COLOR[0]));

    add_text(hp_info.texture_->value_, text,
             cv::Rect(BASE_POINT.at(5) + cv::Point(50, -9),
                      hp_info.texture_->rect_.size()),
                      letter_color, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);
  } else if (TIME_BAR_BACK_ANIMATION_START+10 + LETTER_FADE_DURATION <= count_) {
    add_text(hp_info.texture_->value_, text,
             cv::Rect(BASE_POINT.at(5) + cv::Point(50, -9),
                      hp_info.texture_->rect_.size()),
                      LETTER_COLOR, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);
  }

  // strength 3
  text = normal_move_name.at(2);
  text += ":";
  text += std::to_string(
      main_chara->get_normal_move_power(normal_move_name.at(2)).first);
  if (TIME_BAR_BACK_ANIMATION_START+15 <= count_ &&
      count_ < TIME_BAR_BACK_ANIMATION_START+15 + LETTER_FADE_DURATION) {
    const int count{count_ - TIME_BAR_BACK_ANIMATION_START-15};
    ::DWORD letter_color {D3DCOLOR_ARGB(
        static_cast<int>(BASIC_COLOR[3] * count/LETTER_FADE_DURATION),
        static_cast<int>(BASIC_COLOR[2]),
        static_cast<int>(BASIC_COLOR[1]),
        static_cast<int>(BASIC_COLOR[0]))};

    add_text(hp_info.texture_->value_, text,
             cv::Rect(BASE_POINT.at(5) + cv::Point(-30, 60 -9),
                      hp_info.texture_->rect_.size()),
                      letter_color, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);
  } else if (TIME_BAR_BACK_ANIMATION_START+15 + LETTER_FADE_DURATION <= count_) {
    add_text(hp_info.texture_->value_, text,
             cv::Rect(BASE_POINT.at(5) + cv::Point(-30, 60 -9),
                      hp_info.texture_->rect_.size()),
                      LETTER_COLOR, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);
  }

  // strength 4
  text = normal_move_name.at(3);
  text += ":";
  text += std::to_string(
      main_chara->get_normal_move_power(normal_move_name.at(3)).first);
  if (TIME_BAR_BACK_ANIMATION_START+20 <= count_ &&
      count_ < TIME_BAR_BACK_ANIMATION_START+20 + LETTER_FADE_DURATION) {
    const int count{count_ - TIME_BAR_BACK_ANIMATION_START-20};
    ::DWORD letter_color = D3DCOLOR_ARGB(
        static_cast<int>(BASIC_COLOR[3] * count/LETTER_FADE_DURATION),
        static_cast<int>(BASIC_COLOR[2]),
        static_cast<int>(BASIC_COLOR[1]),
        static_cast<int>(BASIC_COLOR[0]));

    add_text(hp_info.texture_->value_, text,
             cv::Rect(BASE_POINT.at(5) + cv::Point(-140, -9),
                      hp_info.texture_->rect_.size()),
                      letter_color, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);
  } else if (TIME_BAR_BACK_ANIMATION_START+20 + LETTER_FADE_DURATION <=
             count_) {
    add_text(hp_info.texture_->value_, text,
             cv::Rect(BASE_POINT.at(5) + cv::Point(-140, -9),
                      hp_info.texture_->rect_.size()),
                      LETTER_COLOR, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);
  }

  // HP
  const std::string sz_health{boost::lexical_cast<std::string>(health)};
  const std::string sz_max_health{
      "/ " + boost::lexical_cast<std::string>(max_health)};

  if (LINE_4_ANIMATION_END <= count_ &&
      count_ < LINE_4_ANIMATION_END + LETTER_FADE_DURATION) {
    const int count{count_ - LINE_4_ANIMATION_END};
    ::DWORD letter_color = D3DCOLOR_ARGB(
        static_cast<int>(BASIC_COLOR[3] * count/LETTER_FADE_DURATION),
        static_cast<int>(BASIC_COLOR[2]),
        static_cast<int>(BASIC_COLOR[1]),
        static_cast<int>(BASIC_COLOR[0]));

    add_text(hp_info.texture_->value_, "HP",
             cv::Rect(BASE_POINT.at(4) + cv::Point(-10, -30),
                      hp_info.texture_->rect_.size()),
                      letter_color, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);

    add_text(hp_info.texture_->value_, sz_health,
             cv::Rect(BASE_POINT.at(4) + cv::Point(-33, -10),
                      hp_info.texture_->rect_.size()),
                      letter_color, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, false);

    add_text(hp_info.texture_->value_, sz_max_health,
             cv::Rect(BASE_POINT.at(4) + cv::Point(-20, 10),
                      hp_info.texture_->rect_.size()),
                      letter_color, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, false);

  } else if(LINE_4_ANIMATION_END + LETTER_FADE_DURATION <= count_) {
    add_text(hp_info.texture_->value_, "HP",
             cv::Rect(BASE_POINT.at(4) + cv::Point(-10, -30),
                      hp_info.texture_->rect_.size()),
                      LETTER_COLOR, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, true);

    add_text(hp_info.texture_->value_, sz_health,
             cv::Rect(BASE_POINT.at(4) + cv::Point(-33, -10),
                      hp_info.texture_->rect_.size()),
                      LETTER_COLOR, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      ANSI_CHARSET, false);

    add_text(hp_info.texture_->value_, sz_max_health,
             cv::Rect(BASE_POINT.at(4) + cv::Point(-20, 10),
                      hp_info.texture_->rect_.size()),
                      LETTER_COLOR, "游ゴシック", 20, FW_NORMAL, locked_rect,
                      SHIFTJIS_CHARSET, false);
  }
  hp_info.texture_->value_->UnlockRect(0);
}

void hud::operator()(basic_window& a_basic_window)
{
//  auto th1 = std::thread([&]{
//    std::vector<decltype(frame_list_)::iterator> abandon;
//    for (decltype(frame_list_)::iterator frame{frame_list_.begin()};
//         frame != frame_list_.end(); ++frame) {
//      if (frame->frame_animator_ != nullptr) {
//        if (!(*frame->frame_animator_)(*frame)) {
//          abandon.push_back(frame);
//        }
//      }
//    }
//
//    for (auto&& x : abandon) {
//      textures_.erase(x->texture_);
//      frame_list_.erase(x);
//    }
//  });
//  auto th2 = std::thread([&]{
//    for (auto&& message : message_list_) {
//      if (message.message_writer_ != nullptr) {
//        (*message.message_writer_)();
//      }
//    }
//  });
//
//  auto th3 = std::thread([&]{
//    if (HP_info_ != nullptr && HP_info_->HP_info_animator_ != nullptr) {
//      (*HP_info_->HP_info_animator_)(*HP_info_, a_basic_window);
//    }
//  });
//  th1.join();
//  th2.join();
//  th3.join();
  std::vector<decltype(frame_list_)::iterator> abandon;
  for (decltype(frame_list_)::iterator frame{frame_list_.begin()};
       frame != frame_list_.end(); ++frame) {
    if (frame->frame_animator_ != nullptr) {
      if (!(*frame->frame_animator_)(*frame)) {
        abandon.push_back(frame);
      }
    }
  }

  for (auto&& x : abandon) {
    textures_.erase(x->texture_);
    frame_list_.erase(x);
  }

  for (auto&& message : message_list_) {
    if (message.message_writer_ != nullptr) {
      (*message.message_writer_)();
    }
  }

  if (HP_info_ != nullptr && HP_info_->HP_info_animator_ != nullptr) {
    (*HP_info_->HP_info_animator_)(*HP_info_, a_basic_window);
  }

  sprite_->Begin(D3DXSPRITE_ALPHABLEND);
  for (decltype(textures_)::iterator it = textures_.begin();
       it != textures_.end(); ++it) {
    ::RECT rect = {0, 0, it->rect_.width, it->rect_.height};
    ::D3DXVECTOR3 center(0, 0, 0);
    ::D3DXVECTOR3 position(static_cast<float>(it->rect_.x),
                           static_cast<float>(it->rect_.y), 0);
    sprite_->Draw(it->value_.get(),
                  nullptr, &center, &position,
                  D3DCOLOR_ARGB(255, 255, 255, 255));
  }
  sprite_->End();
}
}
