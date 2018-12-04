#include "stdafx.hpp"

#include "animation_mesh.hpp"
#include "skinned_animation_mesh.hpp"
#include "basic_window.hpp"
#include "mesh.hpp"
#include "camera.hpp"
#include "character.hpp"
#include "base_mesh.hpp"

namespace early_go {
/* A definition of the static member variable. */
std::weak_ptr<::ID3DXFont> basic_window::render_string_object::weak_font_;

/* c'tor */
basic_window::basic_window(const ::HINSTANCE& hinstance)
    : direct3d9_{nullptr, custom_deleter{}},
      d3d_present_parameters_{},
      d3d_device_{},
      font_{},
      animation_mesh_{},
      skinned_animation_mesh_{},
      skinned_animation_mesh2_{},
      mesh_{},
      mesh2_{},
      early_{new_crt character{d3d_device_,
                               {0.0f, 0.0f, 0.0f},
                               {D3DX_PI, 0.0f, 0.0f},
                               1.0f}},
      suo_{new_crt character{d3d_device_,
                             {0.0f, 0.0f, 0.0f},
                             {D3DX_PI, 0.0f, 0.0f},
                             1.0f}},
      light_direction_{-1.0f, 0.0f, 0.0f},
      light_brightness_{1.0f},
      camera_{new_crt camera{{0.0f, 1.3f, -1.1639f*3}, {0.0f, 1.3f, 0.0f}}}
{
  ::WNDCLASSEX wndclassex{};
  wndclassex.cbSize        = sizeof(wndclassex);
  wndclassex.style         = CS_HREDRAW | CS_VREDRAW;
  wndclassex.lpfnWndProc   =
    [](::HWND hwnd, ::UINT msg, ::WPARAM wparam, ::LPARAM lparam) -> ::LRESULT {
      if (msg == WM_CLOSE) {
        ::PostQuitMessage(0);
      } else {
        return ::DefWindowProc(hwnd, msg, wparam, lparam);
      }
      return 0;
    };
  wndclassex.cbClsExtra    = 0;
  wndclassex.cbWndExtra    = 0;
  wndclassex.hInstance     = hinstance;
  wndclassex.hIcon         = ::LoadIcon(nullptr, IDI_APPLICATION);
  wndclassex.hCursor       = ::LoadCursor(nullptr, IDC_ARROW);
  wndclassex.hbrBackground =
      static_cast<::HBRUSH>(::GetStockObject(BLACK_BRUSH));
  wndclassex.lpszMenuName  = nullptr;
  wndclassex.lpszClassName = constants::APP_NAME.c_str();
  wndclassex.hIconSm       = ::LoadIcon(nullptr, IDI_APPLICATION);

  ::RegisterClassEx(&wndclassex);

  ::RECT rc{};
  ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);

  ::HWND hwnd{::CreateWindow(constants::APP_NAME.c_str(),
                             constants::APP_NAME.c_str(),
                             WS_OVERLAPPEDWINDOW,
                             rc.right/2 - constants::WINDOW_WIDTH/2,
                             rc.bottom/2 - constants::WINDOW_HEIGHT/2,
                             constants::WINDOW_WIDTH,
                             constants::WINDOW_HEIGHT,
                             nullptr,
                             nullptr,
                             hinstance,
                             nullptr)};

  ::ShowWindow(hwnd, SW_SHOW);
  ::UpdateWindow(hwnd);

  initialize_direct3d(hwnd);
}

