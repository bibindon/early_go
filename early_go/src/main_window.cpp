#pragma comment(lib,"winmm.lib")

#include "stdafx.hpp"

#include "animation_mesh.hpp"
#include "skinned_animation_mesh.hpp"
#include "main_window.hpp"
#include "mesh.hpp"
#include "camera.hpp"
#include "character.hpp"
#include "abstract_mesh.hpp"
#include "key.hpp"
#include "operation.hpp"
#include "hud.hpp"
#include "hud2.hpp"
#include "novel.hpp"
#include "resource.h"
#include "sprite_anim.hpp"
#include "sprite.hpp"
#include "text_d3dxfont.hpp"

#include <thread>

using std::chrono::system_clock;
using std::shared_ptr;
using std::vector;
using std::string;
using std::make_shared;

namespace early_go
{
// A definition of the static member variable. 
std::weak_ptr<ID3DXFont> main_window::render_string_object::weak_font_;

// c'tor 
main_window::main_window(const HINSTANCE &hinstance)
    : direct3d9_{nullptr, custom_deleter{}},
      d3d_present_parameters_{},
      d3d_device_{},
      font_{},
      light_direction_{-1.0f, 1.0f, -1.0f},
      light_brightness_{1.0f},
      camera_{new_crt camera{{0.0f, 1.3f, -1.1639f * 3}, {0.0f, 1.3f, 0.0f}}},
      operation_{new_crt operation{camera_}},
      early_{new_crt character{d3d_device_, operation_, {0, 0, 0}, direction::FRONT, 1.0f}},
      suo_{new_crt character{d3d_device_, operation_, {0, 0, 2}, direction::BACK, 1.0f}}
{
    WNDCLASSEX wndclassex{};
    wndclassex.cbSize = sizeof(wndclassex);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc =
        [](::HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT
        {
            if (msg == WM_CLOSE)
            {
                PostQuitMessage(0);
            }
            else
            {
                return DefWindowProc(hwnd, msg, wparam, lparam);
            }
            return 0;
        };
    wndclassex.cbClsExtra = 0;
    wndclassex.cbWndExtra = 0;
    wndclassex.hInstance = hinstance;
    wndclassex.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_ICON1));
    wndclassex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndclassex.hbrBackground = static_cast<::HBRUSH>(::GetStockObject(BLACK_BRUSH));
    wndclassex.lpszMenuName = nullptr;
    wndclassex.lpszClassName = constants::APP_NAME.c_str();
    wndclassex.hIconSm = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_ICON1));

    RegisterClassEx(&wndclassex);

    RECT rc{};
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);

    HWND hwnd{CreateWindow(
        constants::APP_NAME.c_str(),
        constants::APP_NAME.c_str(),
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        rc.right / 2 - constants::WINDOW_WIDTH / 2,
        rc.bottom / 2 - constants::WINDOW_HEIGHT / 2,
        constants::WINDOW_WIDTH,
        constants::WINDOW_HEIGHT,
        nullptr,
        nullptr,
        hinstance,
        nullptr)};

    hwnd_ = hwnd;

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    initialize_direct3d(hwnd);

    init_lua();
}

