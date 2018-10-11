#include "stdafx.hpp"

#include <boost/stacktrace.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "basic_window.hpp"
#include "error_dialog.hpp"
#include "resource.h"

int WINAPI WinMain(_In_ ::HINSTANCE hinstance,
                   _In_opt_ ::HINSTANCE, _In_ ::LPSTR, _In_ ::INT)
try {
#if (defined(DEBUG) || defined(_DEBUG)) && defined(MEMORY_LEAKS)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  early_go::basic_window window{hinstance};
  return window();
} catch (const std::exception& e) {
  early_go::show_error_dialog(hinstance, e);
  return EXIT_FAILURE;
}
