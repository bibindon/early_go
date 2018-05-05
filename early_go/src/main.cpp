#include "stdafx.hpp"

#include "basic_window.hpp"
#include "resource.h"

std::string sz_exception;

int WINAPI WinMain(_In_ ::HINSTANCE a_hinstance,
                   _In_opt_ ::HINSTANCE, _In_ ::LPSTR, _In_ ::INT)
try {
#if (defined(DEBUG) || defined(_DEBUG)) && defined(MEMORY_LEAKS)
  _CrtSetDbgFlag(
      _CRTDBG_ALLOC_MEM_DF      |
//      _CRTDBG_DELAY_FREE_MEM_DF |
//      _CRTDBG_CHECK_ALWAYS_DF   |
      _CRTDBG_LEAK_CHECK_DF);
#endif

  early_go::basic_window window{a_hinstance};

  return window();
} catch (const std::exception& a_exception) {
  early_go::log_liner{} << boost::diagnostic_information(a_exception);

  // TODO : extract to inline_macro.hpp
  ::DLGPROC dialog_procedure = [](::HWND a_hwnd,
                                  ::UINT a_ui_message,
                                  ::WPARAM a_wparam,
                                  ::LPARAM) -> ::INT_PTR {
    if (a_ui_message == WM_COMMAND) {
      if (LOWORD(a_wparam) == IDOK) {
        ::EndDialog(a_hwnd, IDOK);
        return TRUE;
      }
    } else if (a_ui_message == WM_CLOSE) {
      ::EndDialog(a_hwnd, IDOK);
      return TRUE;
    } else if (a_ui_message == WM_SIZE) {
      ::HWND h_edit_box{::GetDlgItem(a_hwnd, IDC_EDIT1)};
      ::HWND h_button{::GetDlgItem(a_hwnd, IDOK)};

      ::RECT rect_dialog{};
      ::RECT rect_edit_box{};
      ::RECT rect_button{};

      ::GetClientRect(a_hwnd,     &rect_dialog);
      ::GetClientRect(h_edit_box, &rect_edit_box);
      ::GetClientRect(h_button,   &rect_button);
      
      ::MoveWindow(h_edit_box,
                   0,
                   30,
                   rect_dialog.right,
                   rect_dialog.bottom - rect_button.bottom*2 - 30,
                   TRUE);

      ::MoveWindow(h_button,
                   rect_dialog.right  - rect_button.right*3/2,
                   rect_dialog.bottom - rect_button.bottom*3/2,
                   rect_button.right,
                   rect_button.bottom,
                   TRUE);

      ::SetDlgItemText(a_hwnd, IDC_EDIT1, sz_exception.c_str());
    }
    return FALSE;
  };
  std::string tmp = boost::diagnostic_information(a_exception);
  sz_exception.clear();

  for (std::string::const_iterator cit{tmp.cbegin()}; cit != tmp.cend(); ++cit)
  {
    if (*cit == '\n')
    {
      sz_exception.push_back('\r');
    }
    sz_exception.push_back(*cit);
  }

  ::DialogBox(a_hinstance,
      MAKEINTRESOURCE(IDD_DIALOG1), nullptr, dialog_procedure);

  return EXIT_FAILURE;
}
