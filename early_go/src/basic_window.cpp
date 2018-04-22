#include "stdafx.hpp"

#include "animation_mesh.hpp"
#include "skinned_animation_mesh.hpp"
#include "basic_window.hpp"
#include "mesh.hpp"

namespace early_go {
/* A definition of the static member variable. */
std::weak_ptr<::ID3DXFont> basic_window::render_string_object::swp_id3dx_font_;

/* c'tor */
basic_window::basic_window(const ::HINSTANCE& a_kr_hinstance)
    : up_direct3d9_{nullptr, custom_deleter{}},
      d3d_present_parameters_{},
      sp_direct3d_device9_{},
      sp_id3dx_font_{},
      sp_animation_mesh_{},
      sp_skinned_animation_mesh_{},
      sp_skinned_animation_mesh2_{},
      sp_mesh_{},
      sp_mesh2_{},
      mat_view_{},
      mat_projection_{},
      light_direction_{-1.0f, 0.0f, 0.0f},
      light_brightness_{1.0f},
      vec_eye_position_{0.0f, 2.0f, -4.0f},
      vec_look_at_position_{0.0f, 0.0f, 0.0f}
{
  ::WNDCLASSEX wndclassex{};
  wndclassex.cbSize        = sizeof(wndclassex);
  wndclassex.style         = CS_HREDRAW | CS_VREDRAW;
  wndclassex.lpfnWndProc   = [](::HWND a_hwnd,
                                ::UINT a_ui_message,
                                ::WPARAM a_wparam,
                                ::LPARAM a_lparam) -> ::LRESULT {
    if (a_ui_message == WM_CLOSE) {
      ::PostQuitMessage(0);
    } else {
      return ::DefWindowProc(a_hwnd, a_ui_message, a_wparam, a_lparam);
    }
    return 0;
  };
  wndclassex.cbClsExtra    = 0;
  wndclassex.cbWndExtra    = 0;
  wndclassex.hInstance     = a_kr_hinstance;
  wndclassex.hIcon         = ::LoadIcon(nullptr, IDI_APPLICATION);
  wndclassex.hCursor       = ::LoadCursor(nullptr, IDC_ARROW);
  wndclassex.hbrBackground =
      static_cast<::HBRUSH>(::GetStockObject(BLACK_BRUSH));
  wndclassex.lpszMenuName  = nullptr;
  wndclassex.lpszClassName = constants::APP_NAME.c_str();
  wndclassex.hIconSm       = ::LoadIcon(nullptr, IDI_APPLICATION);

  ::RegisterClassEx(&wndclassex);

  ::HWND hwnd{::CreateWindow(constants::APP_NAME.c_str(),
                              constants::APP_NAME.c_str(),
                              WS_OVERLAPPEDWINDOW,
                              0,
                              0,
                              constants::WINDOW_WIDTH,
                              constants::WINDOW_HEIGHT,
                              nullptr,
                              nullptr,
                              a_kr_hinstance,
                              nullptr)};
  ::ShowWindow(hwnd, SW_SHOW);
  ::UpdateWindow(hwnd);
  this->initialize_direct3d(hwnd);
}

void basic_window::initialize_direct3d(const ::HWND& a_kr_hwnd)
{
  /* Create a Direct3D object. */
  ::LPDIRECT3D9 p_direct3d9{::Direct3DCreate9(D3D_SDK_VERSION)};

  if (nullptr == p_direct3d9) {
    ::MessageBox(0, "Direct3D‚Ì‰Šú‰»‚ÉŽ¸”s‚µ‚Ü‚µ‚½", "", MB_OK);
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to create 'Direct3D'."});
  }
  this->up_direct3d9_.reset(p_direct3d9);

  /* Create a Direct3D Device object. ~ */
  this->d3d_present_parameters_.BackBufferFormat       = ::D3DFMT_UNKNOWN;
  this->d3d_present_parameters_.BackBufferCount        = 1;
  this->d3d_present_parameters_.SwapEffect             =
                                                        ::D3DSWAPEFFECT_DISCARD;
  this->d3d_present_parameters_.Windowed               = TRUE;
  this->d3d_present_parameters_.EnableAutoDepthStencil = TRUE;
  this->d3d_present_parameters_.AutoDepthStencilFormat = ::D3DFMT_D16;

  /* for receiving */
  ::LPDIRECT3DDEVICE9 p_direct3d_device9{};

  if (FAILED(
      this->up_direct3d9_->CreateDevice(D3DADAPTER_DEFAULT,
                                        ::D3DDEVTYPE_HAL,
                                        a_kr_hwnd,
                                        D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                        &this->d3d_present_parameters_,
                                        &p_direct3d_device9))) {
    if (FAILED(
        this->up_direct3d9_->CreateDevice(D3DADAPTER_DEFAULT,
                                          ::D3DDEVTYPE_HAL,
                                          a_kr_hwnd,
                                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                          &this->d3d_present_parameters_,
                                          &p_direct3d_device9))) {
      if (SUCCEEDED(
          this->up_direct3d9_->CreateDevice(D3DADAPTER_DEFAULT,
                                            ::D3DDEVTYPE_REF,
                                            a_kr_hwnd,
                                            D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                            &this->d3d_present_parameters_,
                                            &p_direct3d_device9))) {
        ::MessageBox(0,
            constants::FAILED_TO_CREATE_HARDWARE_MODE_MESSAGE.c_str(),
            nullptr, MB_OK);
      } else if (SUCCEEDED(
          this->up_direct3d9_->CreateDevice(D3DADAPTER_DEFAULT,
                                            ::D3DDEVTYPE_REF,
                                            a_kr_hwnd,
                                            D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                            &this->d3d_present_parameters_,
                                            &p_direct3d_device9))) {
        ::MessageBox(0,
            constants::FAILED_TO_CREATE_HARDWARE_MODE_MESSAGE.c_str(),
            nullptr, MB_OK);
      } else {
        BOOST_THROW_EXCEPTION(
            custom_exception{"Failed to create 'Direct3D Device'."});
      }
    }
  }
  /* lazy initialization */
   this->sp_direct3d_device9_.reset(p_direct3d_device9, custom_deleter{});
  this->sp_animation_mesh_.reset(
      new_crt animation_mesh{this->sp_direct3d_device9_,
                             constants::ANIMATION_MESH_FILE_NAME,
                             ::D3DXVECTOR3{0.0f, 0.0f, 0.0f},
                             1.0f});
  this->sp_skinned_animation_mesh_.reset(
      new_crt skinned_animation_mesh{this->sp_direct3d_device9_,
                             constants::SKINNED_ANIMATION_MESH_FILE_NAME,
                             ::D3DXVECTOR3{1.0f, 0.0f, 0.0f},
                             1.0f});
  this->sp_skinned_animation_mesh2_.reset(
      new_crt skinned_animation_mesh{this->sp_direct3d_device9_,
                             constants::SKINNED_ANIMATION_MESH_FILE_NAME2,
                             ::D3DXVECTOR3{-1.0f, 0.0f, 0.0f},
                             1.0f});
  this->sp_mesh_.reset(new_crt mesh{this->sp_direct3d_device9_,
                                    constants::MESH_FILE_NAME,
                                    ::D3DXVECTOR3{-0.5f, 0.0f, -1.0f},
                                    1.0f});
  this->sp_mesh2_.reset(new_crt mesh{this->sp_direct3d_device9_,
                                     constants::MESH_FILE_NAME2,
                                     ::D3DXVECTOR3{ 0.5f, 0.0f, 2.0f},
                                     1.0f});

  /* ~ Create a Direct3D Device object. */

  /* Create a font. ~ */
  ::LPD3DXFONT p_d3dx_font{};
  if (FAILED(::D3DXCreateFont(this->sp_direct3d_device9_.get(),
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
                              &p_d3dx_font))) {
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to create a font."});
  }
  this->sp_id3dx_font_.reset(p_d3dx_font, custom_deleter{});
  render_string_object::swp_id3dx_font_ = this->sp_id3dx_font_;
  /* ~ Creates a font. */
}

/* windows main loop */
int basic_window::operator()()
{
  while (this->msg_.message != WM_QUIT) {
    if (::PeekMessage(&this->msg_, nullptr, 0, 0, PM_REMOVE)) {
      ::TranslateMessage(&this->msg_);
      ::DispatchMessage(&this->msg_);
    } else {
      this->render();
    }
  }
  return static_cast<int>(this->msg_.wParam);
}

void basic_window::render()
{
  static int count_frames = 0;
  static float f_fps = 0;
  static std::chrono::system_clock::time_point  start, end;
  if (count_frames == 100) {
    count_frames = 0;
    end = std::chrono::system_clock::now();
    int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - start).count();
    f_fps = 100 * 1000 / static_cast<float>(elapsed);
    start = std::chrono::system_clock::now();
  }

