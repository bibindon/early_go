#pragma once

#include "stdafx.hpp"

namespace early_go
{
// postpone
class mesh_interface
{
    virtual void update() = 0;
    virtual void render() = 0;
    virtual ~mesh_interface() = 0;
};
// postpone
class shadow_mesh_interface : public mesh_interface
{
    virtual void ztex_render() = 0;
    virtual ~shadow_mesh_interface() = 0;
};
}
