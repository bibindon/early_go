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
    for (int i = 0; i < depth_buf_shadow::LAYER; ++i)
    {
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
            shared_ptr<IDirect3DTexture9> temp { temp_texture, custom_deleter { } };
            dynamic_textures_.at(i) = temp;
        }
    }
}
void dynamic_tex::set_tex(
    const int layer,
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
        dynamic_textures_.at(layer).reset( temp_texture, custom_deleter { });
    }
}
void dynamic_tex::set_pos(const int layer, const D3DXVECTOR2& pos)
{
    depth_buf_shadow_->set_dynamic_pos(layer, pos);
}
void dynamic_tex::set_opacity(const int layer, const float alpha)
{
    depth_buf_shadow_->set_dynamic_opacity(layer, alpha);
}
void dynamic_tex::clear(const int layer)
{
    vector<char> buffer { util::get_image_resource("image/blank.png") };
    LPDIRECT3DTEXTURE9 temp_texture { nullptr };
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
        dynamic_textures_.at(layer).reset( temp_texture, custom_deleter { });
    }
}
bool dynamic_tex::is_tex_animation_finished(const int layer)
{
    return false;
}
void dynamic_tex::update()
{
    for (int i = 0; i < depth_buf_shadow::LAYER; ++i)
    {
        depth_buf_shadow_->set_dynamic_tex(i, dynamic_textures_.at(i));
    }
}
}

