#pragma once
#include "stdafx.hpp"
#include "depth_buf_shadow.hpp"

namespace early_go
{
enum class fade_type
{
    NONE,
    FADE_IN,
    FADE_OUT,
};
class dynamic_tex
{
public:
    dynamic_tex(
        const std::shared_ptr<IDirect3DDevice9>& d3d_device,
        const std::shared_ptr<depth_buf_shadow>& _depth_buf_shadow);
    void set_tex(const int layer_number, const std::string &texture_filename);
    void set_pos(const int layer_number, const D3DXVECTOR2& pos);
    void set_opacity(const int layer_number, const float alpha);
    void clear(const int layer_number);
    bool is_tex_anim_finished(const int layer_number);
    void set_message() {}
    void set_fade_in();
    void set_fade_out();
    void update();
private:
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::shared_ptr<depth_buf_shadow> depth_buf_shadow_;
    std::array<std::shared_ptr<IDirect3DTexture9>, depth_buf_shadow::LAYER> dynamic_textures_ { };
    std::array<bool, depth_buf_shadow::LAYER> anim_finished_ { };
    std::array<float, depth_buf_shadow::LAYER> opacities_ { };

    fade_type fade_type_ { fade_type::NONE };
    int anim_time_ { 0 };

};
} // namespace early_go 

