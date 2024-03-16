#ifndef BASE_MESH_HPP
#define BASE_MESH_HPP

#include "stdafx.hpp"
#include "animation_strategy.hpp"

namespace early_go
{
    struct message_writer;

    class base_mesh
    {
    public:
        base_mesh(
            const std::shared_ptr<IDirect3DDevice9> &,
            const std::string &,
            const D3DXVECTOR3 &,
            const D3DXVECTOR3 &);
        enum class combine_type
        {
            NORMAL,
            ADDITION,
            //  MULTIPLICATION,
        };
        void set_animation(const std::string &);
        void set_default_animation(const std::string &);
        void set_animation_config(const std::string &, const bool &, const float &);
        void render(const D3DXMATRIX &, const D3DXMATRIX &, const D3DXVECTOR4 &, const float &);

        void set_dynamic_texture(const std::string &, const int &, const combine_type &);
        void set_dynamic_texture_position(const int &, const D3DXVECTOR2 &);
        void set_dynamic_texture_opacity(const int &, const float &);
        void flip_dynamic_texture(const int &);
        void clear_dynamic_texture(const int &);

        //  static const int TEXTURE_PIXEL_SIZE;
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
            const std::string & = "ü‡ÉSÉVÉbÉN",
            const int & = 40,
            const int & = 0,
            const BYTE & = SHIFTJIS_CHARSET,
            const bool & = true);

        void set_dynamic_message_color(const int &, const D3DXVECTOR4 &);
        bool is_tex_animation_finished(const int);
        void set_position(const D3DXVECTOR3 &);
        void set_rotation(const D3DXVECTOR3 &);
        void set_shake_texture();
        void set_fade_in();
        void set_fade_out();

    protected:
        std::shared_ptr<IDirect3DDevice9> d3d_device_;
        std::unique_ptr<ID3DXEffect, custom_deleter> effect_;
        std::unique_ptr<animation_strategy> animation_strategy_;
        struct dynamic_texture
        {
            // Do not make dynamic_texture to array because it is necessary to transfer
            // position, opacity and color info to GPU.
            static constexpr int LAYER_NUMBER{8};
            static constexpr int FADE_LAYER{LAYER_NUMBER - 1};
            std::array<std::shared_ptr<IDirect3DTexture9>, LAYER_NUMBER> textures_;
            std::array<std::string, LAYER_NUMBER> filename_;
            std::array<D3DXVECTOR4, LAYER_NUMBER> positions_;
            std::array<float, LAYER_NUMBER> opacities_;
            std::array<D3DXVECTOR4, LAYER_NUMBER> colors_;
            std::array<bool, LAYER_NUMBER> flipped_;
            std::array<cv::Size, LAYER_NUMBER> tex_size_;
            std::array<bool, LAYER_NUMBER> tex_animation_finished_;

            std::array<std::shared_ptr<message_writer>, LAYER_NUMBER> writer_;
            struct texture_shaker
            {
                texture_shaker();
                void operator()(base_mesh &);

            private:
                int count_;
                // A fixed value is used. That's because random value doesn't become
                // appropriate.
                static constexpr int SHAKE_POSITIONS_SIZE{16};
                const static std::array<D3DXVECTOR2, SHAKE_POSITIONS_SIZE> SHAKING_POSITIONS;
                const static int SHAKE_FRAME;
                const static int SHAKE_DURATION;
                D3DXVECTOR2 current_position_;
                D3DXVECTOR2 previous_position_;
            };
            std::shared_ptr<texture_shaker> texture_shaker_;
            struct texture_fader
            {
                enum class fade_type
                {
                    FADE_IN,
                    FADE_OUT,
                };
                texture_fader(const fade_type &);
                void operator()(base_mesh &);

            private:
                int count_;
                fade_type fade_type_;
                const static int FADE_DURATION;
            };
            std::shared_ptr<texture_fader> texture_fader_;
        } dynamic_texture_;
        D3DXHANDLE texture_position_handle_;
        D3DXHANDLE texture_opacity_handle_;
        D3DXHANDLE light_normal_handle_;
        D3DXHANDLE brightness_handle_;
        D3DXHANDLE mesh_texture_handle_;
        D3DXHANDLE diffuse_handle_;
        std::array<D3DXHANDLE, dynamic_texture::LAYER_NUMBER> texture_handle_;
        D3DXVECTOR3 position_;
        D3DXVECTOR3 rotation_;

    private:
        virtual void render_impl(const D3DXMATRIX &, const D3DXMATRIX &) = 0;
    };
}
#endif

