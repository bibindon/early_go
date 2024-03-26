#pragma once

#include <gmock/gmock.h>
#include "pch.h"
#include "src/character.hpp"

class character_mock : public early_go::character
{
public:
    character_mock(
        const std::shared_ptr<IDirect3DDevice9>& a1,
        const std::shared_ptr<early_go::operation>& a2,
        const cv::Point3i& a3,
        const early_go::direction& a4,
        const float& a5)
        : character { a1, a2, a3, a4, a5 }
    {
    }
    MOCK_METHOD(void, set_action_step, (const early_go::direction));
};

