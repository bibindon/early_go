#pragma once

#include "stdafx.hpp"

namespace early_go
{
class character;
class camera;
class seq_interface;

enum class seq_type
{
    TITLE,
    NOVEL,
    BATTLE,
    ENDING,
    EXIT,
    UNKNOWN,
};

// タイトルやバトルシーンなどのシーンを管理するクラス。
// 
// ゲーム業界ではシーンではなくシーケンスということが多いようなのでシーンという言葉は使わなかった。
// またxxxManagerという名前を付けるのはけしからんことらしいのでxxxSwitcherとした。
class sequence_switcher
{
public:
    sequence_switcher(const std::shared_ptr<IDirect3DDevice9>& d3d_device);
    void update(
        const std::shared_ptr<character>& _early,
        const std::shared_ptr<character>& _suo,
        const std::shared_ptr<camera>& _camera);
    void render(
        const std::shared_ptr<character>& _early,
        const std::shared_ptr<character>& _suo,
        const std::shared_ptr<camera>& _camera);
private:
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::shared_ptr<seq_interface> sequence_;
    seq_type seq_type_ { seq_type::TITLE };
};
}

