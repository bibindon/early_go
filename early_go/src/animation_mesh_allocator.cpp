#include "stdafx.hpp"

#include "animation_mesh_allocator.hpp"

namespace early_go {
/* c'tor */
animation_mesh_frame::animation_mesh_frame(const std::string& a_kr_name)
    : D3DXFRAME{}, /* Initializes member with zero. */
      combined_transformation_matrix_{}
{
  /*
   * The 'dup' of the '::_strdup' is the abbreviation of 'duplicate', create
   * the new string by the argument string.
   */
  this->Name = ::_strdup(a_kr_name.c_str());

  /* Make an argument an identity matrix. */
  ::D3DXMatrixIdentity(&this->TransformationMatrix);
  ::D3DXMatrixIdentity(&this->combined_transformation_matrix_);
}

/*
 * A constructor which only initializes member variables from the beginning to
 * the end.
 *
 * References:
 *
 *  // d3dx9anim.h
 *  typedef struct _D3DXMESHCONTAINER
 *  {
 *      LPSTR                   Name;
 *
 *      D3DXMESHDATA            MeshData;
 *
 *      LPD3DXMATERIAL          pMaterials;
 *      LPD3DXEFFECTINSTANCE    pEffects;
 *      DWORD                   NumMaterials;
 *      DWORD                  *pAdjacency;
 *
 *      LPD3DXSKININFO          pSkinInfo;
 *
 *      struct _D3DXMESHCONTAINER *pNextMeshContainer;
 *  } D3DXMESHCONTAINER, *LPD3DXMESHCONTAINER;
 *
 */
animation_mesh_container::animation_mesh_container(
    const std::string&    a_krsz_x_filename,
    const std::string&    a_krsz_meshname,
    ::LPD3DXMESH          a_p_d3dx_mesh,
    const ::D3DXMATERIAL* a_kp_materials,
    const ::DWORD         a_k_materials_number,
    const ::DWORD*        a_kp_adjacency)
    : D3DXMESHCONTAINER{}, /* Initializes with zero. */
      vecup_texture_{}
{
  /*
   * The 'dup' of '::_strdup' is the abbreviation of 'duplicate', create new
   * string by the argument string.
   */
  this->Name = ::_strdup(a_krsz_meshname.c_str());

  ::LPDIRECT3DDEVICE9 _p_temp_direct3d_device9{nullptr};
  a_p_d3dx_mesh->GetDevice(&_p_temp_direct3d_device9);

  /*
   * This IF sentence is just initializing the 'MeshData' of a member variable.
   * When this mesh doesn't have normal vector, add it.
   */
  if (!(a_p_d3dx_mesh->GetFVF() & D3DFVF_NORMAL)) {
    this->MeshData.Type = ::D3DXMESHTYPE_MESH;
    ::HRESULT _hresult{
        a_p_d3dx_mesh->CloneMeshFVF(a_p_d3dx_mesh->GetOptions(),
                                    a_p_d3dx_mesh->GetFVF() | D3DFVF_NORMAL,
                                    _p_temp_direct3d_device9,
                                    &this->MeshData.pMesh)};
    if (FAILED(_hresult)) {
      BOOST_THROW_EXCEPTION(
          custom_exception{"Failed 'CloneMeshFVF' function."});
    }
    a_p_d3dx_mesh = this->MeshData.pMesh;
    ::D3DXComputeNormals(a_p_d3dx_mesh, nullptr);
  } else {
    this->MeshData.pMesh = a_p_d3dx_mesh;
    this->MeshData.Type = ::D3DXMESHTYPE_MESH;
    a_p_d3dx_mesh->AddRef();
  }

  /* This strange bracket is measures of being interpretered as WinAPI macro. */
  this->NumMaterials   = (std::max)(1UL, a_k_materials_number);
  this->pMaterials     = new_crt ::D3DXMATERIAL[this->NumMaterials]{};
  std::vector<std::unique_ptr<::IDirect3DTexture9, custom_deleter> >
      _temp_texture(this->NumMaterials);
  this->vecup_texture_.swap(_temp_texture);

  /* Initialize the 'pAdjacency' of a member variable. */
  unsigned int _ui_faces_amount{a_p_d3dx_mesh->GetNumFaces()};
  this->pAdjacency = new_crt ::DWORD[_ui_faces_amount * 3]{};

  for (unsigned int i{}; i < _ui_faces_amount * 3; ++i) {
    this->pAdjacency[i] = a_kp_adjacency[i];
  }

  /*
   * Initialize the 'pMaterials' and the 'vecup_texture_' of member variables
   * if there are.
   */
  if (a_k_materials_number > 0) {
    for (unsigned int i{}; i < a_k_materials_number; ++i) {
      this->pMaterials[i] = a_kp_materials[i];
    }

    for (unsigned int i{}; i < a_k_materials_number; ++i) {
      this->pMaterials[i].MatD3D.Ambient = ::D3DCOLORVALUE{0.2f, 0.2f, 0.2f, 0};
      if (this->pMaterials[i].pTextureFilename != nullptr) {
        std::string _query;
        _query = "select data from texture where filename = '";
        _query += this->pMaterials[i].pTextureFilename;
        _query += "' and x_filename = '";
        _query += a_krsz_x_filename + "';";

        std::vector<char> _data = get_resource(_query);
        ::LPDIRECT3DTEXTURE9 _pp_temp_texture{};
        if (FAILED(
            ::D3DXCreateTextureFromFileInMemory(_p_temp_direct3d_device9,
                                                &_data[0],
                                                static_cast<UINT>(_data.size()),
                                                &_pp_temp_texture))) {
          BOOST_THROW_EXCEPTION(custom_exception{"texture file is not found."});
        } else {
          this->vecup_texture_.at(i).reset(_pp_temp_texture);
        }
      }
    }
  } else {
    this->pMaterials[0].MatD3D.Diffuse = ::D3DCOLORVALUE{0.5f, 0.5f, 0.5f, 0};
    this->pMaterials[0].MatD3D.Ambient = ::D3DCOLORVALUE{0.5f, 0.5f, 0.5f, 0};
    this->pMaterials[0].MatD3D.Specular = this->pMaterials[0].MatD3D.Diffuse;
  }
}

animation_mesh_allocator::animation_mesh_allocator(
    const std::string & a_krsz_x_filename)
    : ID3DXAllocateHierarchy{},
      x_filename_(a_krsz_x_filename) {}

/*
 * Alghough it's camel case and a strange type name, because this function is a
 * pure virtual function of 'ID3DXAllocateHierarchy'.
 */
::STDMETHODIMP animation_mesh_allocator::CreateFrame(
    ::LPCTSTR a_name, ::LPD3DXFRAME *a_pp_new_frame)
{
  void* p = _aligned_malloc_crt(sizeof(animation_mesh_frame), 16);
  *a_pp_new_frame = new(p) animation_mesh_frame{a_name};
  return S_OK;
}

/*
 * Alghough it's camel case and a strange type name, because this function is a
 * pure virtual function of 'ID3DXAllocateHierarchy'.
 */
::STDMETHODIMP animation_mesh_allocator::CreateMeshContainer(
    ::LPCSTR a_kr_meshname,
    CONST ::D3DXMESHDATA* a_kp_mesh_data,
    CONST ::D3DXMATERIAL* a_kp_materials,
    CONST ::D3DXEFFECTINSTANCE*,
    ::DWORD a_materials_number,
    CONST ::DWORD *a_kp_adjacency,
    ::LPD3DXSKININFO,
    ::LPD3DXMESHCONTAINER *a_pp_mesh_container)
{
  try {
    *a_pp_mesh_container =
        new_crt animation_mesh_container{this->x_filename_,
                                         a_kr_meshname,
                                         a_kp_mesh_data->pMesh,
                                         a_kp_materials,
                                         a_materials_number,
                                         a_kp_adjacency};
  } catch (const std::exception& a_kr_expception) {
    early_go::log_liner{} << boost::diagnostic_information(a_kr_expception);
    return E_FAIL;
  }
  return S_OK;
}

/*
 * Alghough it's camel case and a strange type name, because this function is a
 * pure virtual function of 'ID3DXAllocateHierarchy'.
 */
::STDMETHODIMP animation_mesh_allocator::DestroyFrame(
    ::LPD3DXFRAME a_p_frame_to_free)
{
  safe_delete_array(a_p_frame_to_free->Name);
  a_p_frame_to_free->~D3DXFRAME();
  _aligned_free(a_p_frame_to_free);
  return S_OK;
}

/*
 * Alghough it's camel case and a strange type name, because this function is a
 * pure virtual function of 'ID3DXAllocateHierarchy'.
 */
::STDMETHODIMP animation_mesh_allocator::DestroyMeshContainer(
    ::LPD3DXMESHCONTAINER a_p_mesh_container_base)
{
  animation_mesh_container *_p_mesh_container{
      static_cast<animation_mesh_container*>(a_p_mesh_container_base)};

  safe_release(_p_mesh_container->pSkinInfo);
  safe_delete_array(_p_mesh_container->Name);
  safe_delete_array(_p_mesh_container->pAdjacency);
  safe_delete_array(_p_mesh_container->pMaterials);
  safe_release(_p_mesh_container->MeshData.pMesh);
  safe_delete(_p_mesh_container);

  return S_OK;
}
} /* namespace early_go */
