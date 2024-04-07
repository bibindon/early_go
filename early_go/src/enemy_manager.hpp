#pragma once
#include "stdafx.hpp"

namespace early_go
{
class character;
class camera;
class enemy_manager
{
public:
    enemy_manager(const std::shared_ptr<IDirect3DDevice9>& d3d_device);
    void add_enemy(
        const std::string& key,
        const cv::Point3i & position,
        const direction & direction,
        const float & size,
        const std::shared_ptr<camera>& _camera
    );
    character* enemy(const std::string& key);
    std::unordered_map<std::string, character>* enemy_map();
    void update();
    void render(const std::shared_ptr<camera>& _camera);
private:
    std::unordered_map<std::string, character> enemy_map_;
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
};
}
