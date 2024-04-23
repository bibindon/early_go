#pragma once
#include "stdafx.hpp"
#include "sequence_switcher.hpp"

namespace early_go
{
class player;
class camera;
class seq_interface
{
public:
    virtual seq_type update(
        const std::shared_ptr<player>& _early,
        const std::shared_ptr<camera>& _camera) = 0;
    virtual void render(
        const std::shared_ptr<player>& _early,
        const std::shared_ptr<camera>& _camera) = 0;
    virtual ~seq_interface();
};
}
