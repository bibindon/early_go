#include "stdafx.hpp"

#include "animation_mesh.hpp"
#include "basic_window.hpp"
#include "mesh.hpp"

namespace early_go {
/*
 * A struct of for the argument of the 'lpfnWndProc' that is the member of
 * '::RegisterClassEx'. It's necessary for creating a window to run previously
 * the '::RegisterClassEx' function, and that function needs an instance of
 * WNDCLASSEX as a argument. The 'lpfnWndProc', WNDCLASSEX's member variables,
 * requires a free function like 'C' language style. Consequently, defines a
 * static function and a static weak-pointer for a substitute of the free
 * function.
 */
struct basic_window::window_procedure_object
{
  static std::weak_ptr<animation_mesh> swp_animation_mesh_;
  /* TODO Use direct input */
  static ::LRESULT CALLBACK window_procedure(::HWND a_hwnd,
                                             ::UINT a_ui_message,
                                             ::WPARAM a_wparam,
                                             ::LPARAM a_lparam)
  {
    switch (a_ui_message) {
    case WM_DESTROY:
      ::PostQuitMessage(0);
      break;
    case WM_KEYDOWN:
      switch (static_cast<char>(a_wparam)) {
      case VK_ESCAPE:
        ::PostQuitMessage(0);
        break;
      case VK_SPACE:
        std::shared_ptr<animation_mesh> _shared_ptr{
            window_procedure_object::swp_animation_mesh_.lock()};

        if (_shared_ptr) {
          _shared_ptr->set_play_animation(
              _shared_ptr->get_play_animation() == false ? true : false);
        }
        break;
      }
      break;
    }
    return ::DefWindowProc(a_hwnd, a_ui_message, a_wparam, a_lparam);
  }
};

/* A definition of the static member variable. */
std::weak_ptr<animation_mesh>
    basic_window::window_procedure_object::swp_animation_mesh_;
/* A definition of the static member variable. */
std::weak_ptr<::ID3DXFont> basic_window::render_string_object::swp_id3dx_font_;

/* c'tor */
basic_window::basic_window(const ::HINSTANCE& a_kr_hinstance)
    : up_direct3d9_{nullptr, custom_deleter{}},
      d3d_present_parameters_{},
      sp_direct3d_device9_{},
      sp_id3dx_font_{},
      sp_animation_mesh_{},
      sp_animation_mesh2_{},
      sp_mesh_{},
      sp_mesh2_{}
{
  ::WNDCLASSEX _wndclassex{};
  _wndclassex.cbSize        = sizeof(_wndclassex);
  _wndclassex.style         = CS_HREDRAW | CS_VREDRAW;
  _wndclassex.lpfnWndProc   = window_procedure_object::window_procedure;
  _wndclassex.cbClsExtra    = 0;
  _wndclassex.cbWndExtra    = 0;
  _wndclassex.hInstance     = a_kr_hinstance;
  _wndclassex.hIcon         = ::LoadIcon(nullptr, IDI_APPLICATION);
  _wndclassex.hCursor       = ::LoadCursor(nullptr, IDC_ARROW);
  _wndclassex.hbrBackground =
      static_cast<::HBRUSH>(::GetStockObject(BLACK_BRUSH));
  _wndclassex.lpszMenuName  = nullptr;
  _wndclassex.lpszClassName = constants::APP_NAME.c_str();
  _wndclassex.hIconSm       = ::LoadIcon(nullptr, IDI_APPLICATION);

  ::RegisterClassEx(&_wndclassex);

  ::HWND _hwnd{::CreateWindow(constants::APP_NAME.c_str(),
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
  ::ShowWindow(_hwnd, SW_SHOW);
  ::UpdateWindow(_hwnd);
  this->initialize_direct3d(_hwnd);
}

void basic_window::initialize_direct3d(const ::HWND& a_kr_hwnd)
{
  /* Create a Direct3D object. */
  ::LPDIRECT3D9 _p_direct3d9{::Direct3DCreate9(D3D_SDK_VERSION)};

  if (nullptr == _p_direct3d9) {
    ::MessageBox(0, "Direct3D‚Ì‰Šú‰»‚ÉŽ¸”s‚µ‚Ü‚µ‚½", "", MB_OK);
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to create 'Direct3D'."});
  }
  this->up_direct3d9_.reset(_p_direct3d9);

  /* Create a Direct3D Device object. ~ */
  this->d3d_present_parameters_.BackBufferFormat       = ::D3DFMT_UNKNOWN;
  this->d3d_present_parameters_.BackBufferCount        = 1;
  this->d3d_present_parameters_.SwapEffect             =
                                                        ::D3DSWAPEFFECT_DISCARD;
  this->d3d_present_parameters_.Windowed               = TRUE;
  this->d3d_present_parameters_.EnableAutoDepthStencil = TRUE;
  this->d3d_present_parameters_.AutoDepthStencilFormat = ::D3DFMT_D16;

  /* for receiving */
  ::LPDIRECT3DDEVICE9 _p_direct3d_device9{};

  if (FAILED(
      this->up_direct3d9_->CreateDevice(D3DADAPTER_DEFAULT,
                                        ::D3DDEVTYPE_HAL,
                                        a_kr_hwnd,
                                        D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                        &this->d3d_present_parameters_,
                                        &_p_direct3d_device9))) {
    if (FAILED(
        this->up_direct3d9_->CreateDevice(D3DADAPTER_DEFAULT,
                                          ::D3DDEVTYPE_HAL,
                                          a_kr_hwnd,
                                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                          &this->d3d_present_parameters_,
                                          &_p_direct3d_device9))) {
      if (SUCCEEDED(
          this->up_direct3d9_->CreateDevice(D3DADAPTER_DEFAULT,
                                            ::D3DDEVTYPE_REF,
                                            a_kr_hwnd,
                                            D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                            &this->d3d_present_parameters_,
                                            &_p_direct3d_device9))) {
        ::MessageBox(0,
            constants::FAILED_TO_CREATE_HARDWARE_MODE_MESSAGE.c_str(),
            nullptr, MB_OK);
      } else if (SUCCEEDED(
          this->up_direct3d9_->CreateDevice(D3DADAPTER_DEFAULT,
                                            ::D3DDEVTYPE_REF,
                                            a_kr_hwnd,
                                            D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                            &this->d3d_present_parameters_,
                                            &_p_direct3d_device9))) {
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
  this->sp_direct3d_device9_.reset(_p_direct3d_device9, custom_deleter{});
  this->sp_animation_mesh_.reset(
      new_crt animation_mesh{this->sp_direct3d_device9_,
                             constants::ANIMATION_MESH_FILE_PATH});
  this->sp_animation_mesh2_.reset(
      new_crt animation_mesh{this->sp_direct3d_device9_,
                             constants::ANIMATION_MESH_FILE_PATH,
                             ::D3DXVECTOR3{1.0f, 1.0f, 1.0f}});
  this->sp_mesh_.reset(new_crt mesh{this->sp_direct3d_device9_,
                                    constants::MESH_FILE_PATH,
                                    ::D3DXVECTOR3{-0.5f, 0.0f, 3.0f}});
  this->sp_mesh2_.reset(new_crt mesh{this->sp_direct3d_device9_,
                                     constants::MESH_FILE_PATH2,
                                     ::D3DXVECTOR3{ 0.5f, -1.0f, 3.0f}});

  /* Give a reference for using static function. */
  window_procedure_object::swp_animation_mesh_ = sp_animation_mesh_;

  this->sp_direct3d_device9_->SetRenderState(::D3DRS_ZENABLE, TRUE);
  this->sp_direct3d_device9_->SetRenderState(::D3DRS_CULLMODE, ::D3DCULL_NONE);
  this->sp_direct3d_device9_->SetRenderState(::D3DRS_LIGHTING, TRUE);
  this->sp_direct3d_device9_->SetRenderState(::D3DRS_AMBIENT, 0x80808080);
  this->sp_direct3d_device9_->SetRenderState(::D3DRS_SPECULARENABLE, TRUE);
  //this->sp_direct3d_device9_->SetRenderState(::D3DRS_ALPHABLENDENABLE, TRUE);
  //this->sp_direct3d_device9_->SetRenderState(::D3DRS_SRCBLEND,
  //                                           ::D3DBLEND_SRCALPHA);
  //this->sp_direct3d_device9_->SetRenderState(::D3DRS_DESTBLEND,
  //                                           ::D3DBLEND_INVSRCALPHA);
  this->sp_direct3d_device9_->SetRenderState(::D3DRS_ALPHATESTENABLE, TRUE);
  /* ~ Create a Direct3D Device object. */

  /* Create a font. ~ */
  ::LPD3DXFONT _p_d3dx_font{};
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
                              "MeiryoKe_Gothic",
                              &_p_d3dx_font))) {
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to create a font."});
  }
  this->sp_id3dx_font_.reset(_p_d3dx_font, custom_deleter{});
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
  this->sp_direct3d_device9_->Clear(0,
                                    nullptr,
                                    D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                                    D3DCOLOR_XRGB(0x0f, 0x17, 0x1c),
                                    1.0f,
                                    0);
  if (SUCCEEDED(this->sp_direct3d_device9_->BeginScene())) {
    this->update_light();
    this->sp_animation_mesh_->render();
    this->sp_animation_mesh2_->render();
    this->sp_mesh_->render();
    this->sp_mesh2_->render();

    this->sp_direct3d_device9_->EndScene();
  }
  this->sp_direct3d_device9_->Present(nullptr, nullptr, nullptr, nullptr);
}

void basic_window::update_light()
{
  /* Shed light. Set white color and specular reflection. */
  ::D3DXVECTOR3 _vec_direction{ 1, -0.5, 1 };
  ::D3DLIGHT9 _light = {};
  _light.Type       = ::D3DLIGHT_DIRECTIONAL;
  _light.Diffuse.r  = 1.0f;
  _light.Diffuse.g  = 1.0f;
  _light.Diffuse.b  = 1.0f;
  _light.Specular.r = 1.0f;
  _light.Specular.g = 1.0f;
  _light.Specular.b = 1.0f;
  ::D3DXVec3Normalize(static_cast<::D3DXVECTOR3*>(&_light.Direction),
                      &_vec_direction);

  _light.Range = 200.0f;
  this->sp_direct3d_device9_->SetLight(0, &_light);
  this->sp_direct3d_device9_->LightEnable(0, true);
}

void basic_window::render_string_object::render_string(
    const std::string& a_sz_target,
    const int& a_kr_i_x,
    const int& a_kr_i_y)
{
  std::shared_ptr<::ID3DXFont> _sp_d3dx_font{
      render_string_object::swp_id3dx_font_.lock()};

  if (_sp_d3dx_font) {
    ::RECT _rect{ a_kr_i_x, a_kr_i_y, 0, 0 };
    /* Calcurate size of character string. */
    _sp_d3dx_font->DrawText(NULL,
                            a_sz_target.c_str(),
                            -1,
                            &_rect,
                            DT_CALCRECT,
                            NULL);
    /* Draw with its size. */
    _sp_d3dx_font->DrawText(nullptr,
                            a_sz_target.c_str(),
                            -1,
                            &_rect,
                            DT_LEFT | DT_BOTTOM,
                            0xff00ff00);
  }
}
} /* namespace early_go */
