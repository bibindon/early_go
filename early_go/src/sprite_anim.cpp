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

    // Read ini file.
    // ini file is a text like a following.
    //
    // width, 64
    // height, 64
    // max_frame, 10

    string inifilename = filename;
    size_t index = inifilename.find_last_of('.');
    inifilename.erase(index);
    inifilename += ".ini";
    vector<char> inibuffer = util::get_image_resource(inifilename);
    string inifile(inibuffer.begin(), inibuffer.end());
    
    size_t seek_begin = inifile.find(",");
    size_t seek_end = inifile.find("\r\n");
    string work = string(
        inifile.begin() + seek_begin + 1,
        inifile.begin() + seek_end);
    width_ = std::stoi(work);

    seek_begin = inifile.find(",", seek_end + 2);
    seek_end = inifile.find("\r\n", seek_begin);
    work = string(
        inifile.begin() + seek_begin + 1,
        inifile.begin() + seek_end);
    height_ = std::stoi(work);

    seek_begin = inifile.find(",", seek_end + 2);
    seek_end = inifile.find("\r\n", seek_begin);
    work = string(
        inifile.begin() + seek_begin + 1,
        inifile.begin() + seek_end);
    max_frame_ = std::stoi(work);
}
void sprite_anim::operator()()
{
    frame_++;
    if (frame_ >= max_frame_)
    {
        frame_ = max_frame_-1;
    }

    sprite_->Begin(D3DXSPRITE_ALPHABLEND);
    RECT rect = {
        frame_ * width_,
        0,
        (frame_ * width_) + height_,
        height_};
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
