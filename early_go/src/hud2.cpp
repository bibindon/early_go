#include "stdafx.hpp"
#include "hud2.hpp"
#include "player.hpp"
#include "text_d3dxfont.hpp"
#include "camera.hpp"
#include "enemy_manager.hpp"

using std::shared_ptr;
using std::string;
using std::vector;
using std::to_string;

namespace early_go
{
hud2::hud2(const shared_ptr<IDirect3DDevice9>& d3d_device)
    : d3d_device_ { d3d_device }
{
    early_hp_.reset(new_crt text_d3dxfont { d3d_device, 30 });
    enemy_hp_.reset(new_crt text_d3dxfont { d3d_device, 30 });
}
void hud2::render(
    const shared_ptr<player>& _early,
    const shared_ptr<enemy_manager>& _enemy_manager,
    const shared_ptr<camera>& _camera)
{
    // Show early HP
    {
        int hp = _early->get_health();
        int max_hp = _early->get_max_health();

        string hp_text = to_string(hp) + "/" + to_string(max_hp);

        D3DXVECTOR3 position { _early->get_pos() };
        position.y += 1.0f;
        const POINT coodinate { _camera->get_screen_coordinate(position) };

        early_hp_->set_param(hp_text, coodinate.x, coodinate.y, 0xA0A0ffff);
        early_hp_->render();
    }
    // Show enemy HP
    {
        auto enemy_map = _enemy_manager->enemy_map();
        for (auto& enemy : *enemy_map)
        {
            int hp = enemy.second.get_health();
            int max_hp = enemy.second.get_max_health();

            string hp_text = to_string(hp) + "/" + to_string(max_hp);

            D3DXVECTOR3 position { enemy.second.get_position() };
            position.y += 1.0f;
            const POINT coodinate { _camera->get_screen_coordinate(position) };
            if (coodinate.x >= 0)
            {
                enemy_hp_->set_param(hp_text, coodinate.x, coodinate.y, 0xA0ffA0A0);
                enemy_hp_->render();
            }
        }
    }
}
}
