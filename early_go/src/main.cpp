#include "stdafx.hpp"

#include "basic_window.hpp"
#include "resource.h"

std::string error_msg;

int WINAPI WinMain(_In_ ::HINSTANCE hinstance,
                   _In_opt_ ::HINSTANCE, _In_ ::LPSTR, _In_ ::INT)
try {
#if (defined(DEBUG) || defined(_DEBUG)) && defined(MEMORY_LEAKS)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  early_go::basic_window window{hinstance};

  return window();
} catch (const std::exception& exception) {
  early_go::log_liner{} << boost::diagnostic_information(exception);

  // TODO : extract to inline_macro.hpp
  ::DLGPROC dialog_procedure = [](::HWND hwnd,
                                  ::UINT msg,
                                  ::WPARAM wparam,
                                  ::LPARAM) -> ::INT_PTR {
    if (msg == WM_COMMAND) {
      if (LOWORD(wparam) == IDOK) {
        ::EndDialog(hwnd, IDOK);
        return TRUE;
      }
    } else if (msg == WM_CLOSE) {
      ::EndDialog(hwnd, IDOK);
      return TRUE;
    } else if (msg == WM_SIZE) {
      ::HWND edit_box{::GetDlgItem(hwnd, IDC_EDIT1)};
      ::HWND button{::GetDlgItem(hwnd, IDOK)};

      ::RECT dialog_rect{};
      ::RECT edit_box_rect{};
      ::RECT button_rect{};

      ::GetClientRect(hwnd,     &dialog_rect);
      ::GetClientRect(edit_box, &edit_box_rect);
      ::GetClientRect(button,   &button_rect);
      
      ::MoveWindow(edit_box,
                   0,
                   30,
                   dialog_rect.right,
                   dialog_rect.bottom - button_rect.bottom*2 - 30,
                   TRUE);

      ::MoveWindow(button,
                   dialog_rect.right  - button_rect.right*3/2,
                   dialog_rect.bottom - button_rect.bottom*3/2,
                   button_rect.right,
                   button_rect.bottom,
                   TRUE);

      ::SetDlgItemText(hwnd, IDC_EDIT1, error_msg.c_str());
    }
    return FALSE;
  };
  std::string tmp = boost::diagnostic_information(exception);
  error_msg.clear();

  for (auto cit{tmp.cbegin()}; cit != tmp.cend(); ++cit) {
    if (*cit == '\n') {
      error_msg.push_back('\r');
    }
    error_msg.push_back(*cit);
  }

  ::DialogBox(hinstance,
      MAKEINTRESOURCE(IDD_DIALOG1), nullptr, dialog_procedure);

  return EXIT_FAILURE;
}
