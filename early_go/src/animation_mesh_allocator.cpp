#include "stdafx.hpp"

#include "animation_mesh_allocator.hpp"

namespace early_go {
/* c'tor */
animation_mesh_frame::animation_mesh_frame(const std::string& name)
    : D3DXFRAME{}, /* Initializes member with zero. */
      combined_matrix_{}
{
  /*
   * The 'dup' of the '::_strdup' is the abbreviation of 'duplicate', create
   * the new string by the argument string.
   */
  Name = ::_strdup(name.c_str());

  /* Make an argument an identity matrix. */
  ::D3DXMatrixIdentity(&TransformationMatrix);
  ::D3DXMatrixIdentity(&combined_matrix_);
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
    const std::string&    x_filename,
    const std::string&    mesh_name,
    ::LPD3DXMESH          mesh,
    const ::D3DXMATERIAL* materials,
    const ::DWORD         materials_count,
    const ::DWORD*        adjacency)
    : D3DXMESHCONTAINER{}, /* Initializes with zero. */
      texture_{}
{
  /*
   * The 'dup' of '::_strdup' is the abbreviation of 'duplicate', create new
   * string by the argument string.
   */
  Name = ::_strdup(mesh_name.c_str());

  ::LPDIRECT3DDEVICE9 temp_d3d_device{nullptr};
  mesh->GetDevice(&temp_d3d_device);

  /*
   * This IF sentence is just initializing the 'MeshData' of a member variable.
   * When this mesh doesn't have normal vector, add it.
   */
  if (!(mesh->GetFVF() & D3DFVF_NORMAL)) {
    MeshData.Type = ::D3DXMESHTYPE_MESH;
    ::HRESULT result{mesh->CloneMeshFVF(mesh->GetOptions(),
                                        mesh->GetFVF() | D3DFVF_NORMAL,
                                        temp_d3d_device,
                                        &MeshData.pMesh)};
    if (FAILED(result)) {
      THROW_WITH_TRACE("Failed 'CloneMeshFVF' function.");
    }
    mesh = MeshData.pMesh;
    ::D3DXComputeNormals(mesh, nullptr);
  } else {
    MeshData.pMesh = mesh;
    MeshData.Type = ::D3DXMESHTYPE_MESH;
    mesh->AddRef();
  }

  /* This strange bracket is measures of being interpretered as WinAPI macro. */
  NumMaterials = std::max(1UL, materials_count);
  pMaterials   = new_crt ::D3DXMATERIAL[NumMaterials];
  std::vector<std::unique_ptr<::IDirect3DTexture9, custom_deleter> >
      temp_texture(NumMaterials);
  texture_.swap(temp_texture);

  /* Initialize the 'pAdjacency' of a member variable. */
  uint64_t faces_count{mesh->GetNumFaces()};
  pAdjacency = new_crt ::DWORD[faces_count * 3];

  for (::DWORD i{}; i < faces_count * 3; ++i) {
    pAdjacency[i] = adjacency[i];
  }

  /*
   * Initialize the 'pMaterials' and the 'texture_' of member variables
   * if there are.
   */
  if (materials_count > 0) {
    for (::DWORD i{}; i < materials_count; ++i) {
      pMaterials[i] = materials[i];
    }

    for (::DWORD i{}; i < materials_count; ++i) {
      pMaterials[i].MatD3D.Ambient = ::D3DCOLORVALUE{0.2f, 0.2f, 0.2f, 0};
      if (pMaterials[i].pTextureFilename != nullptr) {
        std::string query{};
        query = "SELECT DATA FROM MODEL WHERE FILENAME = '";
        query += x_filename;
        query = query.erase(query.find_last_of('/')+1);
        query += pMaterials[i].pTextureFilename;
        query += "';";

        std::vector<char> buffer = get_resource(query);
        ::LPDIRECT3DTEXTURE9 temp_texture{};
        if (FAILED(::D3DXCreateTextureFromFileInMemory(
            temp_d3d_device,
            &buffer[0],
            static_cast<::UINT>(buffer.size()),
            &temp_texture))) {
          THROW_WITH_TRACE("texture file is not found.");
        } else {
          texture_.at(i).reset(temp_texture);
        }
      }
    }
  } else {
    pMaterials[0].MatD3D.Diffuse = ::D3DCOLORVALUE{0.5f, 0.5f, 0.5f, 0};
    pMaterials[0].MatD3D.Ambient = ::D3DCOLORVALUE{0.5f, 0.5f, 0.5f, 0};
    pMaterials[0].MatD3D.Specular = pMaterials[0].MatD3D.Diffuse;
  }
}

animation_mesh_allocator::animation_mesh_allocator(
    const std::string& x_filename)
    : ID3DXAllocateHierarchy{},
      x_filename_(x_filename) {}

/*
 * Alghough it's camel case and a strange type name, because this function is a
 * pure virtual function of 'ID3DXAllocateHierarchy'.
 */
::STDMETHODIMP animation_mesh_allocator::CreateFrame(
    ::LPCTSTR name, ::LPD3DXFRAME *new_frame)
{
  *new_frame = new_crt animation_mesh_frame{name};
  return S_OK;
}

/*
 * Alghough it's camel case and a strange type name, because this function is a
 * pure virtual function of 'ID3DXAllocateHierarchy'.
 */
::STDMETHODIMP animation_mesh_allocator::CreateMeshContainer(
    ::LPCSTR                    mesh_name,
    CONST ::D3DXMESHDATA*       mesh_data,
    CONST ::D3DXMATERIAL*       materials,
    CONST ::D3DXEFFECTINSTANCE*,
    ::DWORD                     materials_count,
    CONST ::DWORD*              adjacency,
    ::LPD3DXSKININFO,
    ::LPD3DXMESHCONTAINER*      mesh_container)
{
  try {
    *mesh_container = new_crt animation_mesh_container{x_filename_,
                                                       mesh_name,
                                                       mesh_data->pMesh,
                                                       materials,
                                                       materials_count,
                                                       adjacency};
  } catch (const boost::exception& e) {
    exception_reserve = boost::copy_exception(e);
    return E_FAIL;
  } catch (...) {
    return E_FAIL;
  }

  return S_OK;
}

/*
 * Alghough it's camel case and a strange type name, because this function is a
 * pure virtual function of 'ID3DXAllocateHierarchy'.
 */
::STDMETHODIMP animation_mesh_allocator::DestroyFrame(::LPD3DXFRAME frame)
{
  safe_delete_array(frame->Name);
  frame->~D3DXFRAME();
  safe_delete(frame);
  return S_OK;
}

/*
 * Alghough it's camel case and a strange type name, because this function is a
 * pure virtual function of 'ID3DXAllocateHierarchy'.
 */
::STDMETHODIMP animation_mesh_allocator::DestroyMeshContainer(
    ::LPD3DXMESHCONTAINER mesh_container_base)
{
  animation_mesh_container *mesh_container{
      static_cast<animation_mesh_container*>(mesh_container_base)};

  safe_release(mesh_container->pSkinInfo);
  safe_delete_array(mesh_container->Name);
  safe_delete_array(mesh_container->pAdjacency);
  safe_delete_array(mesh_container->pMaterials);
  safe_release(mesh_container->MeshData.pMesh);
  safe_delete(mesh_container);

  return S_OK;
}
} /* namespace early_go */