void main_window::initialize_direct3d(const HWND &hwnd)
{
    // Create a Direct3D object. 
    LPDIRECT3D9 direct3d9{Direct3DCreate9(D3D_SDK_VERSION)};

    if (nullptr == direct3d9)
    {
        THROW_WITH_TRACE("Failed to create 'Direct3D'.");
    }
    direct3d9_.reset(direct3d9);

    // Create a Direct3D Device object. ~ 
    d3d_present_parameters_.BackBufferFormat = D3DFMT_UNKNOWN;
    d3d_present_parameters_.BackBufferCount = 1;
    d3d_present_parameters_.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3d_present_parameters_.Windowed = TRUE;
    d3d_present_parameters_.EnableAutoDepthStencil = TRUE;
    d3d_present_parameters_.AutoDepthStencilFormat = D3DFMT_D16;
    d3d_present_parameters_.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // for receiving 
    LPDIRECT3DDEVICE9 d3d_device9{};

    if (FAILED(direct3d9_->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hwnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
        &d3d_present_parameters_,
        &d3d_device9)))
    {
        if (FAILED(direct3d9_->CreateDevice(D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            hwnd,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
            &d3d_present_parameters_,
            &d3d_device9)))
        {
            if (FAILED(direct3d9_->CreateDevice(D3DADAPTER_DEFAULT,
                D3DDEVTYPE_REF,
                hwnd,
                D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
                &d3d_present_parameters_,
                &d3d_device9)))
            {
                if (FAILED(direct3d9_->CreateDevice(D3DADAPTER_DEFAULT,
                    D3DDEVTYPE_REF,
                    hwnd,
                    D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
                    &d3d_present_parameters_,
                    &d3d_device9)))
                {
                    THROW_WITH_TRACE("Failed to create 'Direct3D Device'.");
                }
            }
        }
    }
    // lazy initialization 
    d3d_device_.reset(d3d_device9, custom_deleter{});

    early_->set_position(cv::Point3i{0, 0, 0});
    early_->set_max_health(9999);
    early_->set_health(4000);

    early_->set_normal_move("Šî–{UŒ‚", 7, 10);
    early_->set_normal_move("“Ë‚«UŒ‚", 10, 10);
    early_->set_normal_move("“Š‚°UŒ‚", 5, 10);
    early_->set_normal_move("•¥‚¢UŒ‚", 3, 10);

    early_->add_mesh<skinned_animation_mesh>(constants::EARLY_BODY);
    early_->add_mesh<skinned_animation_mesh>(constants::EARLY_ARMOR);
    // early_->add_mesh<animation_mesh>(constants::EARLY_LANCE);
    early_->add_mesh<skinned_animation_mesh>(constants::EARLY_SABER);

    early_->add_mesh<skinned_animation_mesh>(constants::EARLY_HAIR);
    early_->add_mesh<skinned_animation_mesh>(constants::EARLY_SKIRT);
    early_->set_default_animation("Ready");
    early_->set_animation_config("Idle", true, 1.0f);
    early_->set_animation_config("Ready", true, 4.0f);
    early_->set_animation_config("Step_Front", false, 1.0f);
    early_->set_animation_config("Step_Back", false, 1.0f);
    early_->set_animation_config("Step_Right", false, 1.0f);
    early_->set_animation_config("Step_Left", false, 1.0f);
    early_->set_animation_config("Rotate_Back", false, 1.0f);
    early_->set_animation_config("Rotate_Left", false, 1.0f);
    early_->set_animation_config("Rotate_Right", false, 1.0f);
    early_->set_animation_config("Attack", false, 1.0f);
    early_->set_animation_config("Damaged", false, 1.0f);

    suo_->set_position(cv::Point3i{-1, 0, 2});
    suo_->set_max_health(10);
    suo_->set_health(9);
    suo_->add_mesh<skinned_animation_mesh>(constants::SUO_BODY);
    suo_->add_mesh<skinned_animation_mesh>(constants::SUO_ARMOR);
    suo_->add_mesh<animation_mesh>(constants::SUO_SABER);

    suo_->set_default_animation("Ready");
    suo_->set_animation_config("Idle", true, 1.0f);
    suo_->set_animation_config("Ready", true, 4.0f);
    suo_->set_animation_config("Step_Front", false, 1.0f);
    suo_->set_animation_config("Step_Back", false, 1.0f);
    suo_->set_animation_config("Step_Right", false, 1.0f);
    suo_->set_animation_config("Step_Left", false, 1.0f);
    suo_->set_animation_config("Rotate_Back", false, 1.0f);
    suo_->set_animation_config("Rotate_Left", false, 1.0f);
    suo_->set_animation_config("Rotate_Right", false, 1.0f);
    suo_->set_animation_config("Attack", false, 1.0f);
    suo_->set_animation_config("Damaged", false, 1.0f);
    // ~ Create a Direct3D Device object. 

    vector<char> font_buf = util::get_font_resource("font/rounded-mplus-2m-regular.ttf");
    DWORD font_num = 0;
    AddFontMemResourceEx(
        &font_buf.at(0),
        static_cast<DWORD>(font_buf.size()),
        nullptr,
        &font_num);

    // Create font. ~ 
    LPD3DXFONT font{};
    if (FAILED( D3DXCreateFont(
        d3d_device_.get(),
        0,
        10,
        FW_REGULAR,
        NULL,
        FALSE,
        SHIFTJIS_CHARSET,
        OUT_DEFAULT_PRECIS,
        PROOF_QUALITY,
        FIXED_PITCH | FF_MODERN,
        "MS_Gothic",
        &font)))
    {
        THROW_WITH_TRACE("Failed to create a font.");
    }
    font_.reset(font, custom_deleter { });
    render_string_object::weak_font_ = font_;
    // ~ Creates font. 

