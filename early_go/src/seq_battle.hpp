
#pragma once

#include "stdafx.hpp"
#include "seq_interface.hpp"

namespace early_go
{
class operation;
class sprite;
class seq_battle : public seq_interface
{
public:
    seq_battle(
        const std::shared_ptr<IDirect3DDevice9>& d3d_device,
        const std::shared_ptr<camera>& _camera);
    virtual seq_type update(
        const std::shared_ptr<character>& _early,
        const std::shared_ptr<character>& _suo,
        const std::shared_ptr<camera>& _camera) override;
    virtual void render(
        const std::shared_ptr<character>& _early,
        const std::shared_ptr<character>& _suo,
        const std::shared_ptr<camera>& _camera) override;
private:
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::shared_ptr<operation> operation_ { nullptr };
};
}

