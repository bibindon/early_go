#include "stdafx.hpp"
#include "hud2.hpp"

using std::shared_ptr;
using std::string;
using std::vector;

namespace early_go
{
hud2::hud2(const shared_ptr<IDirect3DDevice9>& d3d_device)
    : d3d_device_ { d3d_device }
{
}
void hud2::operator()()
{
}
}