//    hud_ = make_shared<hud>(d3d_device_);
    hud2_ = make_shared<hud2>(d3d_device_);
    novel_ = make_shared<novel>();
}

main_window::~main_window()
{
    //  RemoveFontResource("chogokubosogothic_5.ttf");
}

// windows main loop 
int main_window::operator()()
{
    while (msg_.message != WM_QUIT)
    {
        if (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg_);
            DispatchMessage(&msg_);
        }
        else
        {
            HWND hwnd = GetActiveWindow();
            if (hwnd_ != hwnd)
            {
                continue;
            }
            if (key::update())
            {
                // TODO
                if (novel_->get_is_novel_part())
                {
                    (*novel_)(*this);
                }
                else
                {
                    (*operation_)(*this);
                }
            }
            // #if (defined(DEBUG) || defined(_DEBUG))
            if (!novel_->get_is_novel_part())
            {
                debug();
            }
            // #endif
            render();
        }
    }
    return static_cast<int>(msg_.wParam);
}

std::shared_ptr<character> main_window::get_main_character()
{
    return early_;
}

std::shared_ptr<character> main_window::get_enemy_character()
{
    return suo_;
}

cv::Point main_window::get_screen_coodinate(const D3DXVECTOR3 &world)
{
    const D3DXMATRIX view_matrix{camera_->get_view_matrix()};
    const D3DXMATRIX projection_matrix{camera_->get_projection_matrix()};
    static const D3DXMATRIX viewport_matrix{
        constants::WINDOW_WIDTH / 2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -constants::WINDOW_HEIGHT / 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        constants::WINDOW_WIDTH / 2.0f, constants::WINDOW_HEIGHT / 2.0f, 0.0f, 1.0f};
    D3DXMATRIX matrix{};
    D3DXMatrixTranslation(&matrix, world.x, world.y, world.z);
    matrix = matrix * view_matrix * projection_matrix * viewport_matrix;
    return cv::Point(static_cast<int>(matrix._41 / std::abs(matrix._44)),
                     static_cast<int>(matrix._42 / std::abs(matrix._44)));
}

