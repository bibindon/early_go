#include "stdafx.hpp"

#include "hud.hpp"
#include "text.hpp"
#include "text_thread_ver.hpp"
#include "character.hpp"

#include <boost/lexical_cast.hpp>
#include <thread>

using std::vector;
using std::string;

namespace early_go
{
void create_bezier_curve(cv::Mat *image,
                         const std::array<cv::Point, 3> &base_point,
                         const cv::Scalar &color,
                         const int &line_thickness);

const int hud::EDGE_CIRCLE_RADIUS{3};
hud::hud(const std::shared_ptr<IDirect3DDevice9> &d3d_device)
    : d3d_device_{d3d_device}
{
    LPD3DXSPRITE temp_sprite{nullptr};
    if (FAILED(D3DXCreateSprite(d3d_device_.get(), &temp_sprite)))
    {
        THROW_WITH_TRACE("Failed to create a sprite.");
    }
    sprite_.reset(temp_sprite, custom_deleter{});
}

void hud::create_round_rect(LPDIRECT3DTEXTURE9 &texture,
                            const cv::Point &size, const cv::Scalar &color)
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
                           size.x - EDGE_CIRCLE_RADIUS * 2 + 1, size.y + 1),
                  color, -1, CV_AA);

    cv::rectangle(round_rect,
                  cv::Rect(0, EDGE_CIRCLE_RADIUS,
                           size.x + 1, size.y - EDGE_CIRCLE_RADIUS * 2 + 1),
                  color, -1, CV_AA);

    vector<uchar> cv_buffer{};

    cv_buffer.reserve(
        static_cast<decltype(cv_buffer)::size_type>(round_rect.rows) *
        round_rect.cols * 4);

    cv::imencode(".bmp", round_rect, cv_buffer);
    if (FAILED(D3DXCreateTextureFromFileInMemory(
            d3d_device_.get(),
            &cv_buffer[0],
            static_cast<UINT>(cv_buffer.size()),
            &texture)))
    {
        THROW_WITH_TRACE("Failed create round rect.");
    }
}

void hud::add_image(const string &id,
                    const string &filename,
                    const cv::Point &position)
{
    auto it{std::find_if(textures_.begin(), textures_.end(),
                         [&](auto &&x)
                         { return x.id_ == id; })};

    if (it != textures_.end())
    {
        return;
    }

    vector<char> buffer = util::get_image_resource(filename);
    LPDIRECT3DTEXTURE9 temp_texture{};
    if (FAILED(D3DXCreateTextureFromFileInMemory(
            d3d_device_.get(),
            &buffer[0],
            static_cast<UINT>(buffer.size()), &temp_texture)))
    {
        THROW_WITH_TRACE("Failed to create a texture.");
    }

    D3DSURFACE_DESC desc{};
    if (FAILED(temp_texture->GetLevelDesc(0, &desc)))
    {
        THROW_WITH_TRACE("Failed to create a texture.");
    }
    int width_next_pow_2{get_next_pow_2(desc.Width)};
    int height_next_pow_2{get_next_pow_2(desc.Height)};

    textures_.emplace_back(
        texture{
            id,
            std::shared_ptr<IDirect3DTexture9>(temp_texture, custom_deleter{}),
            cv::Rect(position.x, position.y,
                     width_next_pow_2, height_next_pow_2)});
}

void hud::delete_image(const string &id)
{
    decltype(textures_)::iterator it{
        std::find_if(textures_.begin(), textures_.end(),
                     [&](auto &&x)
                     { return x.id_ == id; })};
    if (it != textures_.end())
    {
        textures_.erase(it);
    }
}

void hud::add_message_in_frame(const string &message_id,
                               const string &frame_id,
                               const string &message)
{
    auto it{std::find_if(textures_.begin(), textures_.end(),
                         [&](auto &&x)
                         { return x.id_ == frame_id; })};

    if (it == textures_.end())
    {
        return;
    }

    add_message(message_id, message,
                cv::Rect(it->rect_.x + 10, it->rect_.y + 10,
                         it->rect_.width - 20, it->rect_.height - 20));
}

