#include "stdafx.hpp"
#include "sprite_anim.hpp"

using std::shared_ptr;
using std::string;
using std::vector;

namespace early_go
{
sprite_anim::sprite_anim(const shared_ptr<IDirect3DDevice9>& d3d_device, string filename)
    : d3d_device_{d3d_device}
{
    // sprite setup
    LPD3DXSPRITE temp_sprite = nullptr;
    if (FAILED(D3DXCreateSprite(d3d_device_.get(), &temp_sprite)))
    {
        THROW_WITH_TRACE("Failed to create a sprite.");
    }
    sprite_.reset(temp_sprite, custom_deleter());

    // texture setup
    vector<char> buffer = util::get_image_resource(filename);
    LPDIRECT3DTEXTURE9 temp_texture = nullptr;
    if (FAILED(D3DXCreateTextureFromFileInMemory(
        d3d_device_.get(),
        &buffer[0],
        static_cast<UINT>(buffer.size()),
        &temp_texture)))
    {
        THROW_WITH_TRACE("Failed to create a texture.");
    }

    D3DSURFACE_DESC desc{};
    if (FAILED(temp_texture->GetLevelDesc(0, &desc)))
    {
        THROW_WITH_TRACE("Failed to create a texture.");
    }

    texture_.reset(temp_texture, custom_deleter());
}
void sprite_anim::operator()()
{
    static int frame = 0;
    frame++;
    int left = (frame/10)%10;

    sprite_->Begin(D3DXSPRITE_ALPHABLEND);
    RECT rect = {left*64, 0, (left*64)+64, 64};
    D3DXVECTOR3 center(0, 0, 0);
    D3DXVECTOR3 position(0, 0, 0);
    sprite_->Draw(
        texture_.get(),
        &rect,
        &center,
        &position,
        D3DCOLOR_ARGB(255, 255, 255, 255));
    sprite_->End();
}
}
