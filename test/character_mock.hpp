#pragma once

#include <gmock/gmock.h>
#include "pch.h"
#include "src/character.hpp"

class character_mock : public early_go::character
{
public:
    character_mock(
        const std::shared_ptr<IDirect3DDevice9>& a1,
        const cv::Point3i& a2,
        const early_go::direction& a3,
        const float& a4)
        : character { a1, a2, a3, a4 }
    {
    }
    MOCK_METHOD(void, set_action_step, (const early_go::direction));
    MOCK_METHOD(void, set_action_rotate, (const early_go::direction));
    MOCK_METHOD(void, set_action_step_and_rotate, (const early_go::direction, const early_go::direction));
    MOCK_METHOD(void, set_action_attack, ());
};

