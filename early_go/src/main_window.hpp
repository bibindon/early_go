#ifndef BASIC_WINDOW_HPP
#define BASIC_WINDOW_HPP

#include <lua.hpp>
#include <lauxlib.h>
#include <lualib.h>

#include "stdafx.hpp"
#include "z_tex.hpp"
#include "depth_buf_shadow.hpp"
#include "shadow_mesh.hpp"
#include "shadow_mesh_anim.hpp"
#include "shadow_mesh_skin.hpp"

#include "gtest/gtest_prod.h"

namespace early_go
{

class mesh;
class animation_mesh;
class skinned_animation_mesh;
class camera;
class character;
class character2;
class abstract_mesh;
class sprite_anim;
class novel;
class sprite;
class text_d3dxfont;
class sequence_switcher;
class player;

struct mesh_info
{
    D3DXVECTOR3 pos;
    D3DXVECTOR3 rotate;
    float scale;
};

/*
 * A class that handles basic window processings.
 */
class main_window
{
public:
    /*
     * A struct for showing debug strings anytime. The reason why this struct is
     * not prototype decralation is necessary that a client of this class can look
     * the 'render_string' function.
     */
    struct render_string_object
    {
        static std::weak_ptr<ID3DXFont> weak_font_;
        static void render_string(const std::string &, const int &, const int &);
    };

    explicit main_window(const HINSTANCE &);
    ~main_window();
    int main_loop();
    std::shared_ptr<character2> get_main_character();

    // TODO remove
    shadow_mesh* shadow_mesh_;
    shadow_mesh_anim* shadow_mesh_anim_;
    shadow_mesh_skin* shadow_mesh_skin_;
    shadow_mesh_skin* shadow_mesh_skin2_;

private:
    MSG msg_;
    std::unique_ptr<IDirect3D9, custom_deleter> direct3d9_;
    D3DPRESENT_PARAMETERS d3d_present_parameters_;
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::shared_ptr<ID3DXFont> font_;

    std::shared_ptr<player> early_;
    std::shared_ptr<character2> chara2_ { };

    std::shared_ptr<camera> camera_;

    void initialize_direct3d(const HWND &);
    void debug();
    void render();

    std::shared_ptr<sprite_anim> sprite_anim_;
    std::shared_ptr<sprite> sprite_;
    std::shared_ptr<text_d3dxfont> text_d3dxfont_ { };

    std::shared_ptr<novel> novel_;
    HWND hwnd_;

    std::shared_ptr<sequence_switcher> sequence_switcher_ { };

    int ideal_time_table[60];

    std::shared_ptr<z_tex> ztex_;
    std::shared_ptr<depth_buf_shadow> depth_buf_shadow_;

    std::shared_ptr<ID3DXSprite> cpSprite;
    // Z値テクスチャ
    std::shared_ptr<IDirect3DTexture9> cpShadowMapTex;

    //----------------------------------------
    // Lua area
    //----------------------------------------

    lua_State *lua_state_;
    void init_lua();
    void init_se();
    static main_window* main_window_;
    static int glue_place_mesh(lua_State *);
    static int glue_place_mesh_anim(lua_State *);
    static int glue_place_mesh_skin(lua_State *);
    static int glue_place_shadow_mesh(lua_State *);
//    static int glue_place_shadow_mesh_anim(lua_State *);
//    static int glue_place_shadow_mesh_skin(lua_State *);
    static void get_mesh_info_from_lua(
        lua_State*, std::string*, D3DXVECTOR3*, D3DXVECTOR3*, float*);
    std::unordered_map<std::string, std::shared_ptr<abstract_mesh> > meshes_lua_;
    std::unordered_map<std::string, std::shared_ptr<shadow_mesh> > shadow_meshes_lua_;
    std::unordered_map<std::string, std::shared_ptr<shadow_mesh_anim> > shadow_anims_lua_;
    std::unordered_map<std::string, std::shared_ptr<shadow_mesh_skin> > shadow_skins_lua_;

    static std::unordered_multimap<std::string, mesh_info> mesh_info_map;

};
} // namespace early_go 
#endif
