#pragma once

#include "stdafx.hpp"

namespace early_go
{
class seq_interface;
// タイトルやバトルシーンなどのシーンを管理するクラス。
// 
// ゲーム業界ではシーンではなくシーケンスということが多いようなのでシーンという言葉は使わなかった。
// またxxxManagerという名前を付けるのはけしからんことらしいのでxxxSwitcherとした。
class sequence_switcher
{
public:
    sequence_switcher(const std::shared_ptr<IDirect3DDevice9>& d3d_device);
    void update();
    void render();
private:
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::shared_ptr<seq_interface> sequence_;
};
}

