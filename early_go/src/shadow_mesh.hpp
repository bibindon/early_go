#include "stdafx.hpp"
#include "z_tex.hpp"
#include "depth_buf_shadow.hpp"

namespace early_go
{
class shadow_mesh
{
public:
    std::shared_ptr<z_tex> ztex_ { nullptr };
    std::shared_ptr<depth_buf_shadow> depth_buf_shadow_ { nullptr };

    shadow_mesh(
        const std::shared_ptr<IDirect3DDevice9>& d3d_device,
        const std::shared_ptr<z_tex>& ztex_,
        const std::shared_ptr<depth_buf_shadow>& _depth_buf_shadow,
        const std::string& name,
        const D3DXVECTOR3& pos,
        const D3DXVECTOR3& rotate,
        const float scale
    );
    void update();
    void ztex_render();
    void render();
    void set_position(const D3DXVECTOR3& pos);
    void set_rotate(const D3DXVECTOR3& rotate);
    void set_scale(const float scale);
    std::string x_filename();
private:
    DWORD materials_count_ { 0 };
    std::shared_ptr<IDirect3DDevice9> d3d_device_ { };
    std::vector<std::shared_ptr<IDirect3DTexture9> > textures_ { };
    std::array<std::shared_ptr<IDirect3DTexture9>, depth_buf_shadow::LAYER> dynamic_textures_ { };
    std::shared_ptr<ID3DXMesh> d3dx_mesh_ { };
    D3DXHANDLE texture_handle_ { nullptr };
    D3DXVECTOR3 pos_ { 0.0f, 0.0f, 0.0f };
    D3DXVECTOR3 rotate_ { 0.0f, 0.0f, 0.0f };
    float scale_ { 1.0f };
    std::string x_filename_ { "" };
};
}

