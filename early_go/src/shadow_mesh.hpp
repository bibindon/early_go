#include "stdafx.hpp"
#include "z_tex.hpp"
#include "depth_buf_shadow.hpp"

namespace early_go
{
class shadow_mesh
{
public:
    z_tex* ZTexCreator_ { nullptr };
    depth_buf_shadow* DepthBS_ { nullptr };

    shadow_mesh(
        const std::shared_ptr<IDirect3DDevice9>& d3d_device,
        z_tex* ZTexCreator,
        depth_buf_shadow* DepthBS,
        const std::string& name,
        const D3DXVECTOR3& pos,
        const D3DXVECTOR3& rotate,
        const float scale
    );
    void update();
    void ztex_render();
    void render(const D3DXMATRIX& view_matrix, const D3DXMATRIX& projection_matrix);
    DWORD materials_count_ { 0 };
    std::shared_ptr<IDirect3DDevice9> d3d_device_ { };
    std::vector<std::shared_ptr<IDirect3DTexture9>> textures_ { };
    std::shared_ptr<IDirect3DTexture9> dynamic_textures_ { };
    std::shared_ptr<ID3DXMesh> d3dx_mesh_ { };
    D3DXHANDLE texture_handle_ { nullptr };
};
}

