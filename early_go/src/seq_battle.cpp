
#include "seq_battle.hpp"
#include "input.hpp"
#include "inline_macro.hpp"
#include "sprite.hpp"
#include "camera.hpp"
#include "se_player.hpp"
#include "bgm_player.hpp"
#include "operation.hpp"
#include "character.hpp"

using std::make_shared;
using std::shared_ptr;

namespace early_go
{
seq_battle::seq_battle(
    const shared_ptr<IDirect3DDevice9>& d3d_device, const shared_ptr<camera>& _camera)
    : d3d_device_ { d3d_device }
{
    se_player::get_ton()->load("sound/damage01.wav");
    se_player::get_ton()->load("sound/attack01.wav");
    bgm_player::get_ton()->load("sound/letsgo.wav");
    bgm_player::get_ton()->play(10);

    operation_.reset(new_crt operation { _camera });
}
seq_type seq_battle::update(
    const std::shared_ptr<character>& _early,
    const std::shared_ptr<character>& _suo,
    const std::shared_ptr<camera>& _camera)
{
    _early->update(operation_.get());
    operation_->update(_early, _suo);
    // Goto ending
    if (keyboard::is_down('R'))
    {
        return seq_type::ENDING;
    }
    return seq_type::BATTLE;
}

void seq_battle::render(
    const std::shared_ptr<character>& _early,
    const std::shared_ptr<character>& _suo,
    const std::shared_ptr<camera>& _camera)
{
}
}

