
#pragma comment (lib, "DxErr.lib")

#include "pch.h"
#include "src/mesh.hpp"

TCHAR gName[100] = "初期化サンプルプログラム";
LRESULT CALLBACK WndProc(HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam)
{
    if (mes == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, mes, wParam, lParam);
}
TEST(mesh_test, mesh_ctor_test)
{
    HINSTANCE hInstance = nullptr;
    HWND hWnd;
    WNDCLASSEX wcex = {
        sizeof(WNDCLASSEX),
        CS_HREDRAW | CS_VREDRAW,
        WndProc,
        0,
        0,
        hInstance,
        NULL,
        NULL,
        (HBRUSH)(COLOR_WINDOW + 1),
        NULL,
        (TCHAR*)gName,
        NULL };
    if (!RegisterClassEx(&wcex))
    {
        GTEST_FAIL();
        return;
    }

    int w = 640;
    int h = 480;
    RECT clientRect = { 0, 0, w, h };
    ::AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, FALSE);

    if (!(hWnd = CreateWindow(
        gName,
        gName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        0,
        clientRect.right - clientRect.left,
        clientRect.bottom - clientRect.top,
        NULL,
        NULL,
        hInstance,
        NULL)))
    {
        GTEST_FAIL();
        return ;
    }

    LPDIRECT3D9 g_pD3D;
    LPDIRECT3DDEVICE9 g_pD3DDev;
    if (!(g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
    {
        GTEST_FAIL();
        return ;
    }

    D3DPRESENT_PARAMETERS d3dpp = {
        (UINT)w,
        (UINT)h,
        D3DFMT_UNKNOWN,
        0,
        D3DMULTISAMPLE_NONE,
        0,
        D3DSWAPEFFECT_DISCARD,
        NULL,
        TRUE,
        TRUE,
        D3DFMT_D24S8,
        0,
        0};

    if (FAILED(g_pD3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &g_pD3DDev)))
    {
        if (FAILED(g_pD3D->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            hWnd,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING,
            &d3dpp,
            &g_pD3DDev)))
        {
            if (FAILED(g_pD3D->CreateDevice(
                D3DADAPTER_DEFAULT,
                D3DDEVTYPE_REF,
                hWnd,
                D3DCREATE_HARDWARE_VERTEXPROCESSING,
                &d3dpp,
                &g_pD3DDev)))
            {
                if (FAILED(g_pD3D->CreateDevice(
                    D3DADAPTER_DEFAULT,
                    D3DDEVTYPE_REF,
                    hWnd,
                    D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                    &d3dpp,
                    &g_pD3DDev)))
                {
                    g_pD3D->Release();
                    GTEST_FAIL();
                    return ;
                }
            }
        }
    }

    ShowWindow(hWnd, SW_SHOW);

    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    d3d_device_.reset(g_pD3DDev, early_go::custom_deleter{});
    {
        early_go::mesh m(
                d3d_device_,
                "model/tiger/tiger.x",
                D3DXVECTOR3{2.5f, 2.0f, 1.0f},
                D3DXVECTOR3{0.0f, 0.0f, 0.0f},
                1.0f);
    }
    // CRT feature can be used only debug mode.
#ifdef DEBUG
    HANDLE hLog = CreateFile(
        "log.txt",
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, hLog);
    _CrtDumpMemoryLeaks();
    CloseHandle(hLog);

    std::ifstream ifs("log.txt");
    if (!ifs)
    {
        DeleteFile("log.txt");
        GTEST_FAIL();
    }
    std::string txt((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();
    BOOL b = DeleteFile("log.txt");
    if (!b || txt.empty())
    {
        GTEST_FAIL();
    }
    std::string::size_type pos = txt.find(".cpp");

    if (pos != std::string::npos)
    {
        GTEST_FAIL();
    }
#endif // DEBUG
}
