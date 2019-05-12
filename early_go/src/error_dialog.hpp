#ifndef ERROR_DIALOG_HPP
#define ERROR_DIALOG_HPP

#include "stdafx.hpp"
#include "resource.h"

#include <boost/algorithm/string/replace.hpp>

namespace early_go {

boost::exception_ptr early_go::exception_reserve{};
static std::string error_msg{};

::INT_PTR CALLBACK error_dialog_procedure(::HWND, ::UINT, ::WPARAM, ::LPARAM);

void show_error_dialog(const ::HINSTANCE& hinstance, const std::exception& e)
{
  std::string tmp = boost::diagnostic_information(e);
  boost::algorithm::replace_all(tmp, "\n", "\r\n");
  error_msg = tmp;

  if (early_go::exception_reserve != nullptr) {
    tmp = "[Reserved exception] \n";
    tmp += boost::diagnostic_information(early_go::exception_reserve);
    boost::algorithm::replace_all(tmp, "\n", "\r\n");
    error_msg += tmp;
  }

  ::DialogBox(hinstance, MAKEINTRESOURCE(IDD_DIALOG1),
      nullptr, error_dialog_procedure);
}

::INT_PTR CALLBACK error_dialog_procedure(
    ::HWND hwnd, ::UINT msg, ::WPARAM wparam, ::LPARAM)
{
  if (msg == WM_SHOWWINDOW) {
    ::SetWindowPos(hwnd, NULL, 400, 400, 1600, 900, NULL);
    return TRUE;
  } else if (msg == WM_COMMAND) {
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

}

#endif