void hud::add_message(const string &id,
                      const string &message,
                      const cv::Rect &rect)
{
    auto it{std::find_if(textures_.begin(), textures_.end(),
                         [&](auto &&x)
                         { return x.id_ == id; })};

    if (it != textures_.end())
    {
        return;
    }

    texture temp_texture{id,
                         std::shared_ptr<IDirect3DTexture9>(),
                         rect};
    textures_.emplace_back(temp_texture);

    std::shared_ptr<message_writer> writer(
        std::make_shared<message_writer>(
            d3d_device_,
            textures_.back().value_,
            message,
            false,
            cv::Rect(0, 0, rect.width, rect.height),
            cv::Size(get_next_pow_2(rect.width), get_next_pow_2(rect.height)),
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

void hud::delete_message(const string &id)
{
    decltype(textures_)::iterator it{
        std::find_if(textures_.begin(), textures_.end(),
                     [&](auto &&x)
                     { return x.id_ == id; })};
    if (it != textures_.end())
    {
        decltype(message_list_)::iterator message{
            std::find_if(message_list_.begin(), message_list_.end(),
                         [&](auto &&x)
                         { return x.texture_ == it; })};
        message_list_.erase(message);
        textures_.erase(it);
    }
}

void hud::add_frame(
    const string &id, const cv::Rect &rect, const cv::Scalar &color)
{
    auto it{std::find_if(textures_.begin(), textures_.end(),
                         [&](auto &&x)
                         { return x.id_ == id; })};

    if (it != textures_.end())
    {
        return;
    }

    LPDIRECT3DTEXTURE9 temp_texture{};
    create_round_rect(temp_texture,
                      cv::Point(hud::EDGE_CIRCLE_RADIUS * 2, hud::EDGE_CIRCLE_RADIUS * 2),
                      color);

    textures_.emplace_back(
        texture{
            id,
            std::shared_ptr<IDirect3DTexture9>(temp_texture, custom_deleter{}),
            rect});

    frame_list_.emplace_back(
        message_frame{*this,
                      --textures_.end(),
                      cv::Point(rect.width, rect.height),
                      color,
                      std::make_shared<frame_animator>(frame_animator{})});
}

void hud::delete_frame(const string &id)
{
    decltype(textures_)::iterator it{
        std::find_if(textures_.begin(), textures_.end(),
                     [&](auto &&x)
                     { return x.id_ == id; })};

    if (it != textures_.end())
    {
        decltype(frame_list_)::iterator frame{
            std::find_if(frame_list_.begin(), frame_list_.end(),
                         [&](auto &&x)
                         { return x.texture_ == it; })};
        frame->frame_animator_.reset(new_crt frame_animator_delete{});
    }
}

hud::frame_animator::frame_animator() : count_{0}
{
}

const float hud::frame_animator::LENGTH = 0.2f;

bool hud::frame_animator::operator()(message_frame &frame)
{
    ++count_;

    float width_progress{};
    float height_progress{};
    if (count_ * constants::ANIMATION_SPEED < LENGTH / 2)
    {
        width_progress = (count_ * constants::ANIMATION_SPEED) / (LENGTH / 2);
        width_progress = std::min(width_progress, 1.0f);
    }
    else if (LENGTH / 2 <= count_ * constants::ANIMATION_SPEED)
    {
        width_progress = 1.0f;
        height_progress = (count_ * constants::ANIMATION_SPEED - LENGTH / 2) / (LENGTH / 2);
        height_progress = std::min(height_progress, 1.0f);
    }
    else
    {
        return false;
    }

    float width = hud::EDGE_CIRCLE_RADIUS * 2 +
                  (frame.size_dest_.x - hud::EDGE_CIRCLE_RADIUS * 2) * width_progress;
    float height = hud::EDGE_CIRCLE_RADIUS * 2 +
                   (frame.size_dest_.y - hud::EDGE_CIRCLE_RADIUS * 2) * height_progress;

    LPDIRECT3DTEXTURE9 temp_texture{};
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

bool hud::frame_animator_delete::operator()(message_frame &frame)
{
    ++count_;

    float progress{};
    if (count_ * constants::ANIMATION_SPEED < LENGTH)
    {
        progress = (count_ * constants::ANIMATION_SPEED) / LENGTH;
        progress = 1.0f - progress;
        progress = std::max(0.0f, progress);
    }
    else
    {
        return false;
    }

    float width = hud::EDGE_CIRCLE_RADIUS * 2 +
                  (frame.size_dest_.x - hud::EDGE_CIRCLE_RADIUS * 2) * progress;
    float height = hud::EDGE_CIRCLE_RADIUS * 2 +
                   (frame.size_dest_.y - hud::EDGE_CIRCLE_RADIUS * 2) * progress;

    LPDIRECT3DTEXTURE9 temp_texture{};
    frame.outer_.create_round_rect(temp_texture,
                                   cv::Point(static_cast<int>(width), static_cast<int>(height)),
                                   frame.color_);
    frame.texture_->value_.reset(temp_texture, custom_deleter{});
    return true;
}

const cv::Size hud::HP_info::TEXTURE_SIZE{1024, 1024};
const cv::Size hud::HP_info2::TEXTURE_SIZE{1024, 1024};

void hud::show_HP_info()
{
    {
        LPDIRECT3DTEXTURE9 temp_texture{};
        HRESULT result{D3DXCreateTexture(
            d3d_device_.get(),
            HP_info::TEXTURE_SIZE.width, HP_info::TEXTURE_SIZE.height,
            1, D3DUSAGE_DYNAMIC,
            D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &temp_texture)};
        if (FAILED(result))
        {
            const string str{DXGetErrorString(result)};
            THROW_WITH_TRACE("Failed to create texture.: " + str);
        }
        else
        {
            D3DLOCKED_RECT locked_rect{};
            temp_texture->LockRect(0, &locked_rect, nullptr, D3DLOCK_DISCARD);

            std::fill(static_cast<int *>(locked_rect.pBits),
                      static_cast<int *>(locked_rect.pBits) +
                          static_cast<size_t>(locked_rect.Pitch) *
                              HP_info::TEXTURE_SIZE.height / sizeof(int),
                      0x00000000);

            temp_texture->UnlockRect(0);
        }

        textures_.emplace_back(
            texture{
                "early_HP",
                std::shared_ptr<IDirect3DTexture9>(temp_texture, custom_deleter{}),
                cv::Rect(cv::Point(0), HP_info::TEXTURE_SIZE)});

        HP_info_.reset(new_crt HP_info{*this, --textures_.end(),
                                       std::make_shared<HP_info_animator>(HP_info_animator{})});
    }
    {
        LPDIRECT3DTEXTURE9 temp_texture{};
        HRESULT result{D3DXCreateTexture(
            d3d_device_.get(),
            HP_info::TEXTURE_SIZE.width, HP_info::TEXTURE_SIZE.height,
            1, D3DUSAGE_DYNAMIC,
            D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &temp_texture)};
        if (FAILED(result))
        {
            const string str{DXGetErrorString(result)};
            THROW_WITH_TRACE("Failed to create texture.: " + str);
        }
        else
        {
            D3DLOCKED_RECT locked_rect{};
            temp_texture->LockRect(0, &locked_rect, nullptr, D3DLOCK_DISCARD);

            std::fill(static_cast<int *>(locked_rect.pBits),
                      static_cast<int *>(locked_rect.pBits) +
                          static_cast<size_t>(locked_rect.Pitch) *
                              HP_info::TEXTURE_SIZE.height / sizeof(int),
                      0x00000000);

            temp_texture->UnlockRect(0);
        }

        textures_.emplace_back(
            texture{
                "enemy_HP",
                std::shared_ptr<IDirect3DTexture9>(temp_texture, custom_deleter{}),
                cv::Rect(cv::Point(0), HP_info::TEXTURE_SIZE)});

        HP_info2_.reset(new_crt HP_info2{*this, --textures_.end(),
                                         std::make_shared<HP_info_animator2>(HP_info_animator2{})});
    }
}

void hud::HP_info_drawer::draw_text_stage_number_1(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    if (CIRCLE_2_IN_ANIMATION_END <= count_ &&
        count_ < CIRCLE_2_IN_ANIMATION_END + LETTER_FADE_DURATION)
    {
        const int count{count_ - CIRCLE_2_IN_ANIMATION_END};
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        // stage number
        writer->add_text("Stage No",
                         BASE_POINT.at(2) - cv::Point(87, 21),
                         letter_color);
    }
    else if (CIRCLE_2_IN_ANIMATION_END + LETTER_FADE_DURATION <= count_)
    {
        // stage number
        writer->add_text("Stage No",
                         BASE_POINT.at(2) - cv::Point(87, 21),
                         LETTER_COLOR);
    }
    copy_text(writer);
}

void hud::HP_info_drawer::draw_text_stage_number_2(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    if (CIRCLE_2_IN_ANIMATION_END <= count_ &&
        count_ < CIRCLE_2_IN_ANIMATION_END + LETTER_FADE_DURATION)
    {
        const int count{count_ - CIRCLE_2_IN_ANIMATION_END};
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        // stage number
        writer->add_text("13",
                         BASE_POINT.at(2) - cv::Point(8, 9),
                         letter_color);
    }
    else if (CIRCLE_2_IN_ANIMATION_END + LETTER_FADE_DURATION <= count_)
    {
        // stage number
        writer->add_text("13",
                         BASE_POINT.at(2) - cv::Point(8, 9),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer::copy_text(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    const cv::Point &start_point = writer->start_point_;
    const cv::Size &canvas_size = writer->canvas_size_;
    const vector<vector<DWORD>> &text_image_ = writer->text_image_;
    for (size_t j{}; j < canvas_size.height; ++j)
    {
        DWORD *row = image_.ptr<DWORD>(static_cast<int>(j + start_point.y));
        row += start_point.x;
        std::transform(text_image_[j].cbegin(), text_image_[j].cend(), row, row,
                       [](const auto &src, const auto &dst)
                       {
                           return (src & 0xff000000UL) != 0 ? src : dst;
                           //            return src > 0x00ffffffUL ? src : dst;
                       });
    }
}
void hud::HP_info_drawer::draw_text_time_1(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    if (TIME_BAR_BACK_ANIMATION_END <= count_ &&
        count_ < TIME_BAR_BACK_ANIMATION_END + LETTER_FADE_DURATION)
    {
        const int count = count_ - TIME_BAR_BACK_ANIMATION_END;
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        writer->add_text("Time", BASE_POINT.at(5) + cv::Point(-18, -19),
                         letter_color);
    }
    else if (TIME_BAR_BACK_ANIMATION_END + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text("Time", BASE_POINT.at(5) + cv::Point(-18, -19),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer::draw_text_time_2(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    if (TIME_BAR_BACK_ANIMATION_END <= count_ &&
        count_ < TIME_BAR_BACK_ANIMATION_END + LETTER_FADE_DURATION)
    {
        const int count = count_ - TIME_BAR_BACK_ANIMATION_END;
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        writer->add_text("99", BASE_POINT.at(5) + cv::Point(-12, -2),
                         letter_color);
    }
    else if (TIME_BAR_BACK_ANIMATION_END + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text("99", BASE_POINT.at(5) + cv::Point(-12, -2),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer::draw_text_strength_1(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    string text{""};
    if (!main_chara_ || main_chara_->get_normal_move().empty())
    {
        return;
    }
    text = main_chara_->get_normal_move().at(0);
    text += ":";
    text += std::to_string(main_chara_->get_normal_move_power(
                                          main_chara_->get_normal_move().at(0))
                               .first);
    if (TIME_BAR_BACK_ANIMATION_START + 5 <= count_ &&
        count_ < TIME_BAR_BACK_ANIMATION_START + 5 + LETTER_FADE_DURATION)
    {
        const int count{count_ - TIME_BAR_BACK_ANIMATION_START - 5};
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        writer->add_text(text, BASE_POINT.at(5) + cv::Point(-30, -60 - 9),
                         letter_color);
    }
    else if (TIME_BAR_BACK_ANIMATION_START + 5 + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text(text, BASE_POINT.at(5) + cv::Point(-30, -60 - 9),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer::draw_text_strength_2(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    string text{""};
    if (!main_chara_ || main_chara_->get_normal_move().size() < 2)
    {
        return;
    }
    text = main_chara_->get_normal_move().at(1);
    text += ":";
    text += std::to_string(main_chara_->get_normal_move_power(
                                          main_chara_->get_normal_move().at(1))
                               .first);
    if (TIME_BAR_BACK_ANIMATION_START + 10 <= count_ &&
        count_ < TIME_BAR_BACK_ANIMATION_START + 10 + LETTER_FADE_DURATION)
    {
        const int count{count_ - TIME_BAR_BACK_ANIMATION_START - 10};
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        writer->add_text(text, BASE_POINT.at(5) + cv::Point(50, -9),
                         letter_color);
    }
    else if (TIME_BAR_BACK_ANIMATION_START + 5 + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text(text, BASE_POINT.at(5) + cv::Point(50, -9),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer::draw_text_strength_3(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    string text{""};
    if (!main_chara_ || main_chara_->get_normal_move().size() < 3)
    {
        return;
    }
    text = main_chara_->get_normal_move().at(2);
    text += ":";
    text += std::to_string(main_chara_->get_normal_move_power(
                                          main_chara_->get_normal_move().at(2))
                               .first);
    if (TIME_BAR_BACK_ANIMATION_START + 15 <= count_ &&
        count_ < TIME_BAR_BACK_ANIMATION_START + 15 + LETTER_FADE_DURATION)
    {
        const int count{count_ - TIME_BAR_BACK_ANIMATION_START - 15};
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        writer->add_text(text, BASE_POINT.at(5) + cv::Point(-30, 60 - 9),
                         letter_color);
    }
    else if (TIME_BAR_BACK_ANIMATION_START + 5 + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text(text, BASE_POINT.at(5) + cv::Point(-30, 60 - 9),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer::draw_text_strength_4(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    string text{""};
    if (!main_chara_ || main_chara_->get_normal_move().size() < 4)
    {
        return;
    }
    text = main_chara_->get_normal_move().at(3);
    text += ":";
    text += std::to_string(main_chara_->get_normal_move_power(
                                          main_chara_->get_normal_move().at(3))
                               .first);
    if (TIME_BAR_BACK_ANIMATION_START + 20 <= count_ &&
        count_ < TIME_BAR_BACK_ANIMATION_START + 20 + LETTER_FADE_DURATION)
    {
        const int count{count_ - TIME_BAR_BACK_ANIMATION_START - 20};
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        writer->add_text(text, BASE_POINT.at(5) + cv::Point(-140, -9),
                         letter_color);
    }
    else if (TIME_BAR_BACK_ANIMATION_START + 5 + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text(text, BASE_POINT.at(5) + cv::Point(-140, -9),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer::draw_text_HP_1(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    if (LINE_4_ANIMATION_END <= count_ &&
        count_ < LINE_4_ANIMATION_END + LETTER_FADE_DURATION)
    {
        const int count{count_ - LINE_4_ANIMATION_END};
        DWORD letter_color = D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]));

        writer->add_text("HP", BASE_POINT.at(4) + cv::Point(-10, -30),
                         letter_color);
    }
    else if (LINE_4_ANIMATION_END + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text("HP", BASE_POINT.at(4) + cv::Point(-10, -30),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer::draw_text_HP_2(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    if (!main_chara_)
    {
        return;
    }
    const int health = main_chara_->get_health();
    const string sz_health{boost::lexical_cast<string>(health)};

    if (LINE_4_ANIMATION_END <= count_ &&
        count_ < LINE_4_ANIMATION_END + LETTER_FADE_DURATION)
    {
        const int count{count_ - LINE_4_ANIMATION_END};
        DWORD letter_color = D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]));

        writer->add_text(sz_health, BASE_POINT.at(4) + cv::Point(-33, -10),
                         letter_color);
    }
    else if (LINE_4_ANIMATION_END + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text(sz_health, BASE_POINT.at(4) + cv::Point(-33, -10),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer::draw_text_HP_3(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    if (!main_chara_)
    {
        return;
    }
    const int max_health = main_chara_->get_max_health();
    const string sz_max_health{
        "/ " + boost::lexical_cast<string>(max_health)};

    if (LINE_4_ANIMATION_END <= count_ &&
        count_ < LINE_4_ANIMATION_END + LETTER_FADE_DURATION)
    {
        const int count{count_ - LINE_4_ANIMATION_END};
        DWORD letter_color = D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]));

        writer->add_text(sz_max_health, BASE_POINT.at(4) + cv::Point(-20, 10),
                         letter_color);
    }
    else if (LINE_4_ANIMATION_END + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text(sz_max_health, BASE_POINT.at(4) + cv::Point(-20, 10),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer::operator()()
{
    funcs_.push_back([&]
                     { draw_Time_bar(); });
    funcs_.push_back([&]
                     { draw_center_circle(); });
    funcs_.push_back([&]
                     { draw_line_1(); });
    funcs_.push_back([&]
                     { draw_curve_1(); });
    funcs_.push_back([&]
                     { draw_line_2(); });
    funcs_.push_back([&]
                     { draw_line_3(); });
    funcs_.push_back([&]
                     { draw_curve_2(); });
    funcs_.push_back([&]
                     { draw_line_4(); });
    funcs_.push_back([&]
                     { draw_line_5(); });
    funcs_.push_back([&]
                     { draw_circle_2(); });
    funcs_.push_back([&]
                     { draw_HP_bar(); });
    std::shared_ptr<message_writer_for_thread> temp_writer{
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{100, 20}}};
    funcs_.push_back([&, temp_writer]
                     { draw_text_stage_number_1(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{30, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_stage_number_2(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{40, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_time_1(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 30, cv::Size{30, 30}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_time_2(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{90, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_strength_1(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{90, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_strength_2(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{90, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_strength_3(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{90, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_strength_4(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{30, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_HP_1(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{50, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_HP_2(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{70, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_HP_3(temp_writer); });

    for (int i = 0; i < THREAD_NUM; ++i)
    {
        drawer_[i].reset(new std::thread([&, i]
                                         {
  while(true) {
    if (finish_request_) {
      break;
    }
    int charge_func_index = 0;
    // block scope for mutex
    {
      std::lock_guard<std::mutex> lock(charge_func_index_mtx_);
      charge_func_index = charge_func_index_;

//          early_go::log_liner() << "i: " << i
//              << "  charge_func_index_mtx_: " << charge_func_index;
      if (charge_func_index < funcs_.size()) {
        ++charge_func_index_;
      }
    }
    if (charge_func_index < funcs_.size()) {
      funcs_[charge_func_index]();
    } else {
      // block scope for mutex
      {
        std::unique_lock<std::mutex> lock(idle_mtx_);
        idle_[i] = true;
        cond_.wait(lock, [this, i]{return idle_[i] == false;});
      }
    }
  } }));
    }
}

hud::HP_info_drawer::~HP_info_drawer()
{
    finish_request_ = true;
    std::fill(&idle_[0], &idle_[THREAD_NUM], false);
    cond_.notify_all();
    std::for_each(&drawer_[0], &drawer_[THREAD_NUM], [](auto x)
                  { x->join(); });
}

hud::HP_info_drawer::HP_info_drawer()
{
}
void hud::HP_info_drawer::draw_center_circle()
{
    if (count_ < CENTER_CIRCLE_ANIMATION_TIME)
    {
        cv::circle(image_,
                   CENTER,
                   CENTER_CIRCLE_RADIUS_IN * count_ / CENTER_CIRCLE_ANIMATION_TIME,
                   BASIC_COLOR, -1, CV_AA);
        cv::ellipse(image_,
                    CENTER,
                    cv::Size(CENTER_CIRCLE_RADIUS_OUT, CENTER_CIRCLE_RADIUS_OUT),
                    45, 0,
                    static_cast<double>(360) * count_ / CENTER_CIRCLE_ANIMATION_TIME,
                    BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
    else
    {
        cv::circle(image_,
                   CENTER,
                   CENTER_CIRCLE_RADIUS_IN,
                   BASIC_COLOR, -1, CV_AA);
        cv::circle(image_,
                   CENTER,
                   CENTER_CIRCLE_RADIUS_OUT,
                   BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
}
void hud::HP_info_drawer::draw_line_1()
{
    if (count_ < LINE_1_ANIMATION_TIME)
    {
        const cv::Point LINE_1_DELTA{
            (LINE_1_DEST - BASE_POINT.at(0)) * count_ / LINE_1_ANIMATION_TIME};

        cv::line(image_,
                 BASE_POINT.at(0),
                 BASE_POINT.at(0) + LINE_1_DELTA,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
    else
    {
        cv::line(image_,
                 BASE_POINT.at(0),
                 LINE_1_DEST,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
}
void hud::HP_info_drawer::draw_curve_1()
{
    if (LINE_1_ANIMATION_TIME <= count_)
    {
        create_bezier_curve(&image_, BEZIER_COOD_1, BASIC_COLOR, LINE_THICKNESS);
    }
}
void hud::HP_info_drawer::draw_line_2()
{
    if (LINE_2_ANIMATION_START <= count_ && count_ < LINE_2_ANIMATION_END)
    {
        const int count{count_ - LINE_2_ANIMATION_START};
        const cv::Point LINE_2_LENGTH{LINE_2_DEST - LINE_2_START};
        const cv::Point LINE_2_LENGTH_DELTA{
            LINE_2_LENGTH * count / LINE_2_ANIMATION_LENGTH};

        cv::line(image_,
                 LINE_2_START,
                 LINE_2_START + LINE_2_LENGTH_DELTA,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
    else if (LINE_2_ANIMATION_END <= count_)
    {
        cv::line(image_,
                 LINE_2_START,
                 LINE_2_DEST,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
}
void hud::HP_info_drawer::draw_line_3()
{
    if (LINE_3_ANIMATION_START <= count_ && count_ < LINE_3_ANIMATION_END)
    {
        const int count{count_ - LINE_3_ANIMATION_START};
        const cv::Point LINE_3_LENGTH{LINE_3_DEST - LINE_3_START};
        const cv::Point LINE_3_LENGTH_DELTA{
            LINE_3_LENGTH * count / LINE_3_ANIMATION_LENGTH};
        cv::line(image_,
                 LINE_3_START,
                 LINE_3_START + LINE_3_LENGTH_DELTA,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
    else if (LINE_3_ANIMATION_END <= count_)
    {
        cv::line(image_,
                 LINE_3_START,
                 LINE_3_DEST,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
}
void hud::HP_info_drawer::draw_curve_2()
{
    if (LINE_3_ANIMATION_END <= count_)
    {
        create_bezier_curve(&image_, BEZIER_COOD_2, BASIC_COLOR, LINE_THICKNESS);
    }
}
void hud::HP_info_drawer::draw_line_4()
{
    if (LINE_4_ANIMATION_START <= count_ && count_ < LINE_4_ANIMATION_END)
    {
        const int count{count_ - LINE_4_ANIMATION_START};
        const cv::Point LINE_4_LENGTH{LINE_4_DEST - LINE_4_START};
        const cv::Point LINE_4_LENGTH_DELTA{
            LINE_4_LENGTH * count / LINE_4_ANIMATION_LENGTH};
        cv::line(image_,
                 LINE_4_START,
                 LINE_4_START + LINE_4_LENGTH_DELTA,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
    else if (LINE_4_ANIMATION_END <= count_)
    {
        cv::line(image_,
                 LINE_4_START,
                 LINE_4_DEST,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
}
void hud::HP_info_drawer::draw_line_5()
{
    if (LINE_5_ANIMATION_START <= count_ && count_ < LINE_5_ANIMATION_END)
    {
        const int count{count_ - LINE_3_ANIMATION_START};
        const cv::Point LINE_5_LENGTH{LINE_5_DEST - LINE_5_START};
        const cv::Point LINE_5_LENGTH_DELTA{
            LINE_5_LENGTH * count / LINE_5_ANIMATION_LENGTH};

        cv::line(image_,
                 LINE_5_START,
                 LINE_5_START + LINE_5_LENGTH_DELTA,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
    else if (LINE_5_ANIMATION_END <= count_)
    {
        cv::line(image_,
                 LINE_5_START,
                 LINE_5_DEST,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
}
void hud::HP_info_drawer::draw_circle_2()
{
    if (11 <= count_ && count_ <= 30)
    {
        const int count{count_ - CIRCLE_2_OUT_ANIMATION_START};
        cv::circle(image_,
                   BASE_POINT.at(2),
                   CIRCLE_2_RADIUS_OUT * count / CIRCLE_2_OUT_ANIMATION_LENGTH,
                   BASIC_COLOR, -1, CV_AA);
    }
    else if (30 < count_)
    {
        cv::circle(image_,
                   BASE_POINT.at(2),
                   CIRCLE_2_RADIUS_OUT,
                   BASIC_COLOR, -1, CV_AA);
    }
    if (21 <= count_ && count_ <= 40)
    {
        int count = count_ - CIRCLE_2_IN_ANIMATION_START;
        cv::circle(image_,
                   BASE_POINT.at(2),
                   CIRCLE_2_RADIUS_IN * count / CIRCLE_2_IN_ANIMATION_LENGTH,
                   cv::Scalar(0), -1, CV_AA);
    }
    else if (40 < count_)
    {
        cv::circle(image_,
                   BASE_POINT.at(2),
                   CIRCLE_2_RADIUS_IN,
                   cv::Scalar(0), -1, CV_AA);
    }
}
void hud::HP_info_drawer::draw_HP_bar()
{
    if (HP_BAR_BACK_ANIMATION_START <= count_ &&
        count_ <= HP_BAR_BACK_ANIMATION_END)
    {
        const int count{count_ - HP_BAR_BACK_ANIMATION_START};
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size(HP_BAR_RADIUS_OUT, HP_BAR_RADIUS_OUT),
                    270, 0, 360.0f * count / HP_BAR_BACK_ANIMATION_LENGTH,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size(HP_BAR_RADIUS_IN, HP_BAR_RADIUS_IN),
                    0, 0, 360,
                    cv::Scalar(0), -1, CV_AA);
    }
    else if (HP_BAR_BACK_ANIMATION_END < count_)
    {
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size(HP_BAR_RADIUS_OUT, HP_BAR_RADIUS_OUT),
                    0, 0, 360,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size(HP_BAR_RADIUS_IN, HP_BAR_RADIUS_IN),
                    0, 0, 360,
                    cv::Scalar(0), -1, CV_AA);
    }
    // HP bar foreground
    if (HP_BAR_FORE_ANIMATION_START <= count_ &&
        count_ <= HP_BAR_FORE_ANIMATION_END)
    {
        const int count{count_ - HP_BAR_FORE_ANIMATION_START};
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size{HP_BAR_RADIUS_OUT, HP_BAR_RADIUS_OUT},
                    270, 0, 360.0f * count / HP_BAR_FORE_ANIMATION_LENGTH,
                    HP_BAR_FORE, -1, CV_AA);
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size{HP_BAR_RADIUS_IN, HP_BAR_RADIUS_IN},
                    0, 0, 360,
                    cv::Scalar(0), -1, CV_AA);
    }
    else if (HP_BAR_FORE_ANIMATION_END < count_)
    {
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size{HP_BAR_RADIUS_OUT, HP_BAR_RADIUS_OUT},
                    -90, 0, 360 * health_ / max_health_,
                    HP_BAR_FORE, -1, CV_AA);
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size{HP_BAR_RADIUS_IN, HP_BAR_RADIUS_IN},
                    0, 0, 360,
                    cv::Scalar(0), -1, CV_AA);
    }

    if (45 <= count_ && count_ < 75)
    {
        int count{count_ - 45};
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size{HP_BAR_RADIUS_IN - 5, HP_BAR_RADIUS_IN - 5},
                    270, 0, 360 * count / 30,
                    HP_BAR_FORE, LINE_THICKNESS, CV_AA);
    }
    else if (75 <= count_)
    {
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size{HP_BAR_RADIUS_IN - 5, HP_BAR_RADIUS_IN - 5},
                    270, 0, 360,
                    HP_BAR_FORE, LINE_THICKNESS, CV_AA);
    }
}
void hud::HP_info_drawer::draw_Time_bar()
{
    // prevent from dividing by zero
    if (power_.second == 0)
    {
        return;
    }
    // Strength bar
    if (TIME_BAR_BACK_ANIMATION_START <= count_ &&
        count_ <= TIME_BAR_BACK_ANIMATION_END)
    {
        const int count = count_ - TIME_BAR_BACK_ANIMATION_START;

        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(45) + 5 - 1, 0,
                    (90 - 10) * count / TIME_BAR_BACK_ANIMATION_LENGTH,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(90) + 45 + 5 - 3, 0,
                    (90 - 10) * count / TIME_BAR_BACK_ANIMATION_LENGTH,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(180) + 45 + 5 + 3, 0,
                    (90 - 10) * count / TIME_BAR_BACK_ANIMATION_LENGTH,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(270) + 45 + 5 + 1, 0,
                    (90 - 10) * count / TIME_BAR_BACK_ANIMATION_LENGTH,
                    HP_BAR_BACK, -1, CV_AA);

        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_IN, TIME_BAR_RADIUS_IN},
                    0, 0, 360,
                    cv::Scalar{0}, -1, CV_AA);
    }
    else if (TIME_BAR_BACK_ANIMATION_END < count_)
    {
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(45) + 5 - 1,
                    0,
                    static_cast<double>(90) - 10,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(90) + 45 + 5 - 3,
                    0,
                    static_cast<double>(90) - 10,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(180) + 45 + 5 + 3,
                    0,
                    static_cast<double>(90) - 10,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(270) + 45 + 5 + 1,
                    0,
                    static_cast<double>(90) - 10,
                    HP_BAR_BACK, -1, CV_AA);

        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_IN, TIME_BAR_RADIUS_IN},
                    0, 0, 360,
                    cv::Scalar{0}, -1, CV_AA);
    }

    if (TIME_BAR_FORE_ANIMATION_START <= count_ &&
        count_ <= TIME_BAR_FORE_ANIMATION_END)
    {
        const int count{count_ - TIME_BAR_FORE_ANIMATION_START};

        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(45) + 5 - 1, 0,
                    (90 - 10) * count / TIME_BAR_FORE_ANIMATION_LENGTH,
                    HP_BAR_FORE, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(90) + 45 + 5 - 3, 0,
                    (90 - 10) * count / TIME_BAR_FORE_ANIMATION_LENGTH,
                    HP_BAR_FORE, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(180) + 45 + 5 + 3, 0,
                    (90 - 10) * count / TIME_BAR_FORE_ANIMATION_LENGTH,
                    HP_BAR_FORE, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(270) + 45 + 5 + 1, 0,
                    (90 - 10) * count / TIME_BAR_FORE_ANIMATION_LENGTH,
                    HP_BAR_FORE, -1, CV_AA);

        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_IN, TIME_BAR_RADIUS_IN),
                    0, 0, 360,
                    cv::Scalar(0), -1, CV_AA);
    }
    else if (TIME_BAR_FORE_ANIMATION_END < count_)
    {
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(45) + 5 - 1,
                    0, (90 - 10) * power_.first / power_.second,
                    HP_BAR_FORE, -1, CV_AA);
        power_ = main_chara_->get_normal_move_power(normal_move_name_.at(1));
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(90) + 45 + 5 - 3,
                    0, (90 - 10) * power_.first / power_.second,
                    HP_BAR_FORE, -1, CV_AA);
        power_ = main_chara_->get_normal_move_power(normal_move_name_.at(2));
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(180) + 45 + 5 + 3,
                    0, (90 - 10) * power_.first / power_.second,
                    HP_BAR_FORE, -1, CV_AA);
        power_ = main_chara_->get_normal_move_power(normal_move_name_.at(3));
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(270) + 45 + 5 + 1,
                    0, (90 - 10) * power_.first / power_.second,
                    HP_BAR_FORE, -1, CV_AA);

        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_IN, TIME_BAR_RADIUS_IN),
                    0, 0, 360,
                    cv::Scalar(0), -1, CV_AA);
    }
}

// TODO animation
void hud::remove_HP_info()
{
    {
        decltype(textures_)::iterator it{
            std::find_if(textures_.begin(), textures_.end(),
                         [&](auto &&x)
                         { return x.id_ == "early_HP"; })};
        if (it != textures_.end())
        {
            textures_.erase(it);
        }

        HP_info_.reset();
    }

    {
        decltype(textures_)::iterator it{
            std::find_if(textures_.begin(), textures_.end(),
                         [&](auto &&x)
                         { return x.id_ == "enemy_HP"; })};
        if (it != textures_.end())
        {
            textures_.erase(it);
        }

        HP_info2_.reset();
    }
}

hud::HP_info_animator::HP_info_animator()
    : HP_info_drawer_{new HP_info_drawer{}}
{
    (*HP_info_drawer_)();
}
hud::HP_info_animator2::HP_info_animator2()
    : HP_info_drawer2_{new HP_info_drawer2{}}
{
    (*HP_info_drawer2_)();
}

void hud::HP_info_animator2::operator()(HP_info2 &hp_info, main_window &window)
{
    while (true)
    {
        bool all_idle = std::all_of(HP_info_drawer2_->get_idles(),
                                    HP_info_drawer2_->get_idles() + HP_info_drawer2::THREAD_NUM,
                                    [](const auto x)
                                    { return x; });
        if (all_idle)
        {
            break;
        }
    }

    static D3DLOCKED_RECT locked_rect{};
    hp_info.texture_->value_->LockRect(0, &locked_rect, nullptr, 0);

    DWORD *pTexBuf = (DWORD *)locked_rect.pBits;
    for (int i{0}; i < 1024; ++i)
    {
        std::copy(&HP_info_drawer2_->get_image().ptr<DWORD>(i)[0],
                  &HP_info_drawer2_->get_image().ptr<DWORD>(i)[1024], &pTexBuf[i * 1024]);
    }

    hp_info.texture_->value_->UnlockRect(0);

    const std::shared_ptr<character> &main_chara{window.get_main_character()};
    const std::shared_ptr<character> &enemy{window.get_enemy_character()};

    HP_info_drawer2_->main_chara_ = main_chara;
    HP_info_drawer2_->enemy_ = enemy;

    HP_info_drawer2_->health_ = enemy->get_health();
    HP_info_drawer2_->max_health_ = enemy->get_max_health();
    HP_info_drawer2_->normal_move_name_ = main_chara->get_normal_move();
    HP_info_drawer2_->power_ = main_chara->get_normal_move_power(
        main_chara->get_normal_move().at(0));

    D3DXVECTOR3 position{enemy->get_position()};
    position.y += 1.0f;
    const cv::Point coodinate{window.get_screen_coodinate(position)};
    const cv::Point CENTER{HP_info2::TEXTURE_SIZE.width / 2,
                           HP_info2::TEXTURE_SIZE.height / 2};
    hp_info.texture_->rect_.x = coodinate.x - CENTER.x;
    hp_info.texture_->rect_.y = coodinate.y - CENTER.y;

    int count = HP_info_drawer2_->get_count();
    if (count > 90)
    {
        HP_info_drawer2_->set_type(HP_info_drawer2::anim_type::APPEARED);
    }

    HP_info_drawer2_->set_count(count + 1);
    HP_info_drawer2_->set_charge_func_index(0);
    std::fill(HP_info_drawer2_->get_idles(),
              HP_info_drawer2_->get_idles() + HP_info_drawer2::THREAD_NUM,
              false);
    HP_info_drawer2_->get_idle_condition_variable().notify_all();

    return;
}

void create_bezier_curve(cv::Mat *image,
                         const std::array<cv::Point, 3> &base_point,
                         const cv::Scalar &color,
                         const int &line_thickness)
{
    const std::array<cv::Point2f, 3> &base_pointf{
        static_cast<cv::Point2f>(base_point.at(0)),
        static_cast<cv::Point2f>(base_point.at(1)),
        static_cast<cv::Point2f>(base_point.at(2))};

    const int FINENESS{8};
    vector<cv::Point2f> point(FINENESS);

    for (int i{0}; i < FINENESS; ++i)
    {
        double d{1.0f - static_cast<double>(i) / FINENESS};
        point.at(i) = base_pointf.at(0) * d * d +
                      2 * base_pointf.at(1) * d * (1.0f - d) +
                      base_pointf.at(2) * (1.0f - d) * (1.0f - d);
    }

    point.push_back(base_point.at(2));

    for (int i{0}; i < FINENESS; ++i)
    {
        cv::line(*image,
                 point.at(i),
                 point.at(static_cast<decltype(point)::size_type>(i) + 1),
                 color, line_thickness, CV_AA);
    }
}

void hud::HP_info_animator::operator()(HP_info &hp_info, main_window &window)
{
    while (true)
    {
        bool all_idle = std::all_of(HP_info_drawer_->get_idles(),
                                    HP_info_drawer_->get_idles() + HP_info_drawer::THREAD_NUM,
                                    [](const auto x)
                                    { return x; });
        if (all_idle)
        {
            break;
        }
    }

    static D3DLOCKED_RECT locked_rect{};
    hp_info.texture_->value_->LockRect(0, &locked_rect, nullptr, 0);

    DWORD *pTexBuf = (DWORD *)locked_rect.pBits;
    for (int i{0}; i < 1024; ++i)
    {
        std::copy(&HP_info_drawer_->get_image().ptr<DWORD>(i)[0],
                  &HP_info_drawer_->get_image().ptr<DWORD>(i)[1024], &pTexBuf[i * 1024]);
    }

    hp_info.texture_->value_->UnlockRect(0);

    const std::shared_ptr<character> &main_chara{window.get_main_character()};
    const std::shared_ptr<character> &enemy{window.get_enemy_character()};

    HP_info_drawer_->main_chara_ = main_chara;
    HP_info_drawer_->enemy_ = enemy;

    HP_info_drawer_->health_ = main_chara->get_health();
    HP_info_drawer_->max_health_ = main_chara->get_max_health();
    HP_info_drawer_->normal_move_name_ = main_chara->get_normal_move();
    HP_info_drawer_->power_ = main_chara->get_normal_move_power(
        main_chara->get_normal_move().at(0));

    D3DXVECTOR3 position{main_chara->get_position()};
    position.y += 1.0f;
    const cv::Point coodinate{window.get_screen_coodinate(position)};
    const cv::Point CENTER{HP_info::TEXTURE_SIZE.width / 2,
                           HP_info::TEXTURE_SIZE.height / 2};
    hp_info.texture_->rect_.x = coodinate.x - CENTER.x;
    hp_info.texture_->rect_.y = coodinate.y - CENTER.y;

    int count = HP_info_drawer_->get_count();
    if (count > 90)
    {
        HP_info_drawer_->set_type(HP_info_drawer::anim_type::APPEARED);
    }

    HP_info_drawer_->set_count(count + 1);
    HP_info_drawer_->set_charge_func_index(0);
    std::fill(HP_info_drawer_->get_idles(),
              HP_info_drawer_->get_idles() + HP_info_drawer::THREAD_NUM,
              false);
    HP_info_drawer_->get_idle_condition_variable().notify_all();

    return;
}

void hud::operator()(main_window &a_main_window)
{
    vector<decltype(frame_list_)::iterator> abandon;
    for (decltype(frame_list_)::iterator frame{frame_list_.begin()};
         frame != frame_list_.end(); ++frame)
    {
        if (frame->frame_animator_ != nullptr)
        {
            if (!(*frame->frame_animator_)(*frame))
            {
                abandon.push_back(frame);
            }
        }
    }

    for (auto &&x : abandon)
    {
        textures_.erase(x->texture_);
        frame_list_.erase(x);
    }

    for (auto &&message : message_list_)
    {
        if (message.message_writer_ != nullptr)
        {
            (*message.message_writer_)();
        }
    }

    if (HP_info_ != nullptr && HP_info_->HP_info_animator_ != nullptr)
    {
        (*HP_info_->HP_info_animator_)(*HP_info_, a_main_window);
    }
    if (HP_info2_ != nullptr && HP_info2_->HP_info_animator_ != nullptr)
    {
        (*HP_info2_->HP_info_animator_)(*HP_info2_, a_main_window);
    }

    sprite_->Begin(D3DXSPRITE_ALPHABLEND);
    for (decltype(textures_)::iterator it = textures_.begin();
         it != textures_.end(); ++it)
    {
        RECT rect = {0, 0, it->rect_.width, it->rect_.height};
        D3DXVECTOR3 center(0, 0, 0);
        D3DXVECTOR3 position(static_cast<float>(it->rect_.x),
                             static_cast<float>(it->rect_.y), 0);
        sprite_->Draw(it->value_.get(),
                      nullptr, &center, &position,
                      D3DCOLOR_ARGB(255, 255, 255, 255));
    }
    sprite_->End();
}
void hud::HP_info_drawer2::draw_text_stage_number_1(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    if (CIRCLE_2_IN_ANIMATION_END <= count_ &&
        count_ < CIRCLE_2_IN_ANIMATION_END + LETTER_FADE_DURATION)
    {
        const int count{count_ - CIRCLE_2_IN_ANIMATION_END};
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        // stage number
        writer->add_text("Stage No",
                         BASE_POINT.at(2) - cv::Point(87, 21),
                         letter_color);
    }
    else if (CIRCLE_2_IN_ANIMATION_END + LETTER_FADE_DURATION <= count_)
    {
        // stage number
        writer->add_text("Stage No",
                         BASE_POINT.at(2) - cv::Point(87, 21),
                         LETTER_COLOR);
    }
    copy_text(writer);
}

void hud::HP_info_drawer2::draw_text_stage_number_2(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    if (CIRCLE_2_IN_ANIMATION_END <= count_ &&
        count_ < CIRCLE_2_IN_ANIMATION_END + LETTER_FADE_DURATION)
    {
        const int count{count_ - CIRCLE_2_IN_ANIMATION_END};
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        // stage number
        writer->add_text("13",
                         BASE_POINT.at(2) - cv::Point(8, 9),
                         letter_color);
    }
    else if (CIRCLE_2_IN_ANIMATION_END + LETTER_FADE_DURATION <= count_)
    {
        // stage number
        writer->add_text("13",
                         BASE_POINT.at(2) - cv::Point(8, 9),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer2::copy_text(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    const cv::Point &start_point = writer->start_point_;
    const cv::Size &canvas_size = writer->canvas_size_;
    const vector<vector<DWORD>> &text_image_ = writer->text_image_;
    for (size_t j{}; j < canvas_size.height; ++j)
    {
        DWORD *row = image_.ptr<DWORD>(static_cast<int>(j + start_point.y));
        row += start_point.x;
        std::transform(text_image_[j].cbegin(), text_image_[j].cend(), row, row,
                       [](const auto &src, const auto &dst)
                       {
                           return (src & 0xff000000UL) != 0 ? src : dst;
                           //            return src > 0x00ffffffUL ? src : dst;
                       });
    }
}
void hud::HP_info_drawer2::draw_text_time_1(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    if (TIME_BAR_BACK_ANIMATION_END <= count_ &&
        count_ < TIME_BAR_BACK_ANIMATION_END + LETTER_FADE_DURATION)
    {
        const int count = count_ - TIME_BAR_BACK_ANIMATION_END;
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        writer->add_text("Time", BASE_POINT.at(5) + cv::Point(-18, -19),
                         letter_color);
    }
    else if (TIME_BAR_BACK_ANIMATION_END + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text("Time", BASE_POINT.at(5) + cv::Point(-18, -19),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer2::draw_text_time_2(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    if (TIME_BAR_BACK_ANIMATION_END <= count_ &&
        count_ < TIME_BAR_BACK_ANIMATION_END + LETTER_FADE_DURATION)
    {
        const int count = count_ - TIME_BAR_BACK_ANIMATION_END;
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        writer->add_text("99", BASE_POINT.at(5) + cv::Point(-12, -2),
                         letter_color);
    }
    else if (TIME_BAR_BACK_ANIMATION_END + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text("99", BASE_POINT.at(5) + cv::Point(-12, -2),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer2::draw_text_strength_1(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    string text{""};
    if (!main_chara_ || main_chara_->get_normal_move().empty())
    {
        return;
    }
    text = main_chara_->get_normal_move().at(0);
    text += ":";
    text += std::to_string(main_chara_->get_normal_move_power(
                                          main_chara_->get_normal_move().at(0))
                               .first);
    if (TIME_BAR_BACK_ANIMATION_START + 5 <= count_ &&
        count_ < TIME_BAR_BACK_ANIMATION_START + 5 + LETTER_FADE_DURATION)
    {
        const int count{count_ - TIME_BAR_BACK_ANIMATION_START - 5};
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        writer->add_text(text, BASE_POINT.at(5) + cv::Point(-30, -60 - 9),
                         letter_color);
    }
    else if (TIME_BAR_BACK_ANIMATION_START + 5 + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text(text, BASE_POINT.at(5) + cv::Point(-30, -60 - 9),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer2::draw_text_strength_2(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    string text{""};
    if (!main_chara_ || main_chara_->get_normal_move().size() < 2)
    {
        return;
    }
    text = main_chara_->get_normal_move().at(1);
    text += ":";
    text += std::to_string(main_chara_->get_normal_move_power(
                                          main_chara_->get_normal_move().at(1))
                               .first);
    if (TIME_BAR_BACK_ANIMATION_START + 10 <= count_ &&
        count_ < TIME_BAR_BACK_ANIMATION_START + 10 + LETTER_FADE_DURATION)
    {
        const int count{count_ - TIME_BAR_BACK_ANIMATION_START - 10};
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        writer->add_text(text, BASE_POINT.at(5) + cv::Point(50, -9),
                         letter_color);
    }
    else if (TIME_BAR_BACK_ANIMATION_START + 5 + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text(text, BASE_POINT.at(5) + cv::Point(50, -9),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer2::draw_text_strength_3(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    string text{""};
    if (!main_chara_ || main_chara_->get_normal_move().size() < 3)
    {
        return;
    }
    text = main_chara_->get_normal_move().at(2);
    text += ":";
    text += std::to_string(main_chara_->get_normal_move_power(
                                          main_chara_->get_normal_move().at(2))
                               .first);
    if (TIME_BAR_BACK_ANIMATION_START + 15 <= count_ &&
        count_ < TIME_BAR_BACK_ANIMATION_START + 15 + LETTER_FADE_DURATION)
    {
        const int count{count_ - TIME_BAR_BACK_ANIMATION_START - 15};
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        writer->add_text(text, BASE_POINT.at(5) + cv::Point(-30, 60 - 9),
                         letter_color);
    }
    else if (TIME_BAR_BACK_ANIMATION_START + 5 + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text(text, BASE_POINT.at(5) + cv::Point(-30, 60 - 9),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer2::draw_text_strength_4(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    string text{""};
    if (!main_chara_ || main_chara_->get_normal_move().size() < 4)
    {
        return;
    }
    text = main_chara_->get_normal_move().at(3);
    text += ":";
    text += std::to_string(main_chara_->get_normal_move_power(
                                          main_chara_->get_normal_move().at(3))
                               .first);
    if (TIME_BAR_BACK_ANIMATION_START + 20 <= count_ &&
        count_ < TIME_BAR_BACK_ANIMATION_START + 20 + LETTER_FADE_DURATION)
    {
        const int count{count_ - TIME_BAR_BACK_ANIMATION_START - 20};
        DWORD letter_color{D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]))};

        writer->add_text(text, BASE_POINT.at(5) + cv::Point(-140, -9),
                         letter_color);
    }
    else if (TIME_BAR_BACK_ANIMATION_START + 5 + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text(text, BASE_POINT.at(5) + cv::Point(-140, -9),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer2::draw_text_HP_1(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    if (LINE_4_ANIMATION_END <= count_ &&
        count_ < LINE_4_ANIMATION_END + LETTER_FADE_DURATION)
    {
        const int count{count_ - LINE_4_ANIMATION_END};
        DWORD letter_color = D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]));

        writer->add_text("HP", BASE_POINT.at(4) + cv::Point(-10, -30),
                         letter_color);
    }
    else if (LINE_4_ANIMATION_END + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text("HP", BASE_POINT.at(4) + cv::Point(-10, -30),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer2::draw_text_HP_2(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    if (!main_chara_ || !enemy_)
    {
        return;
    }
    const int health = enemy_->get_health();
    const string sz_health{boost::lexical_cast<string>(health)};

    if (LINE_4_ANIMATION_END <= count_ &&
        count_ < LINE_4_ANIMATION_END + LETTER_FADE_DURATION)
    {
        const int count{count_ - LINE_4_ANIMATION_END};
        DWORD letter_color = D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]));

        writer->add_text(sz_health, BASE_POINT.at(4) + cv::Point(-33, -10),
                         letter_color);
    }
    else if (LINE_4_ANIMATION_END + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text(sz_health, BASE_POINT.at(4) + cv::Point(-33, -10),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer2::draw_text_HP_3(
    const std::shared_ptr<message_writer_for_thread> &writer)
{
    if (!main_chara_ || !enemy_)
    {
        return;
    }
    const int max_health = enemy_->get_max_health();
    const string sz_max_health{
        "/ " + boost::lexical_cast<string>(max_health)};

    if (LINE_4_ANIMATION_END <= count_ &&
        count_ < LINE_4_ANIMATION_END + LETTER_FADE_DURATION)
    {
        const int count{count_ - LINE_4_ANIMATION_END};
        DWORD letter_color = D3DCOLOR_ARGB(
            static_cast<int>(BASIC_COLOR[3] * count / LETTER_FADE_DURATION),
            static_cast<int>(BASIC_COLOR[2]),
            static_cast<int>(BASIC_COLOR[1]),
            static_cast<int>(BASIC_COLOR[0]));

        writer->add_text(sz_max_health, BASE_POINT.at(4) + cv::Point(-20, 10),
                         letter_color);
    }
    else if (LINE_4_ANIMATION_END + LETTER_FADE_DURATION <= count_)
    {
        writer->add_text(sz_max_health, BASE_POINT.at(4) + cv::Point(-20, 10),
                         LETTER_COLOR);
    }
    copy_text(writer);
}
void hud::HP_info_drawer2::operator()()
{
    funcs_.push_back([&]
                     { draw_Time_bar(); });
    funcs_.push_back([&]
                     { draw_center_circle(); });
    funcs_.push_back([&]
                     { draw_line_1(); });
    funcs_.push_back([&]
                     { draw_curve_1(); });
    funcs_.push_back([&]
                     { draw_line_2(); });
    funcs_.push_back([&]
                     { draw_line_3(); });
    funcs_.push_back([&]
                     { draw_curve_2(); });
    funcs_.push_back([&]
                     { draw_line_4(); });
    funcs_.push_back([&]
                     { draw_line_5(); });
    funcs_.push_back([&]
                     { draw_circle_2(); });
    funcs_.push_back([&]
                     { draw_HP_bar(); });
    std::shared_ptr<message_writer_for_thread> temp_writer{
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{100, 20}}};
    funcs_.push_back([&, temp_writer]
                     { draw_text_stage_number_1(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{30, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_stage_number_2(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{40, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_time_1(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 30, cv::Size{30, 30}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_time_2(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{90, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_strength_1(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{90, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_strength_2(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{90, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_strength_3(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{90, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_strength_4(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{30, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_HP_1(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{50, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_HP_2(temp_writer); });

    temp_writer.reset(
        new message_writer_for_thread{"游ゴシック", 20, cv::Size{70, 20}});
    funcs_.push_back([&, temp_writer]
                     { draw_text_HP_3(temp_writer); });

    for (int i = 0; i < THREAD_NUM; ++i)
    {
        drawer_[i].reset(new std::thread([&, i]
                                         {
  while(true) {
    if (finish_request_) {
      break;
    }
    int charge_func_index = 0;
    // block scope for mutex
    {
      std::lock_guard<std::mutex> lock(charge_func_index_mtx_);
      charge_func_index = charge_func_index_;

//          early_go::log_liner() << "i: " << i
//              << "  charge_func_index_mtx_: " << charge_func_index;
      if (charge_func_index < funcs_.size()) {
        ++charge_func_index_;
      }
    }
    if (charge_func_index < funcs_.size()) {
      funcs_[charge_func_index]();
    } else {
      // block scope for mutex
      {
        std::unique_lock<std::mutex> lock(idle_mtx_);
        idle_[i] = true;
        cond_.wait(lock, [this, i]{return idle_[i] == false;});
      }
    }
  } }));
    }
}

hud::HP_info_drawer2::~HP_info_drawer2()
{
    finish_request_ = true;
    std::fill(&idle_[0], &idle_[THREAD_NUM], false);
    cond_.notify_all();
    std::for_each(&drawer_[0], &drawer_[THREAD_NUM], [](auto x)
                  { x->join(); });
}

hud::HP_info_drawer2::HP_info_drawer2()
{
}
void hud::HP_info_drawer2::draw_center_circle()
{
    if (count_ < CENTER_CIRCLE_ANIMATION_TIME)
    {
        cv::circle(image_,
                   CENTER,
                   CENTER_CIRCLE_RADIUS_IN * count_ / CENTER_CIRCLE_ANIMATION_TIME,
                   BASIC_COLOR, -1, CV_AA);
        cv::ellipse(image_,
                    CENTER,
                    cv::Size(CENTER_CIRCLE_RADIUS_OUT, CENTER_CIRCLE_RADIUS_OUT),
                    45, 0,
                    static_cast<double>(360) * count_ / CENTER_CIRCLE_ANIMATION_TIME,
                    BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
    else
    {
        cv::circle(image_,
                   CENTER,
                   CENTER_CIRCLE_RADIUS_IN,
                   BASIC_COLOR, -1, CV_AA);
        cv::circle(image_,
                   CENTER,
                   CENTER_CIRCLE_RADIUS_OUT,
                   BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
}
void hud::HP_info_drawer2::draw_line_1()
{
    if (count_ < LINE_1_ANIMATION_TIME)
    {
        const cv::Point LINE_1_DELTA{
            (LINE_1_DEST - BASE_POINT.at(0)) * count_ / LINE_1_ANIMATION_TIME};

        cv::line(image_,
                 BASE_POINT.at(0),
                 BASE_POINT.at(0) + LINE_1_DELTA,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
    else
    {
        cv::line(image_,
                 BASE_POINT.at(0),
                 LINE_1_DEST,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
}
void hud::HP_info_drawer2::draw_curve_1()
{
    if (LINE_1_ANIMATION_TIME <= count_)
    {
        create_bezier_curve(&image_, BEZIER_COOD_1, BASIC_COLOR, LINE_THICKNESS);
    }
}
void hud::HP_info_drawer2::draw_line_2()
{
    if (LINE_2_ANIMATION_START <= count_ && count_ < LINE_2_ANIMATION_END)
    {
        const int count{count_ - LINE_2_ANIMATION_START};
        const cv::Point LINE_2_LENGTH{LINE_2_DEST - LINE_2_START};
        const cv::Point LINE_2_LENGTH_DELTA{
            LINE_2_LENGTH * count / LINE_2_ANIMATION_LENGTH};

        cv::line(image_,
                 LINE_2_START,
                 LINE_2_START + LINE_2_LENGTH_DELTA,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
    else if (LINE_2_ANIMATION_END <= count_)
    {
        cv::line(image_,
                 LINE_2_START,
                 LINE_2_DEST,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
}
void hud::HP_info_drawer2::draw_line_3()
{
    if (LINE_3_ANIMATION_START <= count_ && count_ < LINE_3_ANIMATION_END)
    {
        const int count{count_ - LINE_3_ANIMATION_START};
        const cv::Point LINE_3_LENGTH{LINE_3_DEST - LINE_3_START};
        const cv::Point LINE_3_LENGTH_DELTA{
            LINE_3_LENGTH * count / LINE_3_ANIMATION_LENGTH};
        cv::line(image_,
                 LINE_3_START,
                 LINE_3_START + LINE_3_LENGTH_DELTA,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
    else if (LINE_3_ANIMATION_END <= count_)
    {
        cv::line(image_,
                 LINE_3_START,
                 LINE_3_DEST,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
}
void hud::HP_info_drawer2::draw_curve_2()
{
    if (LINE_3_ANIMATION_END <= count_)
    {
        create_bezier_curve(&image_, BEZIER_COOD_2, BASIC_COLOR, LINE_THICKNESS);
    }
}
void hud::HP_info_drawer2::draw_line_4()
{
    if (LINE_4_ANIMATION_START <= count_ && count_ < LINE_4_ANIMATION_END)
    {
        const int count{count_ - LINE_4_ANIMATION_START};
        const cv::Point LINE_4_LENGTH{LINE_4_DEST - LINE_4_START};
        const cv::Point LINE_4_LENGTH_DELTA{
            LINE_4_LENGTH * count / LINE_4_ANIMATION_LENGTH};
        cv::line(image_,
                 LINE_4_START,
                 LINE_4_START + LINE_4_LENGTH_DELTA,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
    else if (LINE_4_ANIMATION_END <= count_)
    {
        cv::line(image_,
                 LINE_4_START,
                 LINE_4_DEST,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
}
void hud::HP_info_drawer2::draw_line_5()
{
    if (LINE_5_ANIMATION_START <= count_ && count_ < LINE_5_ANIMATION_END)
    {
        const int count{count_ - LINE_3_ANIMATION_START};
        const cv::Point LINE_5_LENGTH{LINE_5_DEST - LINE_5_START};
        const cv::Point LINE_5_LENGTH_DELTA{
            LINE_5_LENGTH * count / LINE_5_ANIMATION_LENGTH};

        cv::line(image_,
                 LINE_5_START,
                 LINE_5_START + LINE_5_LENGTH_DELTA,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
    else if (LINE_5_ANIMATION_END <= count_)
    {
        cv::line(image_,
                 LINE_5_START,
                 LINE_5_DEST,
                 BASIC_COLOR, LINE_THICKNESS, CV_AA);
    }
}
void hud::HP_info_drawer2::draw_circle_2()
{
    if (11 <= count_ && count_ <= 30)
    {
        const int count{count_ - CIRCLE_2_OUT_ANIMATION_START};
        cv::circle(image_,
                   BASE_POINT.at(2),
                   CIRCLE_2_RADIUS_OUT * count / CIRCLE_2_OUT_ANIMATION_LENGTH,
                   BASIC_COLOR, -1, CV_AA);
    }
    else if (30 < count_)
    {
        cv::circle(image_,
                   BASE_POINT.at(2),
                   CIRCLE_2_RADIUS_OUT,
                   BASIC_COLOR, -1, CV_AA);
    }
    if (21 <= count_ && count_ <= 40)
    {
        int count = count_ - CIRCLE_2_IN_ANIMATION_START;
        cv::circle(image_,
                   BASE_POINT.at(2),
                   CIRCLE_2_RADIUS_IN * count / CIRCLE_2_IN_ANIMATION_LENGTH,
                   cv::Scalar(0), -1, CV_AA);
    }
    else if (40 < count_)
    {
        cv::circle(image_,
                   BASE_POINT.at(2),
                   CIRCLE_2_RADIUS_IN,
                   cv::Scalar(0), -1, CV_AA);
    }
}
void hud::HP_info_drawer2::draw_HP_bar()
{
    if (HP_BAR_BACK_ANIMATION_START <= count_ &&
        count_ <= HP_BAR_BACK_ANIMATION_END)
    {
        const int count{count_ - HP_BAR_BACK_ANIMATION_START};
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size(HP_BAR_RADIUS_OUT, HP_BAR_RADIUS_OUT),
                    270, 0, 360.0f * count / HP_BAR_BACK_ANIMATION_LENGTH,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size(HP_BAR_RADIUS_IN, HP_BAR_RADIUS_IN),
                    0, 0, 360,
                    cv::Scalar(0), -1, CV_AA);
    }
    else if (HP_BAR_BACK_ANIMATION_END < count_)
    {
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size(HP_BAR_RADIUS_OUT, HP_BAR_RADIUS_OUT),
                    0, 0, 360,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size(HP_BAR_RADIUS_IN, HP_BAR_RADIUS_IN),
                    0, 0, 360,
                    cv::Scalar(0), -1, CV_AA);
    }
    // HP bar foreground
    if (HP_BAR_FORE_ANIMATION_START <= count_ &&
        count_ <= HP_BAR_FORE_ANIMATION_END)
    {
        const int count{count_ - HP_BAR_FORE_ANIMATION_START};
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size{HP_BAR_RADIUS_OUT, HP_BAR_RADIUS_OUT},
                    270, 0, 360.0f * count / HP_BAR_FORE_ANIMATION_LENGTH,
                    HP_BAR_FORE, -1, CV_AA);
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size{HP_BAR_RADIUS_IN, HP_BAR_RADIUS_IN},
                    0, 0, 360,
                    cv::Scalar(0), -1, CV_AA);
    }
    else if (HP_BAR_FORE_ANIMATION_END < count_)
    {
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size{HP_BAR_RADIUS_OUT, HP_BAR_RADIUS_OUT},
                    -90, 0, 360 * health_ / max_health_,
                    HP_BAR_FORE, -1, CV_AA);
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size{HP_BAR_RADIUS_IN, HP_BAR_RADIUS_IN},
                    0, 0, 360,
                    cv::Scalar(0), -1, CV_AA);
    }

    if (45 <= count_ && count_ < 75)
    {
        int count{count_ - 45};
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size{HP_BAR_RADIUS_IN - 5, HP_BAR_RADIUS_IN - 5},
                    270, 0, 360 * count / 30,
                    HP_BAR_FORE, LINE_THICKNESS, CV_AA);
    }
    else if (75 <= count_)
    {
        cv::ellipse(image_,
                    HP_BAR_CENTER,
                    cv::Size{HP_BAR_RADIUS_IN - 5, HP_BAR_RADIUS_IN - 5},
                    270, 0, 360,
                    HP_BAR_FORE, LINE_THICKNESS, CV_AA);
    }
}
void hud::HP_info_drawer2::draw_Time_bar()
{
    // prevent from dividing by zero
    if (power_.second == 0)
    {
        return;
    }
    // Strength bar
    if (TIME_BAR_BACK_ANIMATION_START <= count_ &&
        count_ <= TIME_BAR_BACK_ANIMATION_END)
    {
        const int count = count_ - TIME_BAR_BACK_ANIMATION_START;

        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(45) + 5 - 1, 0,
                    (90 - 10) * count / TIME_BAR_BACK_ANIMATION_LENGTH,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(90) + 45 + 5 - 3, 0,
                    (90 - 10) * count / TIME_BAR_BACK_ANIMATION_LENGTH,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(180) + 45 + 5 + 3, 0,
                    (90 - 10) * count / TIME_BAR_BACK_ANIMATION_LENGTH,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(270) + 45 + 5 + 1, 0,
                    (90 - 10) * count / TIME_BAR_BACK_ANIMATION_LENGTH,
                    HP_BAR_BACK, -1, CV_AA);

        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_IN, TIME_BAR_RADIUS_IN},
                    0, 0, 360,
                    cv::Scalar{0}, -1, CV_AA);
    }
    else if (TIME_BAR_BACK_ANIMATION_END < count_)
    {
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(45) + 5 - 1,
                    0,
                    static_cast<double>(90) - 10,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(90) + 45 + 5 - 3,
                    0,
                    static_cast<double>(90) - 10,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(180) + 45 + 5 + 3,
                    0,
                    static_cast<double>(90) - 10,
                    HP_BAR_BACK, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT},
                    static_cast<double>(270) + 45 + 5 + 1,
                    0,
                    static_cast<double>(90) - 10,
                    HP_BAR_BACK, -1, CV_AA);

        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size{TIME_BAR_RADIUS_IN, TIME_BAR_RADIUS_IN},
                    0, 0, 360,
                    cv::Scalar{0}, -1, CV_AA);
    }

    if (TIME_BAR_FORE_ANIMATION_START <= count_ &&
        count_ <= TIME_BAR_FORE_ANIMATION_END)
    {
        const int count{count_ - TIME_BAR_FORE_ANIMATION_START};

        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(45) + 5 - 1, 0,
                    (90 - 10) * count / TIME_BAR_FORE_ANIMATION_LENGTH,
                    HP_BAR_FORE, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(90) + 45 + 5 - 3, 0,
                    (90 - 10) * count / TIME_BAR_FORE_ANIMATION_LENGTH,
                    HP_BAR_FORE, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(180) + 45 + 5 + 3, 0,
                    (90 - 10) * count / TIME_BAR_FORE_ANIMATION_LENGTH,
                    HP_BAR_FORE, -1, CV_AA);
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(270) + 45 + 5 + 1, 0,
                    (90 - 10) * count / TIME_BAR_FORE_ANIMATION_LENGTH,
                    HP_BAR_FORE, -1, CV_AA);

        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_IN, TIME_BAR_RADIUS_IN),
                    0, 0, 360,
                    cv::Scalar(0), -1, CV_AA);
    }
    else if (TIME_BAR_FORE_ANIMATION_END < count_)
    {
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(45) + 5 - 1,
                    0, (90 - 10) * power_.first / power_.second,
                    HP_BAR_FORE, -1, CV_AA);
        power_ = main_chara_->get_normal_move_power(normal_move_name_.at(1));
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(90) + 45 + 5 - 3,
                    0, (90 - 10) * power_.first / power_.second,
                    HP_BAR_FORE, -1, CV_AA);
        power_ = main_chara_->get_normal_move_power(normal_move_name_.at(2));
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(180) + 45 + 5 + 3,
                    0, (90 - 10) * power_.first / power_.second,
                    HP_BAR_FORE, -1, CV_AA);
        power_ = main_chara_->get_normal_move_power(normal_move_name_.at(3));
        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_OUT, TIME_BAR_RADIUS_OUT),
                    static_cast<double>(270) + 45 + 5 + 1,
                    0, (90 - 10) * power_.first / power_.second,
                    HP_BAR_FORE, -1, CV_AA);

        cv::ellipse(image_,
                    TIME_BAR_CENTER,
                    cv::Size(TIME_BAR_RADIUS_IN, TIME_BAR_RADIUS_IN),
                    0, 0, 360,
                    cv::Scalar(0), -1, CV_AA);
    }
}
}
