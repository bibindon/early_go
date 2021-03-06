#ifndef SKINNED_ANIMATION_MESH_ALLOCATOR_HPP
#define SKINNED_ANIMATION_MESH_ALLOCATOR_HPP
#include "stdafx.hpp"

namespace early_go {
/*
 * A struct inheriting the '::D3DXFRAME' for owing a transform matrix.
 */
struct skinned_animation_mesh_frame : public ::D3DXFRAME
{
  ::D3DXMATRIX combined_matrix_;
  explicit skinned_animation_mesh_frame(const std::string&);
};

/*
 * A struct inheriting the '::D3DXMESHCONTAINER' for owing textures.
 */
struct skinned_animation_mesh_container : public ::D3DXMESHCONTAINER
{
  std::vector<
      std::unique_ptr<
          ::IDirect3DTexture9, custom_deleter
      >
  > texture_;

  ::DWORD                                        palette_size_;
  ::DWORD                                        influence_count_;
  ::DWORD                                        bone_count_;
  std::unique_ptr<::ID3DXBuffer, custom_deleter> bone_buffer_;
  std::vector<::LPD3DXMATRIX>                    frame_combined_matrix_;
  std::vector<::D3DXMATRIX>                      bone_offset_matrices_;

  skinned_animation_mesh_container(const std::string&,
                                   const std::string&,
                                   ::LPD3DXMESH,
                                   const ::D3DXMATERIAL*,
                                   const ::DWORD,
                                   const ::DWORD*,
                                   ::LPD3DXSKININFO);
  void initialize_materials(const DWORD&,
                            const D3DXMATERIAL*,
                            const std::string&,
                            const LPDIRECT3DDEVICE9&);
  void initialize_bone(const ::LPD3DXSKININFO&, const ::LPD3DXMESH&);
  void initialize_FVF(const ::LPDIRECT3DDEVICE9&);
  void initialize_vertex_element();
};

/*
 * A class inheriting the '::ID3DXAllocateHierarchy' for implementing an
 * animation mesh.
 */
class skinned_animation_mesh_allocator : public ::ID3DXAllocateHierarchy
{
public:
  skinned_animation_mesh_allocator(const std::string&);
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
private:
  std::string x_filename_;
};
} /* namespace early_go */
#endif