  if (count_frames == 0) {
    start = std::chrono::system_clock::now();
  }
  ++count_frames;



  ::D3DXVECTOR4 vec4_light_direction{};
  {
    if (::GetAsyncKeyState('I') & 0x8000) {
      this->vec_eye_position_.z += 0.02f;
      this->vec_look_at_position_.z += 0.02f;
    }
    if (::GetAsyncKeyState('K') & 0x8000) {
      this->vec_eye_position_.z -= 0.02f;
      this->vec_look_at_position_.z -= 0.02f;
    }
    if (::GetAsyncKeyState('J') & 0x8000) {
      this->vec_eye_position_.x -= 0.02f;
      this->vec_look_at_position_.x -= 0.02f;
    }
    if (::GetAsyncKeyState('L') & 0x8000) {
      this->vec_eye_position_.x += 0.02f;
      this->vec_look_at_position_.x += 0.02f;
    }
    if (::GetAsyncKeyState('H') & 0x8000) {
      this->vec_eye_position_.y += 0.02f;
      this->vec_look_at_position_.y += 0.02f;
    }
    if (::GetAsyncKeyState('N') & 0x8000) {
      this->vec_eye_position_.y -= 0.02f;
      this->vec_look_at_position_.y -= 0.02f;
    }
    if (::GetAsyncKeyState('Q') & 0x8000) {
      ::PostQuitMessage(0);
    }
    if (::GetAsyncKeyState('F') & 0x8000) {
      this->light_direction_.x += 0.2f;
      if (this->light_direction_.x >= 1.0f) {
        this->light_direction_.x = 1.0f;
      }
    }
    if (::GetAsyncKeyState('S') & 0x8000) {
      this->light_direction_.x -= 0.2f;
      if (this->light_direction_.x <= -1.0f) {
        this->light_direction_.x = -1.0f;
      }
    }
    if (::GetAsyncKeyState('E') & 0x8000) {
      this->light_direction_.z += 0.2f;
      if (this->light_direction_.z >= 1.0f) {
        this->light_direction_.z = 1.0f;
      }
    }
    if (::GetAsyncKeyState('D') & 0x8000) {
      this->light_direction_.z -= 0.2f;
      if (this->light_direction_.z <= -1.0f) {
        this->light_direction_.z = -1.0f;
      }
    }
    if (::GetAsyncKeyState('T') & 0x8000) {
      this->light_direction_.y += 0.2f;
      if (this->light_direction_.y >= 1.0f) {
        this->light_direction_.y = 1.0f;
      }
    }
    if (::GetAsyncKeyState('G') & 0x8000) {
      this->light_direction_.y -= 0.2f;
      if (this->light_direction_.y <= -1.0f) {
        this->light_direction_.y = -1.0f;
      }
    }
    if (::GetAsyncKeyState('R') & 0x8000) {
      this->light_direction_.x = 0.0f;
      this->light_direction_.y = 0.0f;
      this->light_direction_.z = 0.0f;
    }
    if (::GetAsyncKeyState('1') & 0x8000) {
      this->sp_skinned_animation_mesh_->play_animation_set(1);
    }
    if (::GetAsyncKeyState('2') & 0x8000) {
      this->sp_skinned_animation_mesh_->play_animation_set(2);
    }
    if (::GetAsyncKeyState('3') & 0x8000) {
      this->sp_skinned_animation_mesh_->play_animation_set(3);
    }
    if (::GetAsyncKeyState('4') & 0x8000) {
      this->sp_skinned_animation_mesh_->play_animation_set(4);
    }
    if (::GetAsyncKeyState('5') & 0x8000) {
      this->sp_skinned_animation_mesh_->play_animation_set("Wolf_Walk_cycle_");
    }
    if (::GetAsyncKeyState('Z') & 0x8000) {
      this->sp_mesh_->set_dynamic_texture(
          "board.png", 0, mesh::combine_type::NORMAL);
    }
    if (::GetAsyncKeyState('X') & 0x8000) {
      this->sp_mesh_->set_dynamic_texture(
          "board2.png", 0, mesh::combine_type::NORMAL);
    }
    if (::GetAsyncKeyState('C') & 0x8000) {
      static float f = 0.0f;
      f += 0.01f;
      this->sp_mesh_->set_dynamic_texture_position(0, {f, f} );
    }
    if (::GetAsyncKeyState('V') & 0x8000) {
      static float f = 3.1415926535f/2;
      f += 0.1f;
      this->sp_mesh_->set_dynamic_texture_opacity(0, std::sin(f)/2+0.5f);
    }
    if (::GetAsyncKeyState('B') & 0x8000) {
      this->sp_mesh_->set_dynamic_message(0, "abcijijihoge\na‚ ‚ ", {50, 50, 150, 150});
//      this->sp_mesh_->set_dynamic_message(0, "‚P‚Q‚R‚S‚T‚U‚V‚W‚X");
    }
    vec4_light_direction.x = this->light_direction_.x;
    vec4_light_direction.y = this->light_direction_.y;
    vec4_light_direction.z = this->light_direction_.z;
    vec4_light_direction.w = 1.0f;
    ::D3DXVec4Normalize(&vec4_light_direction, &vec4_light_direction);


    ::D3DXVECTOR3 vec_up_vector{ 0.0f, 1.0f, 0.0f};
    ::D3DXMatrixLookAtLH(&this->mat_view_,
                         &this->vec_eye_position_,
                         &this->vec_look_at_position_,
                         &vec_up_vector);
  }
  {
    ::D3DXMatrixPerspectiveFovLH(
        &this->mat_projection_,
        D3DX_PI / 4,
        static_cast<float>(constants::WINDOW_WIDTH) / constants::WINDOW_HEIGHT,
        0.1f,
        3000.0f);
  }


