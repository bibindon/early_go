#pragma once

#include "stdafx.hpp"
#include "seq_interface.hpp"

#include <lua.hpp>
#include <lauxlib.h>
#include <lualib.h>

namespace early_go
{
struct portrait
{
    std::string filename_;
    std::string position_;
    bool is_flip;
};
class sprite;
class seq_novel : public seq_interface
{
public:
    seq_novel(
        const std::shared_ptr<IDirect3DDevice9>& d3d_device,
        const std::shared_ptr<character>& _early);
    virtual seq_type update(
        const std::shared_ptr<character>& _early,
        const std::shared_ptr<character>& _suo,
        const std::shared_ptr<camera>& _camera) override;
    virtual void render(
        const std::shared_ptr<character>& _early,
        const std::shared_ptr<character>& _suo,
        const std::shared_ptr<camera>& _camera) override;
private:
    void init_lua();
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
    
    static std::shared_ptr<character> early_;
    static std::deque<std::shared_ptr<portrait> > portrait_order_;

    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    bool first_update_ { true };
    lua_State *lua_state_;
    lua_State *coroutine_;

    std::shared_ptr<sprite> black_image_;
};
}
