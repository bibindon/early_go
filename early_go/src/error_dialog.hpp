#ifndef ERROR_DIALOG_HPP
#define ERROR_DIALOG_HPP

#include "stdafx.hpp"

namespace early_go
{

extern boost::exception_ptr early_go::exception_reserve;
static std::string error_msg;

INT_PTR CALLBACK error_dialog_procedure(::HWND, UINT, WPARAM, LPARAM);
void show_error_dialog(const HINSTANCE& hinstance, const std::exception& e);

}
#endif

