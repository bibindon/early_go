#include "stdafx.hpp"

#include "main_window.hpp"
#include "error_dialog.hpp"
#include "resource.h"

#include <boost/stacktrace.hpp>
#include <boost/algorithm/string/replace.hpp>

int WINAPI WinMain(_In_ HINSTANCE hinstance,
                   _In_opt_ HINSTANCE, _In_ LPSTR, _In_ INT)
try
{
#if (defined(DEBUG) || defined(_DEBUG))
    // OpenCV always output memory leaks.
    // If filename and line number are not shown, those are OpenCV memory leaks.
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    early_go::main_window window{hinstance};
    return window();
}
catch (const std::exception &e)
{
    early_go::show_error_dialog(hinstance, e);
    return EXIT_FAILURE;
}
