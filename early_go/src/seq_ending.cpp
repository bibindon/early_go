
#include "seq_ending.hpp"
#include "input.hpp"
#include "inline_macro.hpp"
#include "sprite.hpp"
#include "camera.hpp"
#include "se_player.hpp"
#include "bgm_player.hpp"
#include "operation.hpp"

using std::make_shared;
using std::shared_ptr;

namespace early_go
{
seq_ending::seq_ending(
    const shared_ptr<IDirect3DDevice9>& d3d_device, const shared_ptr<camera>& _camera)
    : d3d_device_ { d3d_device }
{
    background_ = make_shared<sprite>(d3d_device_, "image/ending01.png");
    confirmation_ = make_shared<sprite>(d3d_device_, "image/ending02.png");

    bgm_player::get_ton()->load("sound/ending.wav");

}
seq_type seq_ending::update(
    const std::shared_ptr<character>& _early,
    const std::shared_ptr<camera>& _camera)
{

    ++counter_;
    if (counter_ <= 62 * 5)
    {
        if (!bgm_played_)
        {
            bgm_played_ = true;
            bgm_player::get_ton()->play(30);
        }
    }
    if (counter_ <= 62 * 10)
    {
        background_pos_.x += -0.3f;
    }
    else if (counter_ <= 62 * 20)
    {
        // do nothing
    }
    else if (counter_ <= 62 * 22)
    {
        confirmation_shown_ = true;
    }
    else if (62 * 22 < counter_)
    {
        if (input::is_down(input_type::A))
        {
            return seq_type::EXIT;
        }
    }
    return seq_type::ENDING;
}

void seq_ending::render(
    const std::shared_ptr<character>& _early,
    const std::shared_ptr<camera>& _camera)
{
    background_->render(background_pos_);
    if (confirmation_shown_)
    {
        confirmation_->render(D3DXVECTOR3 { 0.0f, 0.0f, 0.0f });
    }
}
}

