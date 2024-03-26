
#pragma comment (lib, "DxErr.lib")

#include "pch.h"
#include "src/operation.hpp"
#include "character_mock.hpp"

using std::shared_ptr;

// FRIEND_TEST macro needs same namespace.
namespace early_go
{
TEST(operation_test, operation_test)
{
    // Create character_moc object;
    shared_ptr<camera> camera { };
    main_window window { nullptr };
    shared_ptr<IDirect3DDevice9> d3d_device;
    shared_ptr<operation> ope { new operation { camera } };
    character_mock* _chara_mock =
        new character_mock 
        {
            d3d_device,
            ope,
            { 0, 0, 0 },
            direction::NONE,
            0.0f
        };

    // Set character_mock.
    window.early_.reset(_chara_mock);

    // "set_action_step" should be called with direction::FRONT of argument.
    EXPECT_CALL(*_chara_mock, set_action_step(direction::FRONT)).Times(1);

    // Set keyboard state for this test.
    {
        shared_ptr<key::key_info> _key_info{ new_crt key::key_info { } };
        key::key_deque_.push_front(_key_info);
    }
    {
        shared_ptr<key::key_info> _key_info{ new_crt key::key_info { } };
        _key_info->key_table_['W'] = 0x0001;
        key::key_deque_.push_front(_key_info);
    }

    // Test operation::operator
    (*ope)(window);
}
}
