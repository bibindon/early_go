#include "stdafx.hpp"
#include "hud2.hpp"
#include "character.hpp"
#include "text_d3dxfont.hpp"

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
void hud2::operator()(const shared_ptr<character>& _early, const shared_ptr<character>& _suo)
{
    {
        int hp = _early->get_health();
        int max_hp = _early->get_max_health();

        string hp_text = to_string(hp) + "/" + to_string(max_hp);
        early_hp_->set_param(hp_text, 800, 500, 0xA0ffffff);
        (*early_hp_)();
    }
    {
        int hp = _suo->get_health();
        int max_hp = _suo->get_max_health();

        string hp_text = to_string(hp) + "/" + to_string(max_hp);
        enemy_hp_->set_param(hp_text, 200, 200, 0xA0ffffff);
        (*enemy_hp_)();
    }
}
}
