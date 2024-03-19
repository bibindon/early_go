#ifndef NOVEL_HPP
#define NOVEL_HPP

#include <lua.hpp>
#include <lauxlib.h>
#include <lualib.h>

#include "stdafx.hpp"
#include "key.hpp"
#include "main_window.hpp"

namespace early_go
{
class main_window;
struct portrait;

class novel
{
public:
    bool get_is_novel_part() const { return is_novel_part_; }
    void set_is_novel_part(const bool val) { is_novel_part_ = val; }
    void operator()(main_window &);
    lua_State *lua_state_;

private:
    static int glue_draw_background(lua_State *);
    static int glue_draw_portrait(lua_State *);
    static int glue_draw_portrait_flip(lua_State *);
    static int glue_remove_portrait(lua_State *);
    static int glue_fade_in(lua_State *);
    static int glue_fade_out(lua_State *);
    static int glue_draw_message_window(lua_State *);
    static int glue_draw_text(lua_State *);
    //  static int glue_shake(lua_State*);
    static void redraw_portrait();
    bool is_novel_part_;
    static main_window *window_;
    static std::deque<portrait> portrait_order_;
};
/// <summary>
/// i.e. {"early", "center", true}
///      {"shiho", "left", false}
///      {"suo", "right", true}
/// </summary>
struct portrait
{
    std::string filename_;
    std::string position_;
    bool is_flip;
};
} // namespace early_go 
#endif
