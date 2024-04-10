
#include "seq_battle.hpp"
#include "input.hpp"
#include "inline_macro.hpp"
#include "sprite.hpp"
#include "camera.hpp"
#include "se_player.hpp"
#include "bgm_player.hpp"
#include "operation.hpp"
#include "character.hpp"
#include "stage.hpp"
#include "hud2.hpp"
#include "enemy_manager.hpp"

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
    bgm_player::get_ton()->load("sound/Cyber_City90.wav");
    bgm_player::get_ton()->play(10);

    operation_.reset(new_crt operation { _camera });
    stage_.reset(new_crt stage { });

    hud2_ = make_shared<hud2>(d3d_device_);
    enemy_manager_ = make_shared<enemy_manager>(d3d_device_);

    thread_ = std::thread {
        [&]()
        {
            // enemy_manager_->add_enemy("suo", { -1, 0, 2 }, direction::BACK, 1.0f, _camera);
            // enemy_manager_->add_enemy("suo2", { -10, 0, 2 }, direction::BACK, 1.0f, _camera);
            // enemy_manager_->add_enemy("suo3", { -1, 0, 20 }, direction::BACK, 1.0f, _camera);
            // enemy_manager_->add_enemy("suo4", { -13, 0, 12 }, direction::BACK, 1.0f, _camera);
            // enemy_manager_->add_enemy("suo5", { 1, 0, 2 }, direction::BACK, 1.0f, _camera);
            // enemy_manager_->add_enemy("suo6", { 11, 0, 5 }, direction::BACK, 1.0f, _camera);
            // enemy_manager_->add_enemy("suo7", { 31, 0, 32 }, direction::BACK, 1.0f, _camera);
            // enemy_manager_->add_enemy("suo8", { 20, 0, 0 }, direction::BACK, 1.0f, _camera);
            // enemy_manager_->add_enemy("suo9", { 10, 0, 2 }, direction::BACK, 1.0f, _camera);
         }
    };
}
seq_type seq_battle::update(
    const std::shared_ptr<character>& _early,
    const std::shared_ptr<camera>& _camera)
{
    operation_->update(_early, enemy_manager_, stage_);
    // Goto ending
    if (keyboard::is_down('R'))
    {
        return seq_type::ENDING;
    }
    return seq_type::BATTLE;
}

void seq_battle::render(
    const std::shared_ptr<character>& _early,
    const std::shared_ptr<camera>& _camera)
{
    hud2_->render(_early, enemy_manager_, _camera);
    enemy_manager_->render(_camera);
}
seq_battle::~seq_battle()
{
    thread_.join();
}
}

