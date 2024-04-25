#include "enemy.hpp"

using std::shared_ptr;
using std::string;

namespace early_go
{
void enemy::update()
{
    // プレイヤーの方向に移動
    D3DXVECTOR3 player_pos { player_->get_pos() };
    D3DXVECTOR3 target_dir { player_pos - position_ };
    D3DXVec3Normalize(&target_dir, &target_dir);
    position_ += (target_dir/60);
    set_position(position_);

    // プレイヤーの方向に体を回転
    float yaw = std::atan2(-target_dir.x, -target_dir.z);
    D3DXVECTOR3 rotation { yaw, 0.f, 0.f };
    set_rotation(rotation);
}
}

