#include "novel.hpp"
#include "character.hpp"
#include "main_window.hpp"

using std::string;
using std::vector;
using std::remove_if;
using std::shared_ptr;

namespace early_go
{
// i.e. {"early", "center", true}
//      {"shiho", "left", false}
//      {"suo", "right", true}
struct novel::portrait
{
    string filename_;
    string position_;
    bool is_flip;
};
main_window *novel::window_ = nullptr;

std::deque<shared_ptr<novel::portrait> > novel::portrait_order_;

int novel::glue_draw_background(lua_State *L)
{
    const char *filename = lua_tostring(L, -1);
    window_->get_main_character()->set_dynamic_texture(
        constants::EARLY_BODY, filename, 0, abstract_mesh::combine_type::NORMAL);
    return 0;
}
int novel::glue_draw_portrait(lua_State *L)
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

    shared_ptr<portrait> _portrait{ new_crt portrait{filename, position, false } };
    portrait_order_.push_front(_portrait);

    redraw_portrait();

    return 0;
}
int novel::glue_draw_portrait_flip(lua_State *L)
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

    shared_ptr<portrait> _portrait{ new_crt portrait{filename, position, true } };
    portrait_order_.push_front(_portrait);

    redraw_portrait();

    return 0;
}
int novel::glue_remove_portrait(lua_State *L)
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
int novel::glue_fade_in(lua_State *)
{
    window_->get_main_character()->set_fade_in(constants::EARLY_BODY);
    return 0;
}
int novel::glue_fade_out(lua_State *)
{
    window_->get_main_character()->set_fade_out(constants::EARLY_BODY);
    return 0;
}
int novel::glue_draw_message_window(lua_State *L)
{
    window_->get_main_character()->set_dynamic_texture(
        constants::EARLY_BODY, "image/message_window.png", 1,
        abstract_mesh::combine_type::NORMAL);
    window_->get_main_character()->set_dynamic_texture_opacity(
        constants::EARLY_BODY, 2, 0.9f);
    return 0;
}
int novel::glue_draw_text(lua_State *L)
{
    const char *message = lua_tostring(L, -1);
    window_->get_main_character()->set_dynamic_message(
        constants::EARLY_BODY, 5,
        message, true,
        {200, 620, 1400, 800},
        D3DCOLOR_ARGB(255, 255, 255, 255),
        "ŸàƒSƒVƒbƒN",
        32,
        FW_NORMAL,
        SHIFTJIS_CHARSET,
        true);
    return 0;
}
void novel::redraw_portrait()
{
    // most front == (layer 4)
    // most back  == (layer 2)

    auto it{portrait_order_.cbegin()};
    for (int layer = 4; layer >= 2; --layer, ++it)
    {
        if (it == portrait_order_.cend())
        {
            window_->get_main_character()->clear_dynamic_texture(constants::EARLY_BODY, layer);
            break;
        }
        window_->get_main_character()->set_dynamic_texture(
            constants::EARLY_BODY, (*it)->filename_, layer, abstract_mesh::combine_type::NORMAL);

        if ((*it)->position_ == "center")
        {
            window_->get_main_character()->set_dynamic_texture_position(
                constants::EARLY_BODY, layer, D3DXVECTOR2{-0.12f, 0.0f});
        }
        else if ((*it)->position_ == "left")
        {
            window_->get_main_character()->set_dynamic_texture_position(
                constants::EARLY_BODY, layer, D3DXVECTOR2{0.1f, 0.0f});
        }
        else if ((*it)->position_ == "right")
        {
            window_->get_main_character()->set_dynamic_texture_position(
                constants::EARLY_BODY, layer, D3DXVECTOR2{-0.3f, 0.0f});
        }
        if ((*it)->is_flip)
        {
            window_->get_main_character()->flip_dynamic_texture(constants::EARLY_BODY, layer);
        }
    }
}
lua_State *co;
void novel::operator()(main_window &window)
{
    window_ = &window;

    if (key::is_down('J'))
    {

        lua_state_ = luaL_newstate();
        luaopen_base(lua_state_);
        luaL_openlibs(lua_state_);
        lua_pushcfunction(lua_state_, glue_draw_background);
        lua_setglobal(lua_state_, "draw_background");
        lua_pushcfunction(lua_state_, glue_draw_portrait);
        lua_setglobal(lua_state_, "draw_portrait");
        lua_pushcfunction(lua_state_, glue_draw_portrait_flip);
        lua_setglobal(lua_state_, "draw_portrait_flip");
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

        //if (luaL_loadfile(lua_state_, "test.lua"))
        vector<char> buff = util::get_lua_resource("script/test.lua");
        if (luaL_loadbuffer(lua_state_, &buff.at(0), buff.size(), "script/test.lua"))
        {
            log_liner{} << "cannot load file.";
            lua_close(lua_state_);
            return;
        }

        if (lua_pcall(lua_state_, 0, 0, 0) != 0)
        {
            log_liner{} << "hoge";
            lua_close(lua_state_);
            return;
        }
        co = lua_newthread(lua_state_);
        lua_getglobal(co, "step");

        int i;
        int ret = lua_resume(co, 0, 0, &i);

        // lua_getglobal(L, "num");
        // double num = lua_tonumber(L, -1);
    }
    if (key::is_down('K'))
    {
        bool tex_animation_all_finished = true;
        for (int i = 0; i < 8; ++i)
        {
            if (!window_->get_main_character()->is_tex_animation_finished(constants::EARLY_BODY, i))
            {
                tex_animation_all_finished = false;
                break;
            }
        }
        if (tex_animation_all_finished)
        {
            int i;
            int ret = lua_resume(co, 0, 0, &i);
            log_liner{} << ret;
        }
    }

    if (key::is_down(' ') || key::is_down('\r'))
    {
        // TODO next page
        int i;
        int ret = lua_resume(co, nullptr, 0, &i);
    }
    else if (key::is_down(27 /*Esc key*/))
    {
        // TODO menu screen
        window_->get_main_character()->set_dynamic_texture(
            constants::EARLY_BODY, "image/settings_window.png",
            6, abstract_mesh::combine_type::NORMAL);
        window_->get_main_character()->set_dynamic_texture_opacity(
            constants::EARLY_BODY, 6, 0.5f);
        window_->get_main_character()->set_dynamic_texture_position(
            constants::EARLY_BODY, 6, {-0.25f, 0.0f});
    }
    else if (key::is_down('I'))
    {
        // For Debug
        // TODO enable to restart lua script
        lua_close(lua_state_);
        portrait_order_.clear();
        for (int i = 0; i < 8; ++i)
        {
            window_->get_main_character()->clear_dynamic_texture(constants::EARLY_BODY, i);
        }
        is_novel_part_ = false;
    }
}
}
