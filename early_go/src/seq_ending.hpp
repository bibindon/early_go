#pragma once

#include "stdafx.hpp"
#include "seq_interface.hpp"

namespace early_go
{
class operation;
class sprite;
class seq_ending: public seq_interface
{
public:
    seq_ending(
        const std::shared_ptr<IDirect3DDevice9>& d3d_device,
        const std::shared_ptr<camera>& _camera);
    virtual seq_type update(
        const std::shared_ptr<player>& _early,
        const std::shared_ptr<camera>& _camera) override;
    virtual void render(
        const std::shared_ptr<player>& _early,
        const std::shared_ptr<camera>& _camera) override;
private:
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::shared_ptr<sprite> background_;
    std::shared_ptr<sprite> confirmation_;
    D3DXVECTOR3 background_pos_ { 0.0f, 0.0f, 0.0f };
    int counter_ { 0 };
    bool bgm_played_ { false };
    bool confirmation_shown_ { false };
};
}
