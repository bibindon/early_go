#ifndef ANIMATION_MESH_ALLOCATOR_HPP
#define ANIMATION_MESH_ALLOCATOR_HPP

#include "inline_macro.hpp"

namespace early_go {

/*
 * A struct inheriting the '::D3DXFRAME' for owing a transform matrix.
 */
struct animation_mesh_frame : public ::D3DXFRAME
{
  ::D3DXMATRIXA16 combined_transformation_matrix_;
  explicit animation_mesh_frame(const std::string&);
};

/*
 * A struct inheriting the '::D3DXMESHCONTAINER' for owing textures.
 */
struct animation_mesh_container : public ::D3DXMESHCONTAINER
{
  std::vector<
      std::unique_ptr<
          ::IDirect3DTexture9, custom_deleter
      >
  > vecup_texture_;
  animation_mesh_container(const std::string&,
                           ::LPD3DXMESH,
                           const ::D3DXMATERIAL*,
                           const ::DWORD,
                           const ::DWORD*);
};

/*
 * A class inheriting the '::ID3DXAllocateHierarchy' for implementing an
 * animation mesh.
 */
class animation_mesh_allocator : public ::ID3DXAllocateHierarchy
{
public:
  /* A function which be must defined. ~ */
  STDMETHOD(CreateFrame)(THIS_ ::LPCTSTR, ::LPD3DXFRAME*);
  STDMETHOD(CreateMeshContainer)(THIS_ ::LPCSTR,
                                 CONST ::D3DXMESHDATA*,
                                 CONST ::D3DXMATERIAL*,
                                 CONST ::D3DXEFFECTINSTANCE*,
                                 ::DWORD,
                                 CONST ::DWORD*,
                                 ::LPD3DXSKININFO,
                                 ::LPD3DXMESHCONTAINER*);
  STDMETHOD(DestroyFrame)(THIS_ ::LPD3DXFRAME);
  STDMETHOD(DestroyMeshContainer)(THIS_ ::LPD3DXMESHCONTAINER);
  /* ~ A function which be must defined. */
};
} /* namespace early_go */
#endif
