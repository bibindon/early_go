#include "enemy_manager.hpp"

#include "character.hpp"
#include "camera.hpp"
#include "animation_mesh.hpp"
#include "skinned_animation_mesh.hpp"
#include "light.hpp"

using std::shared_ptr;

namespace early_go
{
enemy_manager::enemy_manager(const std::shared_ptr<IDirect3DDevice9>& d3d_device)
    : d3d_device_ { d3d_device }
{
}
void enemy_manager::add_enemy(
    const cv::Point3i& position, const direction& direction, const float& size,
    const std::shared_ptr<camera>& _camera)
{
    enemy_map_.insert(std::make_pair("suo", character { d3d_device_, position, direction, size }));

    enemy_map_.at("suo").set_camera(_camera);
    enemy_map_.at("suo").set_position(position);
    enemy_map_.at("suo").set_max_health(10);
    enemy_map_.at("suo").set_health(9);
    enemy_map_.at("suo").add_mesh<skinned_animation_mesh>(constants::SUO_BODY);
    enemy_map_.at("suo").add_mesh<skinned_animation_mesh>(constants::SUO_ARMOR);
    enemy_map_.at("suo").add_mesh<animation_mesh>(constants::SUO_SABER);

    enemy_map_.at("suo").set_default_animation("Ready");
    enemy_map_.at("suo").set_animation_config("Idle", true, 1.0f);
    enemy_map_.at("suo").set_animation_config("Ready", true, 4.0f);
    enemy_map_.at("suo").set_animation_config("Step_Front", false, 1.0f);
    enemy_map_.at("suo").set_animation_config("Step_Back", false, 1.0f);
    enemy_map_.at("suo").set_animation_config("Step_Right", false, 1.0f);
    enemy_map_.at("suo").set_animation_config("Step_Left", false, 1.0f);
    enemy_map_.at("suo").set_animation_config("Rotate_Back", false, 1.0f);
    enemy_map_.at("suo").set_animation_config("Rotate_Left", false, 1.0f);
    enemy_map_.at("suo").set_animation_config("Rotate_Right", false, 1.0f);
    enemy_map_.at("suo").set_animation_config("Attack", false, 1.0f);
    enemy_map_.at("suo").set_animation_config("Damaged", false, 1.0f);
}
character* enemy_manager::enemy(const std::string& key)
{
    return &enemy_map_.at("suo");
}
void enemy_manager::update()
{
}
void enemy_manager::render(const shared_ptr<camera>& _camera)
{
    D3DXMATRIX view_matrix { _camera->get_view_matrix() };
    D3DXMATRIX projection_matrix { _camera->get_projection_matrix() };
    if (enemy_map_.at("suo").get_health() > 0)
    {
        enemy_map_.at("suo").render(
            view_matrix,
            projection_matrix,
            light::get_ton()->normal_dir(),
            light::get_ton()->brightness());
    }
}
}
