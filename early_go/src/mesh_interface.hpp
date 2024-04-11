#pragma once

#include "stdafx.hpp"

namespace early_go
{
// TODO
class mesh_interface
{
    virtual void update() = 0;
    virtual void render() = 0;
    virtual ~mesh_interface() = 0;
};
// TODO
class shadow_mesh_interface
{
    virtual void update() = 0;
    virtual void ztex_render() = 0;
    virtual void render() = 0;
    virtual ~shadow_mesh_interface() = 0;
};
}
