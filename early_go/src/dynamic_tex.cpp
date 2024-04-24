#include "dynamic_tex.hpp"

using std::shared_ptr;
using std::string;
using std::vector;

namespace early_go
{
dynamic_tex::dynamic_tex(
    const std::shared_ptr<IDirect3DDevice9>& d3d_device,
    const std::shared_ptr<depth_buf_shadow>& _depth_buf_shadow)
{
    d3d_device_ = d3d_device;
    depth_buf_shadow_ = _depth_buf_shadow;
    vector<char> buffer { util::get_image_resource("image/blank.png") };
    LPDIRECT3DTEXTURE9 temp_texture{};
    if (FAILED ( D3DXCreateTextureFromFileInMemory(
            d3d_device_.get(),
            &buffer[0],
            static_cast<UINT>(buffer.size()),
            &temp_texture)))
    {
        THROW_WITH_TRACE("texture file is not found.");
    }
    else
    {
        dynamic_textures_.reset( temp_texture, custom_deleter { });
    }
}
void dynamic_tex::set_tex(
    const int layer_number,
    const std::string& texture_filename)
{
    vector<char> buffer { util::get_image_resource(texture_filename) };
    LPDIRECT3DTEXTURE9 temp_texture{};
    if (FAILED ( D3DXCreateTextureFromFileInMemory(
            d3d_device_.get(),
            &buffer[0],
            static_cast<UINT>(buffer.size()),
            &temp_texture)))
    {
        THROW_WITH_TRACE("texture file is not found.");
    }
    else
    {
        dynamic_textures_.reset( temp_texture, custom_deleter { });
    }
}
void dynamic_tex::set_pos(const int layer_number, const D3DXVECTOR2& pos)
{
}
void dynamic_tex::set_opacity(const int layer_number, const float alpha)
{
}
void dynamic_tex::clear(const int layer_number)
{
    vector<char> buffer { util::get_image_resource("image/blank.png") };
    LPDIRECT3DTEXTURE9 temp_texture{};
    if (FAILED ( D3DXCreateTextureFromFileInMemory(
            d3d_device_.get(),
            &buffer[0],
            static_cast<UINT>(buffer.size()),
            &temp_texture)))
    {
        THROW_WITH_TRACE("texture file is not found.");
    }
    else
    {
        dynamic_textures_.reset( temp_texture, custom_deleter { });
    }
}
bool dynamic_tex::is_tex_animation_finished(const int layer_number)
{
    return false;
}
void dynamic_tex::update()
{
    depth_buf_shadow_->set_dynamic_tex(dynamic_textures_);
}
}

