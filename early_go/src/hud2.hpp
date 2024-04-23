#ifndef HUD2_HDD
#define HUD2_HDD

#include "stdafx.hpp"

namespace early_go
{
class player;
class text_d3dxfont;
class camera;
class enemy_manager;
class hud2
{
public:
    hud2(const std::shared_ptr<IDirect3DDevice9>& d3d_device);
    void render(
        const std::shared_ptr<player>& _early,
        const std::shared_ptr<enemy_manager>& _enemy_manager,
        const std::shared_ptr<camera>& _camera);
private:
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::shared_ptr<text_d3dxfont> early_hp_ { };
    std::shared_ptr<text_d3dxfont> enemy_hp_ { };

    UINT width_ { 0 };
    UINT height_ { 0 };
};
} // namespace early_go 
#endif
