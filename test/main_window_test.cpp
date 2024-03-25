
#pragma comment (lib, "DxErr.lib")

#include "pch.h"
#include "src/main_window.hpp"

#include <thread>

void send_q_key()
{
    Sleep(15000);
    HWND hwnd = FindWindow(nullptr, "early_go");
    if (hwnd)
    {
        SendMessage(hwnd, WM_CLOSE, 0, 0);
    }
}

TEST(main_window_test, main_window_test)
{
    std::thread _thread(send_q_key);
    HINSTANCE hInstance = nullptr;
    early_go::main_window _window{ hInstance };
    _window();
    _thread.join();
}

