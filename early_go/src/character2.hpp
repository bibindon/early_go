// characterクラス改
// ・影が表示できる
// ・邪魔くさいのでインナークラスを排除
#pragma once
#include "stdafx.hpp"
#include "character.hpp"
#include "stage.hpp"
#include "dynamic_tex.hpp"

namespace early_go
{
class shadow_mesh_anim;
class shadow_mesh_skin;
class action;

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
    virtual void update(const std::shared_ptr<stage>& _stage);
    void render_ztex();
    void render_depth();

    void set_grid_position(vec3i pos);
    vec3i get_grid_position() { return grid_position_; }
    void set_direction(const direction direction);
    direction get_direction() { return direction_; }
    std::unordered_map<std::string, float> get_duration_map() { return duration_map_; }

    void set_default_animation(const std::string& animation_name);
    void set_animation_config(const std::string &, const bool &, const float &);
    void set_animation(const std::string& animation_name);
    void set_pos(const D3DXVECTOR3& pos);
    D3DXVECTOR3 get_pos();
    void set_rotate(const D3DXVECTOR3& rotate);
    void set_rotate_by_direction(const direction);
    void set_scale(const float scale);

    int get_health() const { return health_; }
    void set_health(const int health) { health_ = health; }
    int get_max_health() const { return max_health_; }
    void set_max_health(const int health) { max_health_ = health; }

    void set_fade_in(const std::string&) {}
    void set_fade_out(const std::string&) {}
    void set_action_step(const direction step_dir);
    void set_action_rotate(const direction);
    void set_action_step_and_rotate(const direction, const direction);
    void set_action_attack();
    void cancel_action();
    operation::behavior_state get_action_state() { return action_state_; }

    // dynamic texture関連
    std::shared_ptr<dynamic_tex> get_dynamic_tex(const std::string& x_filename);

private:
    //----------------------------------------------------------------
    // function
    //----------------------------------------------------------------
    std::string create_animation_fullname(
        const std::string& former_name, const std::string& model_fullname);

    //----------------------------------------------------------------
    // variable
    //----------------------------------------------------------------
    std::unordered_map<std::string, std::shared_ptr<shadow_mesh_anim>> mesh_map_anim_;
    std::unordered_map<std::string, std::shared_ptr<shadow_mesh_skin>> mesh_map_skin_;

    std::shared_ptr<action> current_action_;

    direction direction_ { direction::NONE };
    vec3i grid_position_ { 0, 0, 0 };

    D3DXVECTOR3 pos_ { 0.f, 0.f, 0.f };
    D3DXVECTOR3 rotate_ { 0.f, 0.f, 0.f };
    float scale_ { 1.f };

    int health_ { };
    int max_health_ { };

    operation::behavior_state action_state_{};

    std::unordered_map<std::string, float> duration_map_;
    std::shared_ptr<dynamic_tex> dynamic_tex_;
};
} // namespace early_go 

