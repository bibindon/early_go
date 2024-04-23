#pragma once

#include "stdafx.hpp"

namespace early_go
{
struct vec3i
{
    int x { 0 };
    int y { 0 };
    int z { 0 };
};
class stage
{
public:
    stage();
    std::unordered_map<int, std::unordered_map<int, bool>> offensive_area();
    void set_offensive_area(std::unordered_map<int, std::unordered_map<int, bool>> area);
    void set_offensive_position(const int& x, const int& z);
    void clear();

private:
    std::unordered_map<int, std::unordered_map<int, bool>> offensive_area_ { };
};
}
