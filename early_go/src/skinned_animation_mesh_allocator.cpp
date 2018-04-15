#include "stdafx.hpp"
#include "skinned_animation_mesh.hpp"
#include "skinned_animation_mesh_allocator.hpp"

namespace early_go {
/* c'tor */
skinned_animation_mesh_frame::skinned_animation_mesh_frame(
    const std::string& a_kr_name)
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
skinned_animation_mesh_container::skinned_animation_mesh_container(
    const std::string&    a_krsz_x_filename,
    const std::string&    a_krsz_meshname,
    ::LPD3DXMESH          a_p_d3dx_mesh,
    const ::D3DXMATERIAL* a_kp_materials,
    const ::DWORD         a_k_materials_number,
    const ::DWORD*        a_kp_adjacency,
    ::LPD3DXSKININFO      a_p_skin_info)
    : D3DXMESHCONTAINER{}, /* Initializes with zero. */
      vecup_texture_{},
      dw_palette_size_{},
      dw_influence_number_{},
      dw_bone_amount_{},
      up_bone_buffer_{nullptr, custom_deleter{}},
      vecp_frame_combined_matrix_{},
      vec_bone_offset_matrices_{}
{
  /*
   * The 'dup' of '::_strdup' is the abbreviation of 'duplicate', create new
   * string by the argument string.
   */
  this->Name = ::_strdup(a_krsz_meshname.c_str());

  ::LPDIRECT3DDEVICE9 p_temp_direct3d_device9{nullptr};
  a_p_d3dx_mesh->GetDevice(&p_temp_direct3d_device9);

  /*
   * This IF sentence is just initializing the 'MeshData' of a member variable.
   * When this mesh doesn't have normal vector, add it.
   */
  ::HRESULT hresult{};
  if (!(a_p_d3dx_mesh->GetFVF() & D3DFVF_NORMAL)) {
    this->MeshData.Type = ::D3DXMESHTYPE_MESH;
    hresult =
        a_p_d3dx_mesh->CloneMeshFVF(a_p_d3dx_mesh->GetOptions(),
                                    a_p_d3dx_mesh->GetFVF() | D3DFVF_NORMAL,
                                    p_temp_direct3d_device9,
                                    &this->MeshData.pMesh);
    if (FAILED(hresult)) {
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

  /* Initialize the 'pAdjacency' of a member variable. */
  ::DWORD dw_face_amount{a_p_d3dx_mesh->GetNumFaces()};
  this->pAdjacency = new_crt ::DWORD[dw_face_amount * 3]{};

  for (unsigned int i{}; i < dw_face_amount * 3; ++i) {
    this->pAdjacency[i] = a_kp_adjacency[i];
  }

  this->initialize_materials(a_k_materials_number,
                             a_kp_materials,
                             a_krsz_x_filename,
                             p_temp_direct3d_device9);
  this->initialize_bone(a_p_skin_info, a_p_d3dx_mesh);
  this->initialize_FVF(p_temp_direct3d_device9);
  this->initialize_vertex_element();
}

void skinned_animation_mesh_container::initialize_materials(
    const DWORD&             a_k_materials_number,
    const D3DXMATERIAL*      a_kp_materials,
    const std::string&       a_krsz_x_filename,
    const LPDIRECT3DDEVICE9& a_p_temp_direct3d_device9)
{
  /* This strange bracket is measures of being interpretered as WinAPI macro. */
  this->NumMaterials = (std::max)(1UL, a_k_materials_number);
  this->pMaterials = new_crt::D3DXMATERIAL[this->NumMaterials]{};
  std::vector<std::unique_ptr<::IDirect3DTexture9, custom_deleter> >
      temp_texture(this->NumMaterials);
  this->vecup_texture_.swap(temp_texture);

  /*
  * Initialize the 'pMaterials' and the 'vecup_texture_' of member variables
  * if there are.
  */
  if (a_k_materials_number > 0) {
    for (::DWORD i{}; i < a_k_materials_number; ++i) {
      this->pMaterials[i] = a_kp_materials[i];
      if (this->pMaterials[i].pTextureFilename != nullptr) {
        std::string sz_query;
        sz_query = "select data from texture where filename = '";
        sz_query += this->pMaterials[i].pTextureFilename;
        sz_query += "' and x_filename = '";
        sz_query += a_krsz_x_filename + "';";
        std::vector<char> vecc_buffer = get_resource(sz_query);
        ::LPDIRECT3DTEXTURE9 p_temp_texture{};
        if (FAILED(
            ::D3DXCreateTextureFromFileInMemory(
                a_p_temp_direct3d_device9,
                &vecc_buffer[0],
                static_cast<UINT>(vecc_buffer.size()),
                &p_temp_texture))) {
          BOOST_THROW_EXCEPTION(custom_exception{"texture file is not found."});
        } else {
          this->vecup_texture_.at(i).reset(p_temp_texture);
        }
      }
    }
  }
  else {
    this->pMaterials[0].MatD3D.Diffuse = ::D3DCOLORVALUE{ 0.5f, 0.5f, 0.5f, 0 };
    this->pMaterials[0].MatD3D.Ambient = ::D3DCOLORVALUE{ 0.5f, 0.5f, 0.5f, 0 };
    this->pMaterials[0].MatD3D.Specular = this->pMaterials[0].MatD3D.Diffuse;
  }
}

void skinned_animation_mesh_container::initialize_bone(
    const ::LPD3DXSKININFO &a_p_skin_info, const ::LPD3DXMESH &a_p_d3dx_mesh)
{
  if (a_p_skin_info == nullptr) {
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to get skin info."});
  }
  this->pSkinInfo = a_p_skin_info;
  this->pSkinInfo->AddRef();

  ::UINT dw_bone_amount = pSkinInfo->GetNumBones();
  this->vec_bone_offset_matrices_.resize(dw_bone_amount);

  for (::DWORD i = 0; i < dw_bone_amount; ++i) {
    this->vec_bone_offset_matrices_[i] =
        *this->pSkinInfo->GetBoneOffsetMatrix(i);
  }

  // TODO Improve.
  ::UINT MAX_MATRICES = 26;
  this->dw_palette_size_ = min(MAX_MATRICES, this->pSkinInfo->GetNumBones());

  // generate skinned mesh
  safe_release(this->MeshData.pMesh);

  LPD3DXBUFFER p_bone_buffer{};
  if (FAILED(
      this->pSkinInfo->ConvertToIndexedBlendedMesh(a_p_d3dx_mesh,
                                                   0, /* not used */
                                                   this->dw_palette_size_,
                                                   this->pAdjacency,
                                                   nullptr,
                                                   nullptr,
                                                   nullptr,
                                                   &this->dw_influence_number_,
                                                   &this->dw_bone_amount_,
                                                   &p_bone_buffer,
                                                   &this->MeshData.pMesh))) {
    BOOST_THROW_EXCEPTION(custom_exception{ "Failed to get skin info." });
  }
  this->up_bone_buffer_.reset(p_bone_buffer);
}

void skinned_animation_mesh_container::initialize_FVF(
    const ::LPDIRECT3DDEVICE9 &a_p_temp_direct3d_device9)
{
  ::DWORD new_FVF = (this->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) |
      D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;

  if (new_FVF != this->MeshData.pMesh->GetFVF()) {
    ::LPD3DXMESH p_mesh{};
    ::HRESULT hresult =
        this->MeshData.pMesh->CloneMeshFVF(this->MeshData.pMesh->GetOptions(),
                                           new_FVF,
                                           a_p_temp_direct3d_device9,
                                           &p_mesh);
    if (SUCCEEDED(hresult)) {
      this->MeshData.pMesh->Release();
      this->MeshData.pMesh = p_mesh;
      p_mesh = NULL;
    }
  }
}

void skinned_animation_mesh_container::initialize_vertex_element()
{
  ::D3DVERTEXELEMENT9 decl[::MAX_FVF_DECL_SIZE];
  ::LPD3DVERTEXELEMENT9 p_current_decl;
  ::HRESULT hresult = this->MeshData.pMesh->GetDeclaration(decl);
  if (FAILED(hresult)) {
    BOOST_THROW_EXCEPTION(custom_exception{ "Failed to get skin info." });
  }

  p_current_decl = decl;
  while (p_current_decl->Stream != 0xff) {
    if ( (p_current_decl->Usage      == ::D3DDECLUSAGE_BLENDINDICES)
      && (p_current_decl->UsageIndex == 0)) {
      p_current_decl->Type = ::D3DDECLTYPE_D3DCOLOR;
    }
    p_current_decl++;
  }

  hresult = this->MeshData.pMesh->UpdateSemantics(decl);
  if (FAILED(hresult)) {
    BOOST_THROW_EXCEPTION(custom_exception{ "Failed to get skin info." });
  }
}

skinned_animation_mesh_allocator::skinned_animation_mesh_allocator(
    const std::string & a_krsz_x_filename)
    : ID3DXAllocateHierarchy{},
      x_filename_(a_krsz_x_filename) {}

/*
 * Alghough it's camel case and a strange type name, because this function is a
 * pure virtual function of 'ID3DXAllocateHierarchy'.
 */
::STDMETHODIMP skinned_animation_mesh_allocator::CreateFrame(
    ::LPCTSTR a_name, ::LPD3DXFRAME *a_pp_new_frame)
{
  *a_pp_new_frame = new_crt skinned_animation_mesh_frame{a_name};
  return S_OK;
}

/*
 * Alghough it's camel case and a strange type name, because this function is a
 * pure virtual function of 'ID3DXAllocateHierarchy'.
 */
::STDMETHODIMP skinned_animation_mesh_allocator::CreateMeshContainer(
    ::LPCSTR a_kr_meshname,
    CONST ::D3DXMESHDATA* a_kp_mesh_data,
    CONST ::D3DXMATERIAL* a_kp_materials,
    CONST ::D3DXEFFECTINSTANCE*,
    ::DWORD a_materials_number,
    CONST ::DWORD *a_kp_adjacency,
    ::LPD3DXSKININFO a_p_skin_info,
    ::LPD3DXMESHCONTAINER *a_pp_mesh_container)
{
  try {
    *a_pp_mesh_container =
        new_crt skinned_animation_mesh_container{this->x_filename_,
                                                 a_kr_meshname,
                                                 a_kp_mesh_data->pMesh,
                                                 a_kp_materials,
                                                 a_materials_number,
                                                 a_kp_adjacency,
                                                 a_p_skin_info};
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
::STDMETHODIMP skinned_animation_mesh_allocator::DestroyFrame(
    ::LPD3DXFRAME a_p_frame_to_free)
{
  safe_delete_array(a_p_frame_to_free->Name);
  a_p_frame_to_free->~D3DXFRAME();
  safe_delete(a_p_frame_to_free);
  return S_OK;
}

/*
 * Alghough it's camel case and a strange type name, because this function is a
 * pure virtual function of 'ID3DXAllocateHierarchy'.
 */
::STDMETHODIMP skinned_animation_mesh_allocator::DestroyMeshContainer(
    ::LPD3DXMESHCONTAINER a_p_mesh_container_base)
{
  skinned_animation_mesh_container *_p_mesh_container{
      static_cast<skinned_animation_mesh_container*>(a_p_mesh_container_base)};

  safe_release(_p_mesh_container->pSkinInfo);
  safe_delete_array(_p_mesh_container->Name);
  safe_delete_array(_p_mesh_container->pAdjacency);
  safe_delete_array(_p_mesh_container->pMaterials);
  safe_release(_p_mesh_container->MeshData.pMesh);
  safe_delete(_p_mesh_container);

  return S_OK;
}
} /* namespace early_go */
