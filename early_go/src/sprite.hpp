#ifndef SPRITE_HDD
#define SPRITE_HDD

#include "stdafx.hpp"

namespace early_go
{
class sprite
{
public:
    sprite(const std::shared_ptr<IDirect3DDevice9>& d3d_device, std::string filename);
    // pos.x shows z-order. That should be 0.0f ~ 1.0f.
    void operator()(const D3DXVECTOR3& pos);
private:
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::shared_ptr<ID3DXSprite> sprite_;
    std::shared_ptr<IDirect3DTexture9> texture_;

    UINT width_{};
    UINT height_{};
};
} // namespace early_go 
#endif
