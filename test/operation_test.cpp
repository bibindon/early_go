
#pragma comment (lib, "DxErr.lib")

#include "pch.h"
#include "src/operation.hpp"
#include "character_mock.hpp"
#include "src/se_player.hpp"
#include "src/stage.hpp"

using std::shared_ptr;

// FRIEND_TEST macro needs same namespace.
namespace early_go
{
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

HWND get_window_handle()
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
        return nullptr;
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
        return nullptr;
    }
    return hWnd;
}
/*
TEST(operation_test, operation_test01)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, {0, 0, 2}, direction::BACK, 1.0f } };
    shared_ptr<stage> _stage { new stage { } };

    {
        // Set keyboard state for this test.
        keyboard::key_deque_.clear();
        {
            keyboard::key_info _key_info = { 0 };
            keyboard::key_deque_.push_front(_key_info);
        }
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should be called with direction::FRONT of argument.
        EXPECT_CALL(*_chara_mock, set_action_step(direction::FRONT)).Times(1);

        // Test operation::operator
        ope->update(_early, _enemy, _stage);
    }
}

TEST(operation_test, operation_test02)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, {0, 0, 2}, direction::BACK, 1.0f } };
    shared_ptr<stage> _stage { new stage { } };

    {
        // Set keyboard state for this test.
        keyboard::key_deque_.clear();
        {
            keyboard::key_info _key_info = { 0 };
            keyboard::key_deque_.push_front(_key_info);
        }
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['A'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should be called.
        EXPECT_CALL(*_chara_mock, set_action_step(direction::LEFT)).Times(1);

        // Test operation::operator
        ope->update(_early, _enemy, _stage);
    }
}

TEST(operation_test, operation_test03)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, {0, 0, 2}, direction::BACK, 1.0f } };
    shared_ptr<stage> _stage { new stage { } };

    {
        // Set keyboard state for this test.
        keyboard::key_deque_.clear();
        {
            keyboard::key_info _key_info = { 0 };
            keyboard::key_deque_.push_front(_key_info);
        }
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['S'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should be called.
        EXPECT_CALL(*_chara_mock, set_action_step(direction::BACK)).Times(1);

        // Test operation::operator
        ope->update(_early, _enemy, _stage);
    }
}

TEST(operation_test, operation_test04)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, {0, 0, 2}, direction::BACK, 1.0f } };
    shared_ptr<stage> _stage { new stage { } };

    {
        // Set keyboard state for this test.
        keyboard::key_deque_.clear();
        {
            keyboard::key_info _key_info = { 0 };
            keyboard::key_deque_.push_front(_key_info);
        }
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['D'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should be called.
        EXPECT_CALL(*_chara_mock, set_action_step(direction::RIGHT)).Times(1);

        // Test operation::operator
        ope->update(_early, _enemy, _stage);
    }
}
TEST(operation_test, operation_test05)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, {0, 0, 2}, direction::BACK, 1.0f } };
    shared_ptr<stage> _stage { new stage { } };

    {
        // Set keyboard state for this test.
        keyboard::key_deque_.clear();
        {
            keyboard::key_info _key_info = { 0 };
            keyboard::key_deque_.push_front(_key_info);
        }
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            _key_info.key_table_['I'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should be called.
        EXPECT_CALL(*_chara_mock,
            set_action_step_and_rotate(direction::FRONT, direction::FRONT)).Times(1);

        // Test operation::operator
        ope->update(_early, _enemy, _stage);
    }
}
TEST(operation_test, operation_test06)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, {0, 0, 2}, direction::BACK, 1.0f } };
    shared_ptr<stage> _stage { new stage { } };

    {
        // Set keyboard state for this test.
        keyboard::key_deque_.clear();
        {
            keyboard::key_info _key_info = { 0 };
            keyboard::key_deque_.push_front(_key_info);
        }
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }
        ope->update(_early, _enemy, _stage);
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['I'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should be called.
        EXPECT_CALL(*_chara_mock,
            set_action_step_and_rotate(direction::FRONT, direction::FRONT)).Times(1);

        // Test operation::operator
        ope->update(_early, _enemy, _stage);
    }
}
TEST(operation_test, operation_test07)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, {0, 0, 2}, direction::BACK, 1.0f } };
    shared_ptr<stage> _stage { new stage { } };

    {
        // Set keyboard state for this test.
        keyboard::key_deque_.clear();
        {
            keyboard::key_info _key_info = { 0 };
            keyboard::key_deque_.push_front(_key_info);
        }
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['I'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }
        ope->update(_early, _enemy, _stage);
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should be called.
        EXPECT_CALL(*_chara_mock,
            set_action_step_and_rotate(direction::FRONT, direction::FRONT)).Times(1);

        // Test operation::operator
        ope->update(_early, _enemy, _stage);
    }
}
TEST(operation_test, operation_test08)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, {0, 0, 2}, direction::BACK, 1.0f } };
    shared_ptr<stage> _stage { new stage { } };

    {
        // Set keyboard state for this test.
        keyboard::key_deque_.clear();
        {
            keyboard::key_info _key_info = { 0 };
            keyboard::key_deque_.push_front(_key_info);
        }
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['I'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }
        ope->update(_early, _enemy, _stage);
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }
        ope->update(_early, _enemy, _stage);
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['D'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should not be called.
        EXPECT_CALL(*_chara_mock,
            set_action_step_and_rotate(direction::RIGHT, direction::FRONT)).Times(0);

        // Test operation::operator
        ope->update(_early, _enemy, _stage);
    }
}
TEST(operation_test, operation_test09)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, {0, 0, 2}, direction::BACK, 1.0f } };
    shared_ptr<stage> _stage { new stage { } };

    {
        // Set keyboard state for this test.
        keyboard::key_deque_.clear();
        {
            keyboard::key_info _key_info = { 0 };
            keyboard::key_deque_.push_front(_key_info);
        }
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }
        ope->update(_early, _enemy, _stage);
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should not be called.
        EXPECT_CALL(*_chara_mock, set_action_step(direction::FRONT)).Times(0);

        // Test operation::operator
        ope->update(_early, _enemy, _stage);
    }
}
TEST(operation_test, operation_test10)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, {0, 0, 2}, direction::BACK, 1.0f } };
    shared_ptr<stage> _stage { new stage { } };

    HWND hwnd = get_window_handle();
    se_player::initialize(hwnd);
    se_player::get_ton()->load("sound/attack01.wav");

    {
        // Set keyboard state for this test.
        keyboard::key_deque_.clear();
        {
            keyboard::key_info _key_info = { 0 };
            keyboard::key_deque_.push_front(_key_info);
        }
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }
        ope->update(_early, _enemy, _stage);
        {
            keyboard::key_info _key_info = { 0 };
            _key_info.key_table_['A'] = 0x0001;
            keyboard::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should not be called.
        EXPECT_CALL(*_chara_mock, set_action_step(direction::FRONT)).Times(0);
        EXPECT_CALL(*_chara_mock, set_action_step(direction::LEFT)).Times(0);
        EXPECT_CALL(*_chara_mock, set_action_attack()).Times(1);

        // Test operation::operator
        ope->update(_early, _enemy, _stage);
    }

    se_player::finalize();
}
*/
}
