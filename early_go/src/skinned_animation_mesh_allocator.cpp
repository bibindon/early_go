#include "stdafx.hpp"
#include "skinned_animation_mesh.hpp"
#include "skinned_animation_mesh_allocator.hpp"

using std::vector;
using std::string;

namespace early_go
{
/* c'tor */
skinned_animation_mesh_frame::skinned_animation_mesh_frame(
    const string &name)
    : D3DXFRAME{}, /* Initializes member with zero. */
      combined_matrix_{}
{
    Name = new_crt char[name.length() + 1];
    strcpy_s(Name, name.length() + 1, name.c_str());

    /* Make an argument an identity matrix. */
    D3DXMatrixIdentity(&TransformationMatrix);
    D3DXMatrixIdentity(&combined_matrix_);
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
    const string &x_filename,
    const string &mesh_name,
    LPD3DXMESH mesh,
    const D3DXMATERIAL *materials,
    const DWORD materials_count,
    const DWORD *adjacency,
    LPD3DXSKININFO skin_info)
    : D3DXMESHCONTAINER{}, /* Initializes with zero. */
      texture_{},
      palette_size_{},
      influence_count_{},
      bone_count_{},
      bone_buffer_{nullptr, custom_deleter{}},
      frame_combined_matrix_{},
      bone_offset_matrices_{}
{
    Name = new_crt char[mesh_name.length() + 1];
    strcpy_s(Name, mesh_name.length() + 1, mesh_name.c_str());

    LPDIRECT3DDEVICE9 d3d_device{nullptr};
    mesh->GetDevice(&d3d_device);

    /*
     * This IF sentence is just initializing the 'MeshData' of a member variable.
     * When this mesh doesn't have normal vector, add it.
     */
    HRESULT result{};
    if (!(mesh->GetFVF() & D3DFVF_NORMAL))
    {
        MeshData.Type = D3DXMESHTYPE_MESH;
        result = mesh->CloneMeshFVF(mesh->GetOptions(),
                                    mesh->GetFVF() | D3DFVF_NORMAL,
                                    d3d_device,
                                    &MeshData.pMesh);
        if (FAILED(result))
        {
            THROW_WITH_TRACE("Failed 'CloneMeshFVF' function.");
        }
        mesh = MeshData.pMesh;
        D3DXComputeNormals(mesh, nullptr);
    }
    else
    {
        MeshData.pMesh = mesh;
        MeshData.Type = D3DXMESHTYPE_MESH;
        mesh->AddRef();
    }

    /* Initialize the 'pAdjacency' of a member variable. */
    DWORD adjacency_count{mesh->GetNumFaces() * 3};
    pAdjacency = new_crt DWORD[adjacency_count];

    for (DWORD i{}; i < adjacency_count; ++i)
    {
        pAdjacency[i] = adjacency[i];
    }

    initialize_materials(materials_count, materials, x_filename, d3d_device);
    initialize_bone(skin_info, mesh);
    initialize_FVF(d3d_device);
    initialize_vertex_element();
}

void skinned_animation_mesh_container::initialize_materials(
    const DWORD &materials_count,
    const D3DXMATERIAL *materials,
    const string &x_filename,
    const LPDIRECT3DDEVICE9 &d3d_device)
{
    /* This strange bracket is measures of being interpretered as WinAPI macro. */
    NumMaterials = std::max(1UL, materials_count);
    pMaterials = new_crt D3DXMATERIAL[NumMaterials];
    vector<std::unique_ptr<IDirect3DTexture9, custom_deleter>>
        temp_texture(NumMaterials);
    texture_.swap(temp_texture);

    /*
     * Initialize the 'pMaterials' and the 'texture_' of member variables
     * if there are.
     */
    if (materials_count > 0)
    {
        for (DWORD i{}; i < materials_count; ++i)
        {
            pMaterials[i] = materials[i];
            if (pMaterials[i].pTextureFilename != nullptr)
            {
                vector<char> buffer =
                    util::get_model_texture_resource(x_filename, pMaterials[i].pTextureFilename);
                LPDIRECT3DTEXTURE9 temp_texture{};
                if (FAILED(D3DXCreateTextureFromFileInMemory(
                        d3d_device,
                        &buffer[0],
                        static_cast<UINT>(buffer.size()),
                        &temp_texture)))
                {
                    THROW_WITH_TRACE("texture file is not found.");
                }
                else
                {
                    texture_.at(i).reset(temp_texture);
                }
            }
        }
    }
    else
    {
        pMaterials[0].MatD3D.Diffuse = D3DCOLORVALUE{0.5f, 0.5f, 0.5f, 0};
        pMaterials[0].MatD3D.Ambient = D3DCOLORVALUE{0.5f, 0.5f, 0.5f, 0};
        pMaterials[0].MatD3D.Specular = pMaterials[0].MatD3D.Diffuse;
    }
}

void skinned_animation_mesh_container::initialize_bone(
    const LPD3DXSKININFO &skin_info, const LPD3DXMESH &mesh)
{
    if (skin_info == nullptr)
    {
        THROW_WITH_TRACE("Failed to get skin info.");
    }
    pSkinInfo = skin_info;
    pSkinInfo->AddRef();

    UINT bone_count = pSkinInfo->GetNumBones();
    bone_offset_matrices_.resize(bone_count);

    for (DWORD i = 0; i < bone_count; ++i)
    {
        bone_offset_matrices_[i] = *pSkinInfo->GetBoneOffsetMatrix(i);
    }

    // TODO Improve.
    DWORD MAX_MATRICES = 26;
    palette_size_ = std::min(MAX_MATRICES, pSkinInfo->GetNumBones());

    // generate skinned mesh
    safe_release(MeshData.pMesh);

    LPD3DXBUFFER bone_buffer{};
    if (FAILED(pSkinInfo->ConvertToIndexedBlendedMesh(mesh,
                                                      0, /* not used */
                                                      palette_size_,
                                                      pAdjacency,
                                                      nullptr,
                                                      nullptr,
                                                      nullptr,
                                                      &influence_count_,
                                                      &bone_count_,
                                                      &bone_buffer,
                                                      &MeshData.pMesh)))
    {
        THROW_WITH_TRACE("Failed to get skin info.");
    }
    bone_buffer_.reset(bone_buffer);
}

void skinned_animation_mesh_container::initialize_FVF(
    const LPDIRECT3DDEVICE9 &d3d_device)
{
    DWORD new_FVF = (MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) |
                    D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;

    if (new_FVF != MeshData.pMesh->GetFVF())
    {
        LPD3DXMESH p_mesh{};
        HRESULT hresult = MeshData.pMesh->CloneMeshFVF(
            MeshData.pMesh->GetOptions(),
            new_FVF,
            d3d_device,
            &p_mesh);
        if (SUCCEEDED(hresult))
        {
            MeshData.pMesh->Release();
            MeshData.pMesh = p_mesh;
            p_mesh = NULL;
        }
    }
}

void skinned_animation_mesh_container::initialize_vertex_element()
{
    D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
    LPD3DVERTEXELEMENT9 current_decl;
    HRESULT result = MeshData.pMesh->GetDeclaration(decl);
    if (FAILED(result))
    {
        THROW_WITH_TRACE("Failed to get skin info.");
    }

    current_decl = decl;
    while (current_decl->Stream != 0xff)
    {
        if ((current_decl->Usage == D3DDECLUSAGE_BLENDINDICES) && (current_decl->UsageIndex == 0))
        {
            current_decl->Type = D3DDECLTYPE_D3DCOLOR;
        }
        current_decl++;
    }

    result = MeshData.pMesh->UpdateSemantics(decl);
    if (FAILED(result))
    {
        THROW_WITH_TRACE("Failed to get skin info.");
    }
}

skinned_animation_mesh_allocator::skinned_animation_mesh_allocator(
    const string &x_filename)
    : ID3DXAllocateHierarchy{},
      x_filename_(x_filename) {}

/*
 * Alghough it's camel case and a strange type name, because this function is a
 * pure virtual function of 'ID3DXAllocateHierarchy'.
 */
STDMETHODIMP skinned_animation_mesh_allocator::CreateFrame(
    LPCTSTR name, LPD3DXFRAME *new_frame)
{
    *new_frame = new_crt skinned_animation_mesh_frame{name};
    return S_OK;
}

/*
 * Alghough it's camel case and a strange type name, because this function is a
 * pure virtual function of 'ID3DXAllocateHierarchy'.
 */
STDMETHODIMP skinned_animation_mesh_allocator::CreateMeshContainer(
    LPCSTR mesh_name,
    CONST D3DXMESHDATA *mesh_data,
    CONST D3DXMATERIAL *materials,
    CONST D3DXEFFECTINSTANCE *,
    DWORD materials_count,
    CONST DWORD *adjacency,
    LPD3DXSKININFO skin_info,
    LPD3DXMESHCONTAINER *mesh_container)
{
    try
    {
        *mesh_container = new_crt skinned_animation_mesh_container{x_filename_,
                                                                   mesh_name,
                                                                   mesh_data->pMesh,
                                                                   materials,
                                                                   materials_count,
                                                                   adjacency,
                                                                   skin_info};
    }
    catch (const std::exception &expception)
    {
        early_go::log_liner{} << boost::diagnostic_information(expception);
        return E_FAIL;
    }
    return S_OK;
}

/*
 * Alghough it's camel case and a strange type name, because this function is a
 * pure virtual function of 'ID3DXAllocateHierarchy'.
 */
STDMETHODIMP skinned_animation_mesh_allocator::DestroyFrame(
    LPD3DXFRAME frame)
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
STDMETHODIMP skinned_animation_mesh_allocator::DestroyMeshContainer(
    LPD3DXMESHCONTAINER mesh_container_base)
{
    skinned_animation_mesh_container *mesh_container{
        static_cast<skinned_animation_mesh_container *>(mesh_container_base)};

    safe_release(mesh_container->pSkinInfo);
    safe_delete_array(mesh_container->Name);
    safe_delete_array(mesh_container->pAdjacency);
    safe_delete_array(mesh_container->pMaterials);
    safe_release(mesh_container->MeshData.pMesh);
    safe_delete(mesh_container);

    return S_OK;
}
} /* namespace early_go */
