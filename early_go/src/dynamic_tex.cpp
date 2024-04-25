#include "dynamic_tex.hpp"
#include "text.hpp"

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

    for (int i = 0; i < depth_buf_shadow::LAYER; ++i)
    {
        anim_finished_.at(i) = true;
        opacities_.at(i) = 0.f;
        pos_.at(i) = D3DXVECTOR2 { 0.f, 0.f };
    }

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
    pos_.at(layer) = pos;
}
void dynamic_tex::set_opacity(const int layer, const float alpha)
{
    opacities_.at(layer) = alpha;
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
bool dynamic_tex::is_tex_anim_finished(const int layer)
{
    return anim_finished_.at(layer);
}
void dynamic_tex::set_message(const std::string& message)
{
    messages_.at(5) = message;
    opacities_.at(5) = 1.f;
    anim_finished_.at(5) = false;
    anim_time_ = 0;

    writer_ = new_crt message_writer {
        d3d_device_,
        dynamic_textures_.at(5),
        message,
        true,
        {200, 620, 1400, 800},
        D3DCOLOR_ARGB(255, 255, 255, 255),
        constants::FONT_NAME,
        40,
        FW_NORMAL,
        SHIFTJIS_CHARSET,
        true
    };
}
void dynamic_tex::set_fade_in()
{
    fade_type_ = fade_type::FADE_IN;
    opacities_.at(depth_buf_shadow::FADE_LAYER) = 1.0f;
    anim_finished_.at(depth_buf_shadow::FADE_LAYER) = false;
    anim_time_ = 0;
}
void dynamic_tex::set_fade_out()
{
    fade_type_ = fade_type::FADE_OUT;
    opacities_.at(depth_buf_shadow::FADE_LAYER) = 0.0f;
    anim_finished_.at(depth_buf_shadow::FADE_LAYER) = false;
}
void dynamic_tex::update()
{
    if (fade_type_ == fade_type::FADE_IN)
    {
        ++anim_time_;
        opacities_.at(depth_buf_shadow::FADE_LAYER) -= (1.f/60/3);
        if (anim_time_ >= 60*3)
        {
            opacities_.at(depth_buf_shadow::FADE_LAYER) = 0.f;
            anim_finished_.at(depth_buf_shadow::FADE_LAYER) = true;
            fade_type_ = fade_type::NONE;
        }
    }
    else if (fade_type_ == fade_type::FADE_OUT)
    {
        ++anim_time_;
        opacities_.at(depth_buf_shadow::FADE_LAYER) += (1.f/60/3);
        if (anim_time_ >= 60*3)
        {
            opacities_.at(depth_buf_shadow::FADE_LAYER) = 1.f;
            anim_finished_.at(depth_buf_shadow::FADE_LAYER) = true;
            fade_type_ = fade_type::NONE;
        }
    }
    if (writer_ != nullptr)
    {
        bool finished = writer_->update();
        anim_finished_.at(5) = finished;
    }
    for (int i = 0; i < depth_buf_shadow::LAYER; ++i)
    {
        depth_buf_shadow_->set_dynamic_opacity(i, opacities_.at(i));
        depth_buf_shadow_->set_dynamic_pos(i, pos_.at(i));
        depth_buf_shadow_->set_dynamic_tex(i, dynamic_textures_.at(i));
    }
}
}

