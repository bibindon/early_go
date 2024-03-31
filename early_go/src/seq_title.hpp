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
    std::shared_ptr<sprite> title01_;
    std::shared_ptr<sprite> title02_;
    std::shared_ptr<sprite> title03_;
    D3DXVECTOR3 cursor_pos_ { 0.0f, 0.0f, 0.0f };
    int select_ { 0 };
    int counter_ { 0 };
    bool bgm_playing_ { false };
};
}
