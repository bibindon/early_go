#ifndef HUD2_HDD
#define HUD2_HDD

#include "stdafx.hpp"

namespace early_go
{
class hud2
{
public:
    hud2(const std::shared_ptr<IDirect3DDevice9>& d3d_device);
    void operator()();
private:
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::shared_ptr<ID3DXSprite> sprite_;
    std::shared_ptr<IDirect3DTexture9> texture_;

    UINT width_{};
    UINT height_{};
};
} // namespace early_go 
#endif