void main_window::debug()
{
    if (key::is_down(VK_UP))
    {
        suo_->set_action_step(direction::FRONT);
    }
    if (key::is_down(VK_LEFT))
    {
        suo_->set_action_step(direction::LEFT);
    }
    if (key::is_down(VK_DOWN))
    {
        suo_->set_action_step(direction::BACK);
    }
    if (key::is_down(VK_RIGHT))
    {
        suo_->set_action_step(direction::RIGHT);
    }

    // fps
    static int frame_count = 0;
    static int fps = 0;
    static system_clock::time_point start, end;
    if (frame_count == 100)
    {
        frame_count = 0;
        end = system_clock::now();
        system_clock::rep elapsed{
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()};
        fps = 100 * 1000 / static_cast<int>(elapsed);
        start = system_clock::now();
        log_liner{} << fps;
    }

    if (frame_count == 0)
    {
        start = system_clock::now();
    }
    ++frame_count;

    static bool fps_show{true};
    if (key::is_down('F'))
    {
        fps_show = !fps_show;
        if (!fps_show)
        {
            //hud_->delete_message("fps");
        }
    }
    if (fps_show)
    {
        //hud_->delete_message("fps");
        //hud_->add_message("fps", std::to_string(fps), cv::Rect(30, 10, 64, 32));
    }

    //  if (key::is_hold('I')) {
    //    camera_->move_position({0.0f, 0.0f, 0.02f});
    ////    early_->add_mesh<animation_mesh>(constants::EARLY_LANCE);
    ////    early_->set_default_animation("Ready");
    //  }
    //  if (key::is_hold('J')) {
    //    camera_->move_position({-0.02f, 0.0f, 0.0f});
    //  }
    //  if (key::is_hold('K')) {
    //    camera_->move_position({0.0f, 0.0f, -0.02f});
    //  }
    //  if (key::is_hold('L')) {
    //    camera_->move_position({0.02f, 0.0f, 0.0f});
    //  }
    if (key::is_down('H'))
    {
        camera_->move_position({0.0f, 0.00f, 0.02f});
    }
    if (key::is_hold('N'))
    {
        camera_->move_position({0.0f, -0.02f, 0.0f});
    }

    //  if (key::is_hold('F')) {
    //    light_direction_.x += 0.2f;
    //    if (light_direction_.x >= 1.0f) {
    //      light_direction_.x = 1.0f;
    //    }
    //  }
    //  if (key::is_hold('S')) {
    //    light_direction_.x -= 0.2f;
    //    if (light_direction_.x <= -1.0f) {
    //      light_direction_.x = -1.0f;
    //    }
    //  }
    //  if (key::is_hold('E')) {
    //    light_direction_.z += 0.2f;
    //    if (light_direction_.z >= 1.0f) {
    //      light_direction_.z = 1.0f;
    //    }
    //  }
    //  if (key::is_hold('D')) {
    //    light_direction_.z -= 0.2f;
    //    if (light_direction_.z <= -1.0f) {
    //      light_direction_.z = -1.0f;
    //    }
    //  }
    //  if (key::is_hold('T')) {
    //    light_direction_.y += 0.2f;
    //    if (light_direction_.y >= 1.0f) {
    //      light_direction_.y = 1.0f;
    //    }
    //  }
    //  if (key::is_hold('G')) {
    //    light_direction_.y -= 0.2f;
    //    if (light_direction_.y <= -1.0f) {
    //      light_direction_.y = -1.0f;
    //    }
    //  }
    //  if (key::is_hold('R')) {
    //    light_direction_.x = 0.0f;
    //    light_direction_.y = 0.0f;
    //    light_direction_.z = 0.0f;
    //  }
    if (key::is_down('1'))
    {
        early_->set_animation("Idle");
        suo_->set_animation("Idle");
    }
    if (key::is_down('2'))
    {
        early_->set_animation("Ready");
        suo_->set_animation("Ready");
    }
    if (key::is_down('3'))
    {
        early_->set_animation("Step_Front");
        suo_->set_animation("Step_Front");
    }
    if (key::is_down('4'))
    {
        early_->set_animation("Step_Back");
        suo_->set_animation("Step_Back");
    }
    if (key::is_down('5'))
    {
        early_->set_animation("Step_Left");
        suo_->set_animation("Step_Left");
        meshes_lua_.at(4)->set_animation("Wolf_Idle_");
    }
    if (key::is_down('6'))
    {
        early_->set_animation("Step_Right");
        suo_->set_animation("Step_Right");
        meshes_lua_.at(4)->set_animation("Wolf_Run_Cycle_");
    }
    if (key::is_down('7'))
    {
        early_->set_animation("Rotate_Back");
        suo_->set_animation("Rotate_Back");
    }
    if (key::is_down('8'))
    {
        early_->set_animation("Rotate_Left");
        suo_->set_animation("Rotate_Left");
    }
    if (key::is_down('9'))
    {
        early_->set_animation("Rotate_Right");
        suo_->set_animation("Rotate_Right");
    }
    if (key::is_down('0'))
    {
        early_->set_animation("Attack");
        suo_->set_animation("Attack");
    }
    if (key::is_down('P'))
    {
        early_->set_animation("Damaged");
        suo_->set_animation("Damaged");
    }
    if (key::is_down('Z'))
    {
        early_->set_dynamic_texture(
            constants::EARLY_BODY, "image/back_ground.png", 0, abstract_mesh::combine_type::NORMAL);
        init_lua();
    }
    if (key::is_down('U'))
    {
        novel_->set_is_novel_part(true);
        //  early_->set_dynamic_texture(constants::EARLY_BODY,
        //      "image/settings_window.png", 0, abstract_mesh::combine_type::NORMAL);
    }
    if (key::is_down('X'))
    {
        early_->set_dynamic_texture(
            constants::EARLY_BODY, "image/test1.bmp", 0, abstract_mesh::combine_type::ADDITION);
    }
    if (key::is_down('R'))
    {
        early_->flip_dynamic_texture(constants::EARLY_BODY, 1);
    }
    if (key::is_down('T'))
    {
        early_->clear_dynamic_texture(constants::EARLY_BODY, 1);

        text_d3dxfont_ = make_shared<text_d3dxfont>(d3d_device_);
    }
    if (key::is_down('C'))
    {
        early_->set_fade_in(constants::EARLY_BODY);
        meshes_lua_.at(0)->set_fade_in();
        meshes_lua_.at(1)->set_fade_in();
        meshes_lua_.at(3)->set_fade_in();
    }
    //  if (key::is_hold('4')) {
    //    hud_->add_image("test3", "image/board2.png", cv::Point(300, 400));
    //    static float f = 0.0f;
    //    f -= 0.01f;
    //    early_->set_dynamic_texture_position(constants::EARLY_BODY, 1, {f, f} );
    //    mesh_->set_dynamic_texture_position(1, {f, f} );
    //  }
    if (key::is_down('4'))
    {
        //hud_->show_HP_info();
    }
    if (key::is_down('5'))
    {
        //    hud_->delete_image("test3");
//        hud_->remove_HP_info();
    }
    if (key::is_down('6'))
    {
        //    hud_->add_image("image/board.png");
        //    hud_->add_message("UŒ‚‚ªƒqƒbƒgB“G‚Ì‘Ì—Í‚ª30Œ¸‚è‚Ü‚µ‚½B",
        //                      cv::Rect(100, 100, 200, 100));
        /*
        hud_->add_message_in_frame(
            "test4", "test1",
            "\
1 - Introduction\n\
\n\
Lua is a powerful, efficient, lightweight, embeddable scripting language. It su\
pports procedural programming, object-oriented programming, functional programm\
ing, data-driven programming, and data description.\n\
\n\
Lua combines simple procedural syntax with powerful data description constructs\
based on associative arrays and extensible semantics. Lua is dynamically typed\
, runs by interpreting bytecode with a register-based virtual machine, and has \
automatic memory management with incremental garbage collection, making it idea\
l for configuration, scripting, and rapid prototyping.\n\
\n\
Lua is implemented as a library, written in clean C, the common subset of Stand\
ard C and C++. The Lua distribution includes a host program called lua, which u\
ses the Lua library to offer a complete, standalone Lua interpreter, for intera\
ctive or batch use. Lua is intended to be used both as a powerful, lightweight,\
embeddable scripting language for any program that needs one, and as a powerfu\
l but lightweight and efficient stand-alone language.\n\
\n\
As an extension language, Lua has no notion of a \"main\" program: it works emb\
edded in a host client, called the embedding program or simply the host. (Frequ\
ently, this host is the stand-alone lua program.) The host program can invoke f\
unctions to execute a piece of Lua code, can write and read Lua variables, and \
can register C functions to be called by Lua code. Through the use of C functio\
ns, Lua can be augmented to cope with a wide range of different domains, thus c\
reating customized programming languages sharing a syntactical framework.\n\
\n\
Lua is free software, and is provided as usual with no guarantees, as stated in\
its license. The implementation described in this manual is available at Lua's\
official web site, www.lua.org.\n\
\n\
Like any other reference manual, this document is dry in places. For a discussi\
on of the decisions behind the design of Lua, see the technical papers availabl\
e at Lua's web site. For a detailed introduction to programming in Lua, see Rob\
erto's book, Programming in Lua.\n\
");
*/
        // hud_->add_message("Enemy: 30 damage.", cv::Point(100, 300));
    }

    if (key::is_down('7'))
    {
//        hud_->add_frame( "test1", cv::Rect(250, 300, 600, 450), cv::Scalar(30, 20, 20, 150));
    }

    if (key::is_down('8'))
    {
//        hud_->delete_frame("test1");
    }

    if (key::is_down('9'))
    {
//        hud_->delete_message("test4");
    }

    if (key::is_down('V'))
    {
        early_->set_fade_out(constants::EARLY_BODY);
        meshes_lua_.at(0)->set_fade_out();
        meshes_lua_.at(1)->set_fade_out();
        meshes_lua_.at(3)->set_fade_out();
//        static float f = D3DX_PI / 2;
//        f += 0.1f;
//        early_->set_dynamic_texture_opacity(
//            constants::EARLY_BODY, 1, std::sin(f) / 2 + 0.5f);
    }
    if (key::is_down('B'))
    {
        meshes_lua_.at(0)->set_dynamic_message(
            1,
            "1234567890", false,
            {1000, 1000, 2048, 2048},
            D3DCOLOR_ARGB(255, 255, 64, 64),
            constants::FONT_NAME,
            300,
            FW_NORMAL,
            SHIFTJIS_CHARSET,
            true);
        early_->set_dynamic_message(
            constants::EARLY_BODY, 1,
            "ccccccccccccc", false,
            {210, 270, 2048, 2048},
            D3DCOLOR_ARGB(255, 255, 255, 255),
            constants::FONT_NAME,
            120,
            FW_NORMAL,
            SHIFTJIS_CHARSET,
            true);
    }
    if (key::is_down('M'))
    {
        camera_->set_to_close_up_animation();
    }
    if (key::is_down(VK_OEM_COMMA))
    {
        camera_->set_to_behind_animation();
    }
    if (key::is_down(VK_OEM_PERIOD))
    {
        early_->set_shake_texture(constants::EARLY_BODY);
    }
    if (key::is_down('G'))
    {
        sprite_anim_ = make_shared<sprite_anim>(d3d_device_, "image/sprite_anim_test1.png");
        sprite_ = make_shared<sprite>(d3d_device_, "image/sprite_test1.png");
    }
    // if (key::is_down('W')) {
    //   early_->set_dynamic_message(constants::EARLY_BODY, 1,
    //       "aaaijijjjaa\n‚ ‚ ‚ ", true, { 210, 270, 511, 511 });
    // }
}

