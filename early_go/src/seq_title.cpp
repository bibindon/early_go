#include "seq_title.hpp"
#include "input.hpp"
#include "inline_macro.hpp"
#include "sprite.hpp"
#include "camera.hpp"
#include "se_player.hpp"
#include "bgm_player.hpp"

using std::make_shared;
using std::shared_ptr;

namespace early_go
{
seq_title::seq_title(const shared_ptr<IDirect3DDevice9>& d3d_device)
    : d3d_device_ { d3d_device }
{
    title01_ = make_shared<sprite>(d3d_device_, "image/title01.png");
    title02_ = make_shared<sprite>(d3d_device_, "image/title02.png");
    title03_ = make_shared<sprite>(d3d_device_, "image/title03.png");
    cursor_pos_ = D3DXVECTOR3 { 930.0f, 400.0f, 0.0f };
    se_player::get_ton()->load("sound/cursor_move.wav");
    se_player::get_ton()->load("sound/cursor_confirm.wav");
    bgm_player::get_ton()->load("sound/title.wav");
}
seq_type seq_title::update(
    const std::shared_ptr<character>& _early,
    const std::shared_ptr<character>& _suo,
    const std::shared_ptr<camera>& _camera)
{
    _camera->set_position(
        D3DXVECTOR3 { -0.7f, 1.35f, -0.2f },
        D3DXVECTOR3 { 0.0f, 1.35f, -0.2f },
        D3DX_PI / 4);
    if (input::is_down(input_type::DOWN))
    {
        ++select_;
        if (select_ >= 2)
        {
            select_ = 2;
        }
        se_player::get_ton()->play("sound/cursor_move.wav");
    }
    else if (input::is_down(input_type::UP))
    {
        --select_;
        if (select_ <= 0)
        {
            select_ = 0;
        }
        se_player::get_ton()->play("sound/cursor_move.wav");
    }
    else if (input::is_down(input_type::A))
    {
        if (select_ == 0)
        {
            se_player::get_ton()->play("sound/cursor_confirm.wav");
            return seq_type::NOVEL;
        }
        else if (select_ == 1)
        {
            se_player::get_ton()->play("sound/cursor_confirm.wav");
            return seq_type::BATTLE;
        }
        else if (select_ == 2)
        {
            return seq_type::EXIT;
        }
    }
    if (select_ == 0)
    {
        cursor_pos_ = D3DXVECTOR3 { 1030.0f, 420.0f, 0.0f };
    }
    else if (select_ == 1)
    {
        cursor_pos_ = D3DXVECTOR3 { 1080.0f, 485.0f, 0.0f };
    }
    else if (select_ == 2)
    {
        cursor_pos_ = D3DXVECTOR3 { 1130.0f, 550.0f, 0.0f };
    }

    ++counter_;
    if (counter_ >= 600)
    {
        if (bgm_playing_ == false)
        {
            bgm_playing_ = true;
            bgm_player::get_ton()->play();
        }
        counter_ = 600;
    }

    return seq_type::TITLE;
}

void seq_title::render(
    const std::shared_ptr<character>& _early,
    const std::shared_ptr<character>& _suo,
    const std::shared_ptr<camera>& _camera)
{
    title01_->render(D3DXVECTOR3 { 630.0f, 0.0f, 0.0f });
    title02_->render(D3DXVECTOR3 { 930.0f, 400.0f, 0.0f });
    title03_->render(cursor_pos_);
}

}
