#pragma once
#include "stdafx.hpp"

namespace early_go
{
class light
{
public:
    static std::shared_ptr<light> single_ton_;
    static std::shared_ptr<light> get_ton();
    static void finalize(); // For memory leak check.

    void set_direction(const D3DXVECTOR3 & direction);
    D3DXVECTOR3 direction();
    D3DXVECTOR4 normal_dir();
    void set_brightness(const float brightness);
    float brightness();
private:
    // hide ctor
    light() { }
    light(const light&) { }
    void operator=(const light&) { }

    D3DXVECTOR3 direction_ { 0.0f, 0.0f, 0.0f };
    float brightness_ { 0.0f };
};
} // namespace early_go 

