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
    sequence_switcher();
    void operator()();
private:
    std::shared_ptr<seq_interface> sequence_;
};
}

