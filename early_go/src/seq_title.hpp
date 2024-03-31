#pragma once

#include "stdafx.hpp"
#include "seq_interface.hpp"

namespace early_go
{
class sprite;
class seq_title : public seq_interface
{
public:
    seq_title(const std::shared_ptr<IDirect3DDevice9>& d3d_device);
    virtual void update() override;
    virtual void render() override;
private:
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::shared_ptr<sprite> sprite_;
};
}
