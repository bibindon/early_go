#ifndef MESH_HPP
#define MESH_HPP
#include "stdafx.hpp"
#include "abstract_mesh.hpp"

namespace early_go
{
class mesh : public abstract_mesh
{
public:
    mesh(
        const std::shared_ptr<IDirect3DDevice9>&,
        const std::string&,
        const D3DXVECTOR3&,
        const D3DXVECTOR3&,
        const float&);
    ~mesh() override;

private:
    const static std::string SHADER_FILENAME;
    std::unique_ptr<ID3DXMesh, custom_deleter> d3dx_mesh_;
    DWORD materials_count_;
    D3DXHANDLE world_view_proj_handle_;
    std::vector<D3DCOLORVALUE> colors_;
    std::vector<std::unique_ptr<IDirect3DTexture9, custom_deleter>> textures_;
    D3DXVECTOR3 center_coodinate_;
    float radius_;
    float scale_;
    std::string mesh_name_;

    void render_impl(const D3DXMATRIX &, const D3DXMATRIX &) override;
};
} // namespace early_go 
#endif
