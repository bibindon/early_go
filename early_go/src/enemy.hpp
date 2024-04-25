#pragma once
#include "stdafx.hpp"
#include "character.hpp"
#include "player.hpp"

namespace early_go
{
class stage;
class enemy : public character
{
public:
    using character::character;
    void update();
    void set_player(const std::shared_ptr<player>& _player) { player_ = _player; }
private:
    std::shared_ptr<player> player_;
};
}

