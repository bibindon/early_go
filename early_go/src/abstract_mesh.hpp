#ifndef BASE_MESH_HPP
#define BASE_MESH_HPP

#include "stdafx.hpp"
#include "animation_strategy.hpp"

namespace early_go
{
struct message_writer;

class abstract_mesh
{
public:
    static constexpr int LAYER_NUMBER{8};

    abstract_mesh(
        const std::shared_ptr<IDirect3DDevice9> &,
        const std::string &,
        const D3DXVECTOR3 &,
        const D3DXVECTOR3 &);
    virtual ~abstract_mesh();
    void set_position(const D3DXVECTOR3 &);
    void set_rotation(const D3DXVECTOR3 &);
    void set_animation(const std::string &);
    void set_default_animation(const std::string &);
    void set_animation_config(const std::string &, const bool &, const float &);
    void render(const D3DXMATRIX &, const D3DXMATRIX &, const D3DXVECTOR4 &, const float &);

    //-----------------------------------------------
    // Texture operation
    //-----------------------------------------------

    enum class combine_type
    {
        NORMAL,
        ADDITION,
        //  MULTIPLICATION,
    };

    void set_dynamic_texture(const std::string &, const int &, const combine_type &);
    void set_dynamic_texture_position(const int &, const D3DXVECTOR2 &);
    void set_dynamic_texture_opacity(const int &, const float &);
    void flip_dynamic_texture(const int &);
    void clear_dynamic_texture(const int &);

    void set_dynamic_message(
        const int &,
        const std::string &,
        const bool & = false,
        const cv::Rect & = {
            0,
            0,
            constants::TEXTURE_PIXEL_SIZE - 1,
            constants::TEXTURE_PIXEL_SIZE - 1},
        const DWORD & = D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff),
        const std::string & = constants::FONT_NAME,
        const int & = 40,
        const int & = 0,
        const BYTE & = SHIFTJIS_CHARSET,
        const bool & = true);

    void set_dynamic_message_color(const int &, const D3DXVECTOR4 &);
    bool is_tex_animation_finished(const int);
    void set_shake_texture();
    void set_fade_in();
    void set_fade_out();

protected:
    std::shared_ptr<IDirect3DDevice9> d3d_device_;
    std::unique_ptr<ID3DXEffect, custom_deleter> effect_;
    std::unique_ptr<animation_strategy> animation_strategy_;
    struct dynamic_texture;
    std::unique_ptr<dynamic_texture> dynamic_texture_;
    D3DXHANDLE texture_position_handle_;
    D3DXHANDLE texture_opacity_handle_;
    D3DXHANDLE light_normal_handle_;
    D3DXHANDLE brightness_handle_;
    D3DXHANDLE mesh_texture_handle_;
    D3DXHANDLE diffuse_handle_;
    std::array<D3DXHANDLE, LAYER_NUMBER> texture_handle_;
    D3DXVECTOR3 position_;
    D3DXVECTOR3 rotation_;

private:
    virtual void render_impl(const D3DXMATRIX &, const D3DXMATRIX &) = 0;
};
}
#endif

