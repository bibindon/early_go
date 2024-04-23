#include "seq_novel.hpp"
#include "input.hpp"
#include "inline_macro.hpp"
#include "sprite.hpp"
#include "camera.hpp"
#include "se_player.hpp"
#include "bgm_player.hpp"
#include "player.hpp"

using std::make_shared;
using std::remove_if;
using std::shared_ptr;
using std::vector;

namespace early_go
{
std::shared_ptr<player> seq_novel::early_;
std::deque<std::shared_ptr<portrait> > seq_novel::portrait_order_;

seq_novel::seq_novel(
    const shared_ptr<IDirect3DDevice9>& d3d_device,
    const shared_ptr<player>& _early)
    : d3d_device_ { d3d_device }
{
    black_image_ = make_shared<sprite>(d3d_device_, "image/black.png");
    _early->set_animation("Idle");

    early_ = _early;

    bgm_player::get_ton()->load("sound/novel.wav");
    bgm_player::get_ton()->play(30);

    init_lua();


    int i { 0 };
    int ret = lua_resume(coroutine_, 0, 0, &i);
}
seq_type seq_novel::update(
    const std::shared_ptr<player>& _early,
    const std::shared_ptr<camera>& _camera)
{
    if (first_update_)
    {
        first_update_ = false;
        _camera->set_position(
            D3DXVECTOR3 { 0.0f, 1.2998f, 0.3f },
            D3DXVECTOR3 { 0.0f, 1.2998f, -1.0f },
            D3DX_PI / 100);
        return seq_type::NOVEL;
    }
    if (input::is_down(input_type::UP) || input::is_down(input_type::LEFT) ||
        input::is_down(input_type::DOWN) || input::is_down(input_type::RIGHT) ||
        input::is_down(input_type::Y) || input::is_down(input_type::X) ||
        input::is_down(input_type::A) || input::is_down(input_type::B))
    {
        bool tex_animation_all_finished = true;
        for (int i = 0; i < 8; ++i)
        {
            if (!early_->is_tex_animation_finished(constants::EARLY_BODY, i))
            {
                tex_animation_all_finished = false;
                break;
            }
        }
        if (tex_animation_all_finished)
        {
            int i { 0 };
            int ret { lua_resume(coroutine_, 0, 0, &i) };
            if (ret == 0)
            {
                goto_battle_seq_ = true;
                bgm_player::get_ton()->stop();
                _camera->set_to_behind_animation();
            }
        }
    }
    // Reset lua.
    if (keyboard::is_down('R'))
    {
        lua_close(lua_state_);

        portrait_order_.clear();

        early_->clear_dynamic_texture(constants::EARLY_BODY, 0);
        early_->clear_dynamic_texture(constants::EARLY_BODY, 1);
        early_->clear_dynamic_texture(constants::EARLY_BODY, 2);
        early_->clear_dynamic_texture(constants::EARLY_BODY, 3);
        early_->clear_dynamic_texture(constants::EARLY_BODY, 4);
        early_->clear_dynamic_texture(constants::EARLY_BODY, 5);
        early_->clear_dynamic_texture(constants::EARLY_BODY, 6);
        early_->clear_dynamic_texture(constants::EARLY_BODY, 7);

        init_lua();
        int i { 0 };
        int ret = lua_resume(coroutine_, 0, 0, &i);
    }
    if (goto_battle_seq_ && !_camera->get_animation_running())
    {
        return seq_type::BATTLE;
    }
    return seq_type::NOVEL;
}

void seq_novel::render(
    const std::shared_ptr<player>& _early,
    const std::shared_ptr<camera>& _camera)
{
    if (first_update_)
    {
        black_image_->render(D3DXVECTOR3 { 0.0f, 0.0f, 0.0f });
    }
}
void seq_novel::init_lua()
{
    lua_state_ = luaL_newstate();
    luaopen_base(lua_state_);
    luaL_openlibs(lua_state_);
    lua_pushcfunction(lua_state_, glue_draw_background);
    lua_setglobal(lua_state_, "draw_background");
    lua_pushcfunction(lua_state_, glue_draw_portrait);
    lua_setglobal(lua_state_, "draw_portrait");
    lua_pushcfunction(lua_state_, glue_remove_portrait);
    lua_setglobal(lua_state_, "remove_portrait");
    lua_pushcfunction(lua_state_, glue_fade_in);
    lua_setglobal(lua_state_, "fade_in");
    lua_pushcfunction(lua_state_, glue_fade_out);
    lua_setglobal(lua_state_, "fade_out");
    lua_pushcfunction(lua_state_, glue_draw_message_window);
    lua_setglobal(lua_state_, "draw_message_window");
    lua_pushcfunction(lua_state_, glue_draw_text);
    lua_setglobal(lua_state_, "draw_text");

    vector<char> buff = util::get_lua_resource("script/test.lua");
    if (luaL_loadbuffer(lua_state_, &buff.at(0), buff.size(), "script/test.lua"))
    {
        log_liner{} << "cannot load file.";
        lua_close(lua_state_);
        return;
    }

    if (lua_pcall(lua_state_, 0, 0, 0) != 0)
    {
        log_liner{} << "failed lua_pcall";
        lua_close(lua_state_);
        return;
    }
    coroutine_ = lua_newthread(lua_state_);
    lua_getglobal(coroutine_, "step");
}
int seq_novel::glue_draw_background(lua_State *L)
{
    const char *filename = lua_tostring(L, -1);
    early_->set_dynamic_texture(
        constants::EARLY_BODY, filename, 0, abstract_mesh::combine_type::NORMAL);
    return 0;
}
int seq_novel::glue_draw_portrait(lua_State *L)
{
    const char *filename = lua_tostring(L, -2);
    const char *position = lua_tostring(L, -1);

    // Align portrait order.
    if (strcmp(position, "center") != 0 &&
        strcmp(position, "left") != 0 &&
        strcmp(position, "right") != 0)
    {
        return 0;
    }
    auto it = remove_if(
        portrait_order_.begin(), portrait_order_.end(),
        [&](const shared_ptr<portrait> &x)
        {
            return x->position_ == position;
        });

    if (it != portrait_order_.end())
    {
        portrait_order_.erase(it);
    }

    shared_ptr<portrait> _portrait { new_crt portrait { filename, position } };
    portrait_order_.push_front(_portrait);

    redraw_portrait();

    return 0;
}
int seq_novel::glue_remove_portrait(lua_State *L)
{
    const char *position = lua_tostring(L, -1);
    auto it = remove_if(
        portrait_order_.begin(), portrait_order_.end(),
        [&](const shared_ptr<portrait> &x)
        {
            return x->position_ == position;
        });

    if (it != portrait_order_.end())
    {
        portrait_order_.erase(it);
    }
    redraw_portrait();
    return 0;
}
int seq_novel::glue_fade_in(lua_State *)
{
    early_->set_fade_in(constants::EARLY_BODY);
    return 0;
}
int seq_novel::glue_fade_out(lua_State *)
{
    early_->set_fade_out(constants::EARLY_BODY);
    return 0;
}
int seq_novel::glue_draw_message_window(lua_State *L)
{
    early_->set_dynamic_texture(
        constants::EARLY_BODY, "image/message_window.png", 1,
        abstract_mesh::combine_type::NORMAL);
    early_->set_dynamic_texture_opacity(constants::EARLY_BODY, 2, 0.9f);
    return 0;
}
int seq_novel::glue_draw_text(lua_State *L)
{
    const char *message = lua_tostring(L, -1);
    early_->set_dynamic_message(
        constants::EARLY_BODY, 5,
        message, true,
        {200, 620, 1400, 800},
        D3DCOLOR_ARGB(255, 255, 255, 255),
        constants::FONT_NAME,
        40,
        FW_NORMAL,
        SHIFTJIS_CHARSET,
        true);
    return 0;
}
void seq_novel::redraw_portrait()
{
    // most front == (layer 4)
    // most back  == (layer 2)

    auto it { portrait_order_.cbegin() };
    for (int layer = 4; layer >= 2; --layer, ++it)
    {
        if (it == portrait_order_.cend())
        {
            early_->clear_dynamic_texture(constants::EARLY_BODY, layer);
            break;
        }
        early_->set_dynamic_texture(
            constants::EARLY_BODY, (*it)->filename_, layer, abstract_mesh::combine_type::NORMAL);

        if ((*it)->position_ == "center")
        {
            early_->set_dynamic_texture_position(
                constants::EARLY_BODY, layer, D3DXVECTOR2{-0.12f, 0.0f});
        }
        else if ((*it)->position_ == "left")
        {
            early_->set_dynamic_texture_position(
                constants::EARLY_BODY, layer, D3DXVECTOR2{0.1f, 0.0f});
        }
        else if ((*it)->position_ == "right")
        {
            early_->set_dynamic_texture_position(
                constants::EARLY_BODY, layer, D3DXVECTOR2{-0.3f, 0.0f});
        }
    }
}
}

