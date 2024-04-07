#include "light.hpp"

using std::shared_ptr;
using std::string;

namespace early_go
{
std::shared_ptr<light> light::single_ton_;
std::shared_ptr<light> light::get_ton()
{
    if (single_ton_.get() == nullptr)
    {
        single_ton_.reset(new_crt light { });
    }
    return single_ton_;
}

void light::finalize()
{
    single_ton_ = nullptr;
}

void light::set_direction(const D3DXVECTOR3& direction)
{
    direction_ = direction;
}

D3DXVECTOR3 light::direction()
{
    return direction_;
}

D3DXVECTOR4 light::normal_dir()
{
    D3DXVECTOR4 normal;
    normal.x = direction_.x;
    normal.y = direction_.y;
    normal.z = direction_.z;
    normal.w = 1.0f;
    D3DXVec4Normalize(&normal, &normal);
    return normal;
}

void light::set_brightness(const float brightness)
{
    brightness_ = brightness;
}

float light::brightness()
{
    return brightness_;
}

}