  this->sp_direct3d_device9_->Clear(0,
                                    nullptr,
                                    D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                                    D3DCOLOR_XRGB(0x0f, 0x17, 0x1c),
                                    1.0f,
                                    0);
   if (SUCCEEDED(this->sp_direct3d_device9_->BeginScene())) {
     this->sp_animation_mesh_->render(this->mat_view_,
                                      this->mat_projection_,
                                      vec4_light_direction,
                                      this->light_brightness_);
     this->sp_skinned_animation_mesh_->render(this->mat_view_,
                                              this->mat_projection_,
                                              vec4_light_direction,
                                              this->light_brightness_);
     this->sp_skinned_animation_mesh2_->render(this->mat_view_,
                                               this->mat_projection_,
                                               vec4_light_direction,
                                               this->light_brightness_);
     this->sp_mesh_->render(this->mat_view_,
                            this->mat_projection_,
                            vec4_light_direction,
                            this->light_brightness_);
     this->sp_mesh2_->render(this->mat_view_,
                            this->mat_projection_,
                            vec4_light_direction,
                            this->light_brightness_);

    render_string_object::render_string(std::to_string(f_fps), 10, 30);
    render_string_object::render_string(std::to_string(vec4_light_direction.x), 10, 50);
    render_string_object::render_string(std::to_string(vec4_light_direction.y), 10, 70);
    render_string_object::render_string(std::to_string(vec4_light_direction.z), 10, 90);
    render_string_object::render_string(std::to_string(vec4_light_direction.w), 10, 110);

    this->sp_direct3d_device9_->EndScene();
  }
  this->sp_direct3d_device9_->Present(nullptr, nullptr, nullptr, nullptr);
}

void basic_window::render_string_object::render_string(
    const std::string& a_sz_target,
    const int& a_kr_i_x,
    const int& a_kr_i_y)
{
  std::shared_ptr<::ID3DXFont> sp_d3dx_font{
      render_string_object::swp_id3dx_font_.lock()};

  if (sp_d3dx_font) {
    ::RECT rect{ a_kr_i_x, a_kr_i_y, 0, 0 };
    /* Calcurate size of character string. */
    sp_d3dx_font->DrawText(NULL,
                           a_sz_target.c_str(),
                           -1,
                           &rect,
                           DT_CALCRECT,
                           NULL);
    /* Draw with its size. */
    sp_d3dx_font->DrawText(nullptr,
                           a_sz_target.c_str(),
                           -1,
                           &rect,
                           DT_LEFT | DT_BOTTOM,
                           0xff00ff00);
  }
}
} /* namespace early_go */
