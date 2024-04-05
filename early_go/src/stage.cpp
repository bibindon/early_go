#include "stage.hpp"

namespace early_go
{
stage::stage()
{
    for (int z { 0 }; z < constants::GRID_NUM_HEIGHT; ++z)
    {
        for (int x { -(constants::GRID_NUM_WIDTH - 1) / 2 };
             x < (constants::GRID_NUM_WIDTH + 1) / 2; ++x)
        {
            offensive_area_[z][x] = false;
        }
    }
}
std::unordered_map<int, std::unordered_map<int, bool>> stage::offensive_area()
{
    return offensive_area_;
}
void stage::set_offensive_area(std::unordered_map<int, std::unordered_map<int, bool>> area)
{
    offensive_area_ = area;
}
void stage::set_offensive_position(const int &x, const int &z)
{
    const int current_stage_z { z % (constants::GRID_NUM_HEIGHT + 1) };
    // Check out of range(z).
    if (offensive_area_.find(current_stage_z) == offensive_area_.end())
    {
        return;
    }
    // Check out of range(x).
    if (offensive_area_.at(current_stage_z).find(x) == offensive_area_.at(current_stage_z).end())
    {
        return;
    }
    offensive_area_.at(current_stage_z).at(x) = true;
}
void stage::clear()
{
    for (auto &&z : offensive_area_)
    {
        for (auto &&x : z.second)
        {
            x.second = false;
        }
    }
}
}