void basic_window::initialize_direct3d(const ::HWND& hwnd)
{
  /* Create a Direct3D object. */
  ::LPDIRECT3D9 direct3d9{::Direct3DCreate9(D3D_SDK_VERSION)};

  if (nullptr == direct3d9) {
    THROW_WITH_TRACE("Failed to create 'Direct3D'.");
  }
  direct3d9_.reset(direct3d9);

  /* Create a Direct3D Device object. ~ */
  d3d_present_parameters_.BackBufferFormat       = ::D3DFMT_UNKNOWN;
  d3d_present_parameters_.BackBufferCount        = 1;
  d3d_present_parameters_.SwapEffect             = ::D3DSWAPEFFECT_DISCARD;
  d3d_present_parameters_.Windowed               = TRUE;
  d3d_present_parameters_.EnableAutoDepthStencil = TRUE;
  d3d_present_parameters_.AutoDepthStencilFormat = ::D3DFMT_D16;

  /* for receiving */
  ::LPDIRECT3DDEVICE9 d3d_device9{};

  if (FAILED(direct3d9_->CreateDevice(D3DADAPTER_DEFAULT,
      ::D3DDEVTYPE_HAL,
      hwnd,
      D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
      &d3d_present_parameters_,
      &d3d_device9))) {
    if (FAILED(direct3d9_->CreateDevice(D3DADAPTER_DEFAULT,
        ::D3DDEVTYPE_HAL,
        hwnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
        &d3d_present_parameters_,
        &d3d_device9))) {
      if (FAILED(direct3d9_->CreateDevice(D3DADAPTER_DEFAULT,
          ::D3DDEVTYPE_REF,
          hwnd,
          D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
          &d3d_present_parameters_,
          &d3d_device9))) {
        if (FAILED(direct3d9_->CreateDevice(D3DADAPTER_DEFAULT,
            ::D3DDEVTYPE_REF,
            hwnd,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
            &d3d_present_parameters_,
            &d3d_device9))) {
          THROW_WITH_TRACE("Failed to create 'Direct3D Device'.");
        }
      }
    }
  }
  /* lazy initialization */
  d3d_device_.reset(d3d_device9, custom_deleter{});
  animation_mesh_.reset(new_crt animation_mesh{
      d3d_device_,
      constants::ANIMATION_MESH_FILE_NAME,
      ::D3DXVECTOR3{0.0f, 0.0f, 0.0f},
      ::D3DXVECTOR3{0.0f, 0.0f, 0.0f},
      1.0f});
  skinned_animation_mesh_.reset(new_crt skinned_animation_mesh{
      d3d_device_,
      constants::SKINNED_ANIMATION_MESH_FILE_NAME,
      ::D3DXVECTOR3{1.0f, 0.0f, 0.0f},
      ::D3DXVECTOR3{0.0f, 0.0f, 0.0f},
      1.0f});
  skinned_animation_mesh2_.reset(new_crt skinned_animation_mesh{
      d3d_device_,
      constants::SKINNED_ANIMATION_MESH_FILE_NAME2,
      ::D3DXVECTOR3{-1.0f, 0.0f, 0.0f},
      ::D3DXVECTOR3{0.0f, 0.0f, 0.0f},
      0.003f});
  mesh_.reset(new_crt mesh{
      d3d_device_,
      constants::MESH_FILE_NAME,
      ::D3DXVECTOR3{-0.5f, 0.0f, -1.0f},
      ::D3DXVECTOR3{0.0f, 0.0f, 0.0f},
      1.0f});
  mesh2_.reset(new_crt mesh{
      d3d_device_,
      constants::MESH_FILE_NAME2,
      ::D3DXVECTOR3{ 0.5f, 0.0f, 2.0f},
      ::D3DXVECTOR3{0.0f, 0.0f, 0.0f},
      1.0f});

  early_->set_position({0.0f, 0.0f, -1.0f});
  early_->add_mesh<skinned_animation_mesh>(constants::EARLY_BODY);
  early_->add_mesh<skinned_animation_mesh>(constants::EARLY_ARMOR);
  early_->add_mesh<animation_mesh>(constants::EARLY_LANCE);
  early_->add_mesh<skinned_animation_mesh>(constants::EARLY_SABER);

  early_->add_mesh<skinned_animation_mesh>(constants::EARLY_HAIR);
  early_->add_mesh<skinned_animation_mesh>(constants::EARLY_SKIRT);
  early_->set_default_animation("Ready");
  early_->set_animation_config("Idle",         true,  1.0f);
  early_->set_animation_config("Ready",        true,  4.0f);
  early_->set_animation_config("Step_Front",   false, 1.0f);
  early_->set_animation_config("Step_Back",    false, 1.0f);
  early_->set_animation_config("Step_Right",   false, 1.0f);
  early_->set_animation_config("Step_Left",    false, 1.0f);
  early_->set_animation_config("Rotate_Back",  false, 1.0f);
  early_->set_animation_config("Rotate_Left",  false, 1.0f);
  early_->set_animation_config("Rotate_Right", false, 1.0f);
  early_->set_animation_config("Attack",       false, 1.0f);
  early_->set_animation_config("Damaged",      false, 1.0f);

  suo_->set_position({1.0f, 0.0f, -1.0f});
  suo_->add_mesh<skinned_animation_mesh>(constants::SUO_BODY);
  suo_->add_mesh<skinned_animation_mesh>(constants::SUO_ARMOR);
  suo_->add_mesh<animation_mesh>(constants::SUO_SABER);

  suo_->set_default_animation("Ready");
  suo_->set_animation_config("Idle",         true,  1.0f);
  suo_->set_animation_config("Ready",        true,  4.0f);
  suo_->set_animation_config("Step_Front",   false, 1.0f);
  suo_->set_animation_config("Step_Back",    false, 1.0f);
  suo_->set_animation_config("Step_Right",   false, 1.0f);
  suo_->set_animation_config("Step_Left",    false, 1.0f);
  suo_->set_animation_config("Rotate_Back",  false, 1.0f);
  suo_->set_animation_config("Rotate_Left",  false, 1.0f);
  suo_->set_animation_config("Rotate_Right", false, 1.0f);
  suo_->set_animation_config("Attack",       false, 1.0f);
  suo_->set_animation_config("Damaged",      false, 1.0f);
  /* ~ Create a Direct3D Device object. */

  /* Create font. ~ */
  ::LPD3DXFONT font{};
  if (FAILED(::D3DXCreateFont(d3d_device_.get(),
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
                              &font))) {
    THROW_WITH_TRACE("Failed to create a font.");
  }
  font_.reset(font, custom_deleter{});
  render_string_object::weak_font_ = font_;
  /* ~ Creates font. */
}

