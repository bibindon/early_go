#pragma once
#include "stdafx.hpp"

namespace early_go
{
class character;
class enemy;
class camera;
class player;
class enemy_manager
{
public:
    enemy_manager(
        const std::shared_ptr<IDirect3DDevice9>& d3d_device,
        const std::shared_ptr<player>& _player);

    void add_enemy(
        const std::string& key,
        const cv::Point3i & position,
        const direction & direction,
        const float & size,
        const std::shared_ptr<camera>& _camera
    );
    enemy* get_enemy(const std::string& key);
    std::unordered_map<std::string, enemy>* enemy_map();
    void update();
    void render(const std::shared_ptr<camera>& _camera);
private:
    std::unordered_map<std::string, enemy> enemy_map_;
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::shared_ptr<player> player_;
};
}
