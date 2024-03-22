#ifndef SKINNED_ANIMATION_MESH_HPP
#define SKINNED_ANIMATION_MESH_HPP
#include "stdafx.hpp"
#include "abstract_mesh.hpp"

namespace early_go
{

class skinned_animation_mesh_allocator;

// A class that provides operations for a mesh file having animations.
class skinned_animation_mesh : public abstract_mesh
{
public:
    skinned_animation_mesh(
        const std::shared_ptr<IDirect3DDevice9> &,
        const std::string &,
        const D3DXVECTOR3 &,
        const D3DXVECTOR3 &,
        const float &);
    ~skinned_animation_mesh() override;

private:
    void render_impl(const D3DXMATRIX &, const D3DXMATRIX &) override;
    // A struct which deletes the root 'LPD3DXFRAME' object. The reason why the
    // following struct is not a prototype declaration is necessary for the
    // declaration of the following variable using std::unique_ptr.
    struct frame_root_deleter_object
    {
        std::shared_ptr<skinned_animation_mesh_allocator> allocator_;
        void operator()(const LPD3DXFRAME);
        void release_mesh_allocator(const LPD3DXFRAME);
    };

    const static std::string SHADER_FILENAME;
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::shared_ptr<skinned_animation_mesh_allocator> allocator_;
    std::unique_ptr<D3DXFRAME, frame_root_deleter_object> frame_root_;
    D3DXMATRIX rotation_matrix_;
    std::vector<D3DXMATRIX> world_matrix_array_;
    D3DXVECTOR3 center_coodinate_;
    float scale_;

    // For effect.
    D3DXHANDLE view_projection_handle_;
    D3DXHANDLE scale_handle_;

    void update_frame_matrix(const LPD3DXFRAME, const LPD3DXMATRIX);
    void render_frame(const LPD3DXFRAME);
    void render_mesh_container(const LPD3DXMESHCONTAINER);

    HRESULT allocate_bone_matrix(LPD3DXMESHCONTAINER);
    HRESULT allocate_all_bone_matrices(LPD3DXFRAME);
};
} // namespace early_go 
#endif
