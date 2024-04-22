// characterクラス改
// ・影が表示できる
// ・邪魔くさいのでインナークラスを排除
#pragma once
#include "stdafx.hpp"
#include "character.hpp"

namespace early_go
{
class shadow_mesh_anim;
class shadow_mesh_skin;

class character2
{
public:
    character2(
        const std::shared_ptr<IDirect3DDevice9> &,
        const cv::Point3i &,
        const direction &,
        const float);
    ~character2();
    void add_shadow_mesh_anim(const std::shared_ptr<shadow_mesh_anim>& mesh);
    void add_shadow_mesh_skin(const std::shared_ptr<shadow_mesh_skin>& mesh);
    void update();
    void render_ztex();
    void render_depth();

    void set_default_animation(const std::string& animation_name);
    void set_pos(const D3DXVECTOR3& pos);
    void set_rotate(const D3DXVECTOR3& rotate);
    void set_scale(const float scale);
private:
    //----------------------------------------------------------------
    // function
    //----------------------------------------------------------------
    std::string create_animation_fullname(
        const std::string& former_name, const std::string& model_fullname);

    D3DXVECTOR3 pos_ { 0.f, 0.f, 0.f };
    D3DXVECTOR3 rotate_ { 0.f, 0.f, 0.f };
    float scale_ { 1.f };

    //----------------------------------------------------------------
    // variable
    //----------------------------------------------------------------
    std::unordered_map<std::string, std::shared_ptr<shadow_mesh_anim>> mesh_map_anim_;
    std::unordered_map<std::string, std::shared_ptr<shadow_mesh_skin>> mesh_map_skin_;
};
} // namespace early_go 