void main_window::render()
{
    D3DXVECTOR4 light_direction{};
    light_direction.x = light_direction_.x;
    light_direction.y = light_direction_.y;
    light_direction.z = light_direction_.z;
    light_direction.w = 1.0f;
    D3DXVec4Normalize(&light_direction, &light_direction);

    (*camera_)();
    D3DXMATRIX view_matrix{camera_->get_view_matrix()};
    D3DXMATRIX projection_matrix{camera_->get_projection_matrix()};

    d3d_device_->Clear(
        0,
        nullptr,
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        D3DCOLOR_XRGB(0x40, 0x40, 0x40),
        1.0f,
        0);
    if (SUCCEEDED(d3d_device_->BeginScene()))
    {
        early_->render(view_matrix, projection_matrix, light_direction, light_brightness_);
        suo_->render(view_matrix, projection_matrix, light_direction, light_brightness_);
        for (int i = 0; i < meshes_lua_.size(); ++i)
        {
            meshes_lua_.at(i)->render(
                view_matrix, projection_matrix, light_direction, light_brightness_);
        }

        if (sprite_anim_.get() != nullptr)
        {
            (*sprite_anim_)();
        }
        if (sprite_.get() != nullptr)
        {
            D3DXVECTOR3 pos{};
            static float work = 0.0f;
            work += 1.0f;
            pos.x = work;
            pos.y = work;
            pos.z = 0.0f;
            (*sprite_)(pos);
        }
//        (*hud_)(*this);
        (*hud2_)();

        //render_string_object::render_string("aaa", 0, 0);
        if (text_d3dxfont_.get() != nullptr)
        {
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 0, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 50, D3DCOLOR_ARGB(55, 100, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 100, D3DCOLOR_ARGB(155, 200, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 150, D3DCOLOR_ARGB(255, 0, 0, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 200, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 250, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 300, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 350, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 400, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 450, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 500, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 550, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 600, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 650, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 700, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 750, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 800, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 850, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 900, D3DCOLOR_ARGB(255, 0, 100, 200));
            (*text_d3dxfont_)("Œá”y‚Í”L‚Å‚ ‚éB–¼‘O‚Í‚Ü‚¾–³‚¢B‚Ç‚±‚Å¶‚ê‚½‚©‚Æ‚ñ‚ÆŒ©“–‚ª‚Â‚©‚ÊB", 0, 950, D3DCOLOR_ARGB(255, 0, 100, 200));
        }

        d3d_device_->EndScene();
    }

    // Set fps to 60.
    static system_clock::time_point current_time;
    static system_clock::time_point previous_time = system_clock::now();

    current_time = system_clock::now();

    system_clock::rep elapsed{
        std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time - previous_time).count()};

    const int purpose_fps = 60;
    int milli_sleep_time = (1000 / purpose_fps) - static_cast<int>(elapsed);

    // Adjust
    --milli_sleep_time;

    if (milli_sleep_time >= 1)
    {
        // Make the Sleep function precise.
        timeBeginPeriod(1);
        Sleep(milli_sleep_time);
        timeEndPeriod(1);
    }

    previous_time = system_clock::now();

    d3d_device_->Present(nullptr, nullptr, nullptr, nullptr);
}

void main_window::init_lua()
{
    main_window_ = this;
    lua_state_ = luaL_newstate();
    luaopen_base(lua_state_);
    luaL_openlibs(lua_state_);

    lua_pushcfunction(lua_state_, glue_place_mesh);
    lua_setglobal(lua_state_, "place_mesh");

    lua_pushcfunction(lua_state_, glue_place_anim_mesh);
    lua_setglobal(lua_state_, "place_anim_mesh");

    lua_pushcfunction(lua_state_, glue_place_skin_mesh);
    lua_setglobal(lua_state_, "place_skin_mesh");

    vector<char> buff = util::get_lua_resource("script/init.lua");
    if (buff.size() == 0)
    {
        THROW_WITH_TRACE("Failed to load lua script.: script/init.lua");
    }
    if (luaL_loadbuffer(lua_state_, &buff.at(0), buff.size(), "script/init.lua"))
    {
        log_liner{} << "cannot load file.";
        lua_close(lua_state_);
        return;
    }
    // The lua_pcall calling is necessary before calling a function in lua file.
    if (lua_pcall(lua_state_, 0, 0, 0) != 0)
    {
        log_liner{} << "failed lua_pcall";
        lua_close(lua_state_);
        return;
    }
    int result = lua_getglobal(lua_state_, "init");
    main_window_->meshes_lua_.clear();
    if (lua_pcall(lua_state_, 0, 0, 0) != 0)
    {
        log_liner{} << "failed to call lua function";
        lua_close(lua_state_);
        return;
    }
    lua_close(lua_state_);
}

main_window* main_window::main_window_;

int main_window::glue_place_mesh(lua_State* L)
{
    string filename;
    D3DXVECTOR3 pos;
    D3DXVECTOR3 rotate;
    float scale{};
    get_mesh_info_from_lua(L, &filename, &pos, &rotate, &scale);

    main_window_->meshes_lua_.push_back(shared_ptr<abstract_mesh>(new_crt mesh{
        main_window_->d3d_device_,
        filename,
        D3DXVECTOR3{pos.x, pos.y, pos.z},
        D3DXVECTOR3{rotate.x, rotate.y, rotate.z},
        scale }));
    return 0;
}

int main_window::glue_place_anim_mesh(lua_State* L)
{
    string filename;
    D3DXVECTOR3 pos;
    D3DXVECTOR3 rotate;
    float scale{};
    get_mesh_info_from_lua(L, &filename, &pos, &rotate, &scale);

    main_window_->meshes_lua_.push_back(shared_ptr<abstract_mesh>(new_crt animation_mesh{
        main_window_->d3d_device_,
        filename,
        D3DXVECTOR3{pos.x, pos.y, pos.z},
        D3DXVECTOR3{rotate.x, rotate.y, rotate.z},
        scale }));
    return 0;
}

int main_window::glue_place_skin_mesh(lua_State* L)
{
    string filename;
    D3DXVECTOR3 pos;
    D3DXVECTOR3 rotate;
    float scale{};
    get_mesh_info_from_lua(L, &filename, &pos, &rotate, &scale);

    main_window_->meshes_lua_.push_back(shared_ptr<abstract_mesh>(new_crt skinned_animation_mesh{
        main_window_->d3d_device_,
        filename,
        D3DXVECTOR3{pos.x, pos.y, pos.z},
        D3DXVECTOR3{rotate.x, rotate.y, rotate.z},
        scale }));
    return 0;
}

void main_window::get_mesh_info_from_lua(
    lua_State* L,
    string* filename,
    D3DXVECTOR3* pos,
    D3DXVECTOR3* rotate,
    float* scale)
{
    const char *tempfilename = lua_tostring(L, -4);
    *filename = tempfilename;

    lua_rawgeti(L, -3, 1);
    pos->x = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 1);

    lua_rawgeti(L, -3, 2);
    pos->y = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 1);

    lua_rawgeti(L, -3, 3);
    pos->z = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 1);

    lua_rawgeti(L, -2, 1);
    rotate->x = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 1);

    lua_rawgeti(L, -2, 2);
    rotate->y = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 1);

    lua_rawgeti(L, -2, 3);
    rotate->z = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 1);

    *scale = static_cast<float>(lua_tonumber(L, -1));
}

void main_window::render_string_object::render_string(
    const string &message, const int &x, const int &y)
{
    std::shared_ptr<ID3DXFont> font{render_string_object::weak_font_.lock()};

    if (font)
    {
        RECT rect{x, y, 0, 0};
        // Calcurate size of character string. 
        font->DrawText(
            NULL,
            message.c_str(),
            -1,
            &rect,
            DT_CALCRECT,
            NULL);
        // Draw with its size. 
        font->DrawText(
            nullptr,
            message.c_str(),
            -1,
            &rect,
            DT_LEFT | DT_BOTTOM,
            0xff80ff80);
    }
}

} // namespace early_go 
