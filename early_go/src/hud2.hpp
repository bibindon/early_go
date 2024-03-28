#ifndef HUD2_HDD
#define HUD2_HDD

#include "stdafx.hpp"

namespace early_go
{
class character;
class text_d3dxfont;
class hud2
{
public:
    hud2(const std::shared_ptr<IDirect3DDevice9>& d3d_device);
    void operator()(const std::shared_ptr<character>&, const std::shared_ptr<character>&);
private:
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::shared_ptr<text_d3dxfont> early_hp_ { };
    std::shared_ptr<text_d3dxfont> enemy_hp_ { };

    UINT width_ { 0 };
    UINT height_ { 0 };
};
} // namespace early_go 
#endif
