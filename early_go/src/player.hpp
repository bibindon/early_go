#pragma once
#include "stdafx.hpp"
#include "character2.hpp"

namespace early_go
{
class stage;
class player : public character2
{
public:
    using character2::character2;

    // TODO
    void update(const std::shared_ptr<stage>&) {}
    // TODO
    // playerÉNÉâÉXÇ≈Ç‚ÇÈÇ◊Ç´Ç≈ÇÕÇ»Ç¢ÅH
    void set_dynamic_texture(
        const std::string&, const std::string&, const int, abstract_mesh::combine_type) {}
    void set_dynamic_texture_position(const std::string&, const int, const D3DXVECTOR2&) {}
    void set_dynamic_texture_opacity(const std::string&, const int&, const float&) {}
    void clear_dynamic_texture(const std::string&, const int) {}
    void set_dynamic_message(
        const std::string&,
        const int&,
        const std::string&,
        const bool& = false,
        const cv::Rect & = {
            0,
            0,
            constants::TEXTURE_PIXEL_SIZE - 1,
            constants::TEXTURE_PIXEL_SIZE - 1 },
            const DWORD & = D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff),
            const std::string & = constants::FONT_NAME,
            const int& = 40,
            const int& = 0,
            const BYTE & = SHIFTJIS_CHARSET,
            const bool& = true) {}
    bool is_tex_animation_finished(const std::string& x_filename, const int& layer_number) { return false; }

    void set_fade_in(const std::string&) {}
    void set_fade_out(const std::string&) {}
    void set_action_step(const direction) {}
    void set_action_rotate(const direction) {}
    void set_action_step_and_rotate(const direction, const direction) {}
    void set_action_attack() {}
    void cancel_action() {}
private:
};
} // namespace early_go 

