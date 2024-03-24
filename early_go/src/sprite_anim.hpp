#ifndef SPRITE_ANIM_HDD
#define SPRITE_ANIM_HDD

#include "stdafx.hpp"

namespace early_go
{

class sprite_anim
{
public:
    sprite_anim(const std::shared_ptr<IDirect3DDevice9>& d3d_device, std::string filename);
    void operator()();

private:
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::shared_ptr<ID3DXSprite> sprite_;
    std::shared_ptr<IDirect3DTexture9> texture_;

    int width_{};
    int height_{};
    int max_frame_{};
    int frame_{};
};
} // namespace early_go 
#endif
