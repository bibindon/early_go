
#pragma comment (lib, "DxErr.lib")

#include "pch.h"
#include "src/operation.hpp"
#include "character_mock.hpp"

using std::shared_ptr;

// FRIEND_TEST macro needs same namespace.
namespace early_go
{
TEST(operation_test, operation_test01)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, ope, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, ope, {0, 0, 2}, direction::BACK, 1.0f } };

    {
        // Set keyboard state for this test.
        key::key_deque_.clear();
        {
            key::key_info _key_info = { 0 };
            key::key_deque_.push_front(_key_info);
        }
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should be called with direction::FRONT of argument.
        EXPECT_CALL(*_chara_mock, set_action_step(direction::FRONT)).Times(1);

        // Test operation::operator
        (*ope)(_early, _enemy);
    }
}

TEST(operation_test, operation_test02)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, ope, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, ope, {0, 0, 2}, direction::BACK, 1.0f } };

    {
        // Set keyboard state for this test.
        key::key_deque_.clear();
        {
            key::key_info _key_info = { 0 };
            key::key_deque_.push_front(_key_info);
        }
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['A'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should be called.
        EXPECT_CALL(*_chara_mock, set_action_step(direction::LEFT)).Times(1);

        // Test operation::operator
        (*ope)(_early, _enemy);
    }
}

TEST(operation_test, operation_test03)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, ope, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, ope, {0, 0, 2}, direction::BACK, 1.0f } };

    {
        // Set keyboard state for this test.
        key::key_deque_.clear();
        {
            key::key_info _key_info = { 0 };
            key::key_deque_.push_front(_key_info);
        }
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['S'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should be called.
        EXPECT_CALL(*_chara_mock, set_action_step(direction::BACK)).Times(1);

        // Test operation::operator
        (*ope)(_early, _enemy);
    }
}

TEST(operation_test, operation_test04)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, ope, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, ope, {0, 0, 2}, direction::BACK, 1.0f } };

    {
        // Set keyboard state for this test.
        key::key_deque_.clear();
        {
            key::key_info _key_info = { 0 };
            key::key_deque_.push_front(_key_info);
        }
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['D'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should be called.
        EXPECT_CALL(*_chara_mock, set_action_step(direction::RIGHT)).Times(1);

        // Test operation::operator
        (*ope)(_early, _enemy);
    }
}
TEST(operation_test, operation_test05)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, ope, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, ope, {0, 0, 2}, direction::BACK, 1.0f } };

    {
        // Set keyboard state for this test.
        key::key_deque_.clear();
        {
            key::key_info _key_info = { 0 };
            key::key_deque_.push_front(_key_info);
        }
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            _key_info.key_table_['I'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should be called.
        EXPECT_CALL(*_chara_mock,
            set_action_step_and_rotate(direction::FRONT, direction::FRONT)).Times(1);

        // Test operation::operator
        (*ope)(_early, _enemy);
    }
}
TEST(operation_test, operation_test06)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, ope, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, ope, {0, 0, 2}, direction::BACK, 1.0f } };

    {
        // Set keyboard state for this test.
        key::key_deque_.clear();
        {
            key::key_info _key_info = { 0 };
            key::key_deque_.push_front(_key_info);
        }
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }
        (*ope)(_early, _enemy);
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['I'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should be called.
        EXPECT_CALL(*_chara_mock,
            set_action_step_and_rotate(direction::FRONT, direction::FRONT)).Times(1);

        // Test operation::operator
        (*ope)(_early, _enemy);
    }
}
TEST(operation_test, operation_test07)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, ope, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, ope, {0, 0, 2}, direction::BACK, 1.0f } };

    {
        // Set keyboard state for this test.
        key::key_deque_.clear();
        {
            key::key_info _key_info = { 0 };
            key::key_deque_.push_front(_key_info);
        }
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['I'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }
        (*ope)(_early, _enemy);
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should be called.
        EXPECT_CALL(*_chara_mock,
            set_action_step_and_rotate(direction::FRONT, direction::FRONT)).Times(1);

        // Test operation::operator
        (*ope)(_early, _enemy);
    }
}
TEST(operation_test, operation_test08)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, ope, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, ope, {0, 0, 2}, direction::BACK, 1.0f } };

    {
        // Set keyboard state for this test.
        key::key_deque_.clear();
        {
            key::key_info _key_info = { 0 };
            key::key_deque_.push_front(_key_info);
        }
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['I'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }
        (*ope)(_early, _enemy);
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }
        (*ope)(_early, _enemy);
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['D'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should not be called.
        EXPECT_CALL(*_chara_mock,
            set_action_step_and_rotate(direction::RIGHT, direction::FRONT)).Times(0);

        // Test operation::operator
        (*ope)(_early, _enemy);
    }
}
TEST(operation_test, operation_test09)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, ope, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, ope, {0, 0, 2}, direction::BACK, 1.0f } };

    {
        // Set keyboard state for this test.
        key::key_deque_.clear();
        {
            key::key_info _key_info = { 0 };
            key::key_deque_.push_front(_key_info);
        }
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }
        (*ope)(_early, _enemy);
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should not be called.
        EXPECT_CALL(*_chara_mock, set_action_step(direction::FRONT)).Times(0);

        // Test operation::operator
        (*ope)(_early, _enemy);
    }
}
TEST(operation_test, operation_test10)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock = new character_mock { d3d_device, ope, { 0, 0, 0 }, direction::NONE, 0.0f };
    shared_ptr<character> _early { _chara_mock };
    shared_ptr<character> _enemy { new character{d3d_device, ope, {0, 0, 2}, direction::BACK, 1.0f } };

    {
        // Set keyboard state for this test.
        key::key_deque_.clear();
        {
            key::key_info _key_info = { 0 };
            key::key_deque_.push_front(_key_info);
        }
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['W'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }
        (*ope)(_early, _enemy);
        {
            key::key_info _key_info = { 0 };
            _key_info.key_table_['A'] = 0x0001;
            key::key_deque_.push_front(_key_info);
        }

        // "set_action_step" should not be called.
        EXPECT_CALL(*_chara_mock, set_action_step(direction::FRONT)).Times(0);
        EXPECT_CALL(*_chara_mock, set_action_step(direction::LEFT)).Times(0);
        EXPECT_CALL(*_chara_mock, set_action_attack()).Times(1);

        // Test operation::operator
        (*ope)(_early, _enemy);
    }
}
}