/* windows main loop */
int basic_window::operator()()
{
  while (msg_.message != WM_QUIT) {
    if (::PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
      ::TranslateMessage(&msg_);
      ::DispatchMessage(&msg_);
    } else {
      render();
    }
  }
  return static_cast<int>(msg_.wParam);
}

void basic_window::render()
{
  static int frame_count = 0;
  static float fps = 0;
  static std::chrono::system_clock::time_point start, end;
  if (frame_count == 100) {
    frame_count = 0;
    end = std::chrono::system_clock::now();
    int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - start).count();
    fps = 100 * 1000 / static_cast<float>(elapsed);
    start = std::chrono::system_clock::now();
  }

  if (frame_count == 0) {
    start = std::chrono::system_clock::now();
  }
  ++frame_count;



  ::D3DXVECTOR4 light_direction{};
  {
    if (::GetAsyncKeyState('I') & 0x8000) {
      camera_->move_position({0.0f, 0.0f, 0.002f});
    }
    if (::GetAsyncKeyState('K') & 0x8000) {
      camera_->move_position({0.0f, 0.0f, -0.002f});
    }
    if (::GetAsyncKeyState('J') & 0x8000) {
      camera_->move_position({-0.002f, 0.0f, 0.0f});
    }
    if (::GetAsyncKeyState('L') & 0x8000) {
      camera_->move_position({0.002f, 0.0f, 0.0f});
    }
    if (::GetAsyncKeyState('H') & 0x8000) {
      camera_->move_position({0.0f, 0.002f, 0.0f});
    }
    if (::GetAsyncKeyState('N') & 0x8000) {
      camera_->move_position({0.0f, -0.002f, 0.0f});
    }
    if (::GetAsyncKeyState('Q') & 0x8000) {
      ::PostQuitMessage(0);
    }
    if (::GetAsyncKeyState('F') & 0x8000) {
      light_direction_.x += 0.2f;
      if (light_direction_.x >= 1.0f) {
        light_direction_.x = 1.0f;
      }
    }
    if (::GetAsyncKeyState('S') & 0x8000) {
      light_direction_.x -= 0.2f;
      if (light_direction_.x <= -1.0f) {
        light_direction_.x = -1.0f;
      }
    }
    if (::GetAsyncKeyState('E') & 0x8000) {
      light_direction_.z += 0.2f;
      if (light_direction_.z >= 1.0f) {
        light_direction_.z = 1.0f;
      }
    }
    if (::GetAsyncKeyState('D') & 0x8000) {
      light_direction_.z -= 0.2f;
      if (light_direction_.z <= -1.0f) {
        light_direction_.z = -1.0f;
      }
    }
    if (::GetAsyncKeyState('T') & 0x8000) {
      light_direction_.y += 0.2f;
      if (light_direction_.y >= 1.0f) {
        light_direction_.y = 1.0f;
      }
    }
    if (::GetAsyncKeyState('G') & 0x8000) {
      light_direction_.y -= 0.2f;
      if (light_direction_.y <= -1.0f) {
        light_direction_.y = -1.0f;
      }
    }
    if (::GetAsyncKeyState('R') & 0x8000) {
      light_direction_.x = 0.0f;
      light_direction_.y = 0.0f;
      light_direction_.z = 0.0f;
    }
    if (::GetAsyncKeyState('1') & 0x8000) {
      early_->set_animation("Idle");
      suo_->set_animation("Idle");
    }
    if (::GetAsyncKeyState('2') & 0x8000) {
      early_->set_animation("Ready");
      suo_->set_animation("Ready");
    }
    if (::GetAsyncKeyState('3') & 0x8000) {
      early_->set_animation("Step_Front");
      suo_->set_animation("Step_Front");
    }
    if (::GetAsyncKeyState('4') & 0x8000) {
      early_->set_animation("Step_Back");
      suo_->set_animation("Step_Back");
    }
    if (::GetAsyncKeyState('5') & 0x8000) {
      early_->set_animation("Step_Left");
      suo_->set_animation("Step_Left");
      skinned_animation_mesh_->set_animation("Wolf_Idle_");
    }
    if (::GetAsyncKeyState('6') & 0x8000) {
      early_->set_animation("Step_Right");
      suo_->set_animation("Step_Right");
      skinned_animation_mesh_->set_animation("Wolf_Run_Cycle_");
    }
    if (::GetAsyncKeyState('7') & 0x8000) {
      early_->set_animation("Rotate_Back");
      suo_->set_animation("Rotate_Back");
    }
    if (::GetAsyncKeyState('8') & 0x8000) {
      early_->set_animation("Rotate_Left");
      suo_->set_animation("Rotate_Left");
    }
    if (::GetAsyncKeyState('9') & 0x8000) {
      early_->set_animation("Rotate_Right");
      suo_->set_animation("Rotate_Right");
    }
    if (::GetAsyncKeyState('0') & 0x8000) {
      early_->set_animation("Attack");
      suo_->set_animation("Attack");
    }
    if (::GetAsyncKeyState('P') & 0x8000) {
      early_->set_animation("Damaged");
      suo_->set_animation("Damaged");
    }
    if (::GetAsyncKeyState('Z') & 0x8000) {
      early_->set_dynamic_texture(constants::EARLY_BODY,
          "image/back_ground.png", 0, base_mesh::combine_type::NORMAL);
    }
    if (::GetAsyncKeyState('X') & 0x8000) {
      early_->set_dynamic_texture(constants::EARLY_BODY,
          "image/early_tentative.png", 1, base_mesh::combine_type::NORMAL);
    }
    if (::GetAsyncKeyState('C') & 0x8000) {
      early_->set_fade_in(constants::EARLY_BODY);
      mesh_->set_fade_in();
      animation_mesh_->set_fade_in();
      //static float f = 0.0f;
      //f += 0.01f;
      //early_->set_dynamic_texture_position(
      //    constants::EARLY_BODY, 1, {f, f} );
    }
    if (::GetAsyncKeyState('V') & 0x8000) {
      early_->set_fade_out(constants::EARLY_BODY);
      mesh_->set_fade_out();
      animation_mesh_->set_fade_out();
      //static float f = 3.1415926535f/2;
      //f += 0.1f;
      //early_->set_dynamic_texture_opacity(
      //    constants::EARLY_BODY, 1, std::sin(f)/2+0.5f);
    }
    if (::GetAsyncKeyState('B') & 0x8000) {
      early_->set_dynamic_message(constants::EARLY_BODY, 1,
          "ccccccccccccccccccccc", false, { 210, 270, 511, 511 });
    }
    if (::GetAsyncKeyState('M') & 0x8000) {
      camera_->set_to_close_up_animation();
    }
    if (::GetAsyncKeyState(VK_OEM_COMMA) & 0x8000) {
      camera_->set_to_behind_animation();
    }
    if (::GetAsyncKeyState(VK_OEM_PERIOD) & 0x8000) {
      early_->set_shake_texture(constants::EARLY_BODY);
    }
    (*camera_)();

    if (::GetAsyncKeyState('W') & 0x8000) {
      early_->set_dynamic_message(constants::EARLY_BODY, 1,
          "aaaijijjjaa\n‚ ‚ ‚ ", true, { 210, 270, 511, 511 });
    }

    static float pos = -1.0f;
    if (::GetAsyncKeyState(VK_UP) & 0x8000) {
      pos += 0.02f;
      early_->set_position({0.0f, 0.0f, pos});
    }
    if (::GetAsyncKeyState(VK_DOWN) & 0x8000) {
      pos -= 0.02f;
      early_->set_position({0.0f, 0.0f, pos});
    }

    light_direction.x = light_direction_.x;
    light_direction.y = light_direction_.y;
    light_direction.z = light_direction_.z;
    light_direction.w = 1.0f;
    ::D3DXVec4Normalize(&light_direction, &light_direction);
  }

  ::D3DXMATRIX view_matrix{camera_->get_view_matrix()};
  ::D3DXMATRIX projection_matrix{camera_->get_projection_matrix()};

  d3d_device_->Clear(0,
                     nullptr,
                     D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                     D3DCOLOR_XRGB(0x0f, 0x17, 0x1c),
                     1.0f,
                     0);
   if (SUCCEEDED(d3d_device_->BeginScene())) {
     animation_mesh_->render(view_matrix,
                             projection_matrix,
                             light_direction,
                             light_brightness_);
     skinned_animation_mesh_->render(view_matrix,
                                     projection_matrix,
                                     light_direction,
                                     light_brightness_);
     skinned_animation_mesh2_->render(view_matrix,
                                      projection_matrix,
                                      light_direction,
                                      light_brightness_);
     mesh_->render(view_matrix,
                   projection_matrix,
                   light_direction,
                   light_brightness_);
     mesh2_->render(view_matrix,
                    projection_matrix,
                    light_direction,
                    light_brightness_);
     early_->render(view_matrix,
                    projection_matrix,
                    light_direction,
                    light_brightness_);
     suo_->render(view_matrix,
                    projection_matrix,
                    light_direction,
                    light_brightness_);

    render_string_object::render_string(std::to_string(fps), 10, 30);
//    render_string_object::render_string(
//        std::to_string(eye_position_.x), 10, 50);
//    render_string_object::render_string(
//        std::to_string(eye_position_.y), 10, 70);
//    render_string_object::render_string(
//        std::to_string(eye_position_.z), 10, 90);

    d3d_device_->EndScene();
  }
  d3d_device_->Present(nullptr, nullptr, nullptr, nullptr);
}

void basic_window::render_string_object::render_string(
    const std::string& message, const int& x, const int& y)
{
  std::shared_ptr<::ID3DXFont> font{render_string_object::weak_font_.lock()};

  if (font) {
    ::RECT rect{ x, y, 0, 0 };
    /* Calcurate size of character string. */
    font->DrawText(NULL,
                   message.c_str(),
                   -1,
                   &rect,
                   DT_CALCRECT,
                   NULL);
    /* Draw with its size. */
    font->DrawText(nullptr,
                   message.c_str(),
                   -1,
                   &rect,
                   DT_LEFT | DT_BOTTOM,
                   0xff00ff00);
  }
}

} /* namespace early_go */
