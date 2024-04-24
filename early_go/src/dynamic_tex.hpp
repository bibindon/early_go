#pragma once
#include "stdafx.hpp"

namespace early_go
{
class dynamic_tex
{
public:
    void set_tex(const int layer_number, const std::string &texture_filename);
    void set_pos(const int layer_number, const D3DXVECTOR2& pos);
    void set_opacity(const int layer_number, const float alpha);
    void clear(const int layer_number);
    bool is_tex_animation_finished(const int layer_number);
    void set_message() {}
private:
};
} // namespace early_go 

