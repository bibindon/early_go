#include "stdafx.hpp"

#include "base_mesh.hpp"
#include "text.hpp"

using std::string;
using std::vector;

namespace early_go
{

    base_mesh::base_mesh(
        const std::shared_ptr<IDirect3DDevice9> &d3d_device,
        const string &shader_filename,
        const D3DXVECTOR3 &position,
        const D3DXVECTOR3 &rotation)
        : d3d_device_{d3d_device},
          effect_{nullptr, custom_deleter{}},
          animation_strategy_{nullptr},
          dynamic_texture_{},
          position_{position},
          rotation_{rotation}
    {
        HRESULT result{NULL};

        vector<char> buffer{get_resource(
            "SELECT DATA FROM SHADER WHERE FILENAME = 'shader/" + shader_filename + "';")};

        LPD3DXEFFECT effect{nullptr};
        D3DXCreateEffect(
            d3d_device_.get(),
            &buffer[0],
            static_cast<UINT>(buffer.size()),
            nullptr,
            nullptr,
            0,
            nullptr,
            &effect,
            nullptr);
        effect_.reset(effect);
        if (FAILED(result))
        {
            THROW_WITH_TRACE("Failed to create an effect file.");
        }

        texture_handle_[0] = effect_->GetParameterByName(nullptr, "g_texture_0");
        texture_handle_[1] = effect_->GetParameterByName(nullptr, "g_texture_1");
        texture_handle_[2] = effect_->GetParameterByName(nullptr, "g_texture_2");
        texture_handle_[3] = effect_->GetParameterByName(nullptr, "g_texture_3");
        texture_handle_[4] = effect_->GetParameterByName(nullptr, "g_texture_4");
        texture_handle_[5] = effect_->GetParameterByName(nullptr, "g_texture_5");
        texture_handle_[6] = effect_->GetParameterByName(nullptr, "g_texture_6");
        texture_handle_[7] = effect_->GetParameterByName(nullptr, "g_texture_7");

        texture_position_handle_ = effect_->GetParameterByName(nullptr, "g_position");
        texture_opacity_handle_ = effect_->GetParameterByName(nullptr, "g_opacity");
        light_normal_handle_ = effect_->GetParameterByName(nullptr, "g_light_normal");
        brightness_handle_ = effect_->GetParameterByName(nullptr, "g_light_brightness");
        mesh_texture_handle_ = effect_->GetParameterByName(nullptr, "g_mesh_texture");
        diffuse_handle_ = effect_->GetParameterByName(nullptr, "g_diffuse");

        for (int i{}; i < dynamic_texture::LAYER_NUMBER; ++i)
        {
            LPDIRECT3DTEXTURE9 temp_texture{nullptr};
            HRESULT result{D3DXCreateTexture(
                d3d_device_.get(),
                constants::EMPTY_TEXTURE_SIZE,
                constants::EMPTY_TEXTURE_SIZE,
                1,
                D3DUSAGE_DYNAMIC,
                D3DFMT_A8B8G8R8,
                D3DPOOL_DEFAULT,
                &temp_texture)};
            if (FAILED(result))
            {
                const string str{DXGetErrorString(result)};
                THROW_WITH_TRACE("Failed to create texture.: " + str);
            }
            else
            {
                D3DLOCKED_RECT locked_rect{0};
                temp_texture->LockRect(0, &locked_rect, nullptr, D3DLOCK_DISCARD);

                std::fill(static_cast<int *>(locked_rect.pBits),
                          static_cast<int *>(locked_rect.pBits) +
                              static_cast<std::size_t>(locked_rect.Pitch) *
                                  constants::EMPTY_TEXTURE_SIZE / sizeof(DWORD),
                          0x00000000UL);

                temp_texture->UnlockRect(0);

                dynamic_texture_.textures_.at(i).reset(temp_texture, custom_deleter{});
                dynamic_texture_.positions_.at(i) = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
                dynamic_texture_.tex_size_.at(i) =
                    cv::Size{constants::EMPTY_TEXTURE_SIZE,
                             constants::EMPTY_TEXTURE_SIZE};
                dynamic_texture_.tex_animation_finished_.at(i) = true;

                effect_->SetTexture(texture_handle_.at(i), dynamic_texture_.textures_.at(i).get());
            }
        }
        dynamic_texture_.opacities_.at(dynamic_texture::FADE_LAYER) = 0.0f;
    }

    static vector<::uchar> resize_with_margin(const cv::Mat &source)
    {
        cv::Mat blank{cv::Mat::zeros(
            constants::TEXTURE_PIXEL_SIZE, constants::TEXTURE_PIXEL_SIZE, CV_8UC4)};

        cv::Mat destination{blank(cv::Rect(0, 0, source.rows, source.cols))};
        source.copyTo(destination);
        vector<::uchar> cv_buffer{};
        cv::imencode(".bmp", blank, cv_buffer);
        return cv_buffer;
    }

    static vector<::uchar> resize_with_margin(const vector<char> &buffer)
    {
        vector<::uchar> cv_buffer{buffer.cbegin(), buffer.cend()};
        cv::Mat source{cv::imdecode(cv::Mat(cv_buffer), cv::IMREAD_UNCHANGED)};

        return resize_with_margin(source);
    }

    void base_mesh::set_dynamic_texture(
        const string &filename,
        const int &layer_number,
        const combine_type &combine)
    {
        string query{"SELECT DATA FROM IMAGE WHERE FILENAME = '" + filename + "';"};

        vector<char> buffer{get_resource(query)};

        vector<::uchar> cv_buffer{resize_with_margin(buffer)};

        if (combine == combine_type::NORMAL)
        {
            LPDIRECT3DTEXTURE9 temp_texture{};
            if (FAILED(D3DXCreateTextureFromFileInMemory(
                    d3d_device_.get(),
                    &cv_buffer[0],
                    static_cast<UINT>(cv_buffer.size()),
                    &temp_texture)))
            {
                THROW_WITH_TRACE("texture file is not found.");
            }
            else
            {
                dynamic_texture_.filename_.at(layer_number) = filename;
                dynamic_texture_.textures_.at(layer_number).reset(temp_texture, custom_deleter{});
                dynamic_texture_.opacities_.at(layer_number) = 1.0f;
                dynamic_texture_.flipped_.at(layer_number) = false;
                dynamic_texture_.tex_size_.at(layer_number) =
                    cv::Size{constants::TEXTURE_PIXEL_SIZE, constants::TEXTURE_PIXEL_SIZE};

                effect_->SetTexture(texture_handle_.at(layer_number),
                                    dynamic_texture_.textures_.at(layer_number).get());
            }
        }
        else if (combine == combine_type::ADDITION)
        {
            dynamic_texture_.filename_.at(layer_number) = filename;
            dynamic_texture_.opacities_.at(layer_number) = 1.0f;
            dynamic_texture_.flipped_.at(layer_number) = false;
            dynamic_texture_.tex_size_.at(layer_number) =
                cv::Size{constants::TEXTURE_PIXEL_SIZE, constants::TEXTURE_PIXEL_SIZE};

            cv::Size tex_size = dynamic_texture_.tex_size_.at(layer_number);

            cv::Mat source{cv::imdecode(cv::Mat(cv_buffer), cv::IMREAD_UNCHANGED)};
            int ch = source.channels();

            D3DLOCKED_RECT locked_rect{};
            dynamic_texture_.textures_.at(layer_number)->LockRect(
                0, &locked_rect, nullptr, D3DLOCK_DISCARD);

            unsigned char *tmp = static_cast<unsigned char *>(locked_rect.pBits);
            for (int j = 0; j < tex_size.height; ++j)
            {
                for (int i = 0; i < tex_size.width; ++i)
                {
                    if (source.data[j * 4 * tex_size.width + i * 3] != 0)
                    {

                        // if (source.at<cv::Vec4b>(j, i)[3] != 0) {
                        //          if (200 <= j && j <= 500) {
                        // ???
                        if (tmp != nullptr)
                        {
                            tmp[j * 4 * tex_size.width + i * 4] = 255;     // source.data[j*4*tex_size.width+i*4];
                            tmp[j * 4 * tex_size.width + i * 4 + 1] = 255; // source.data[j*4*tex_size.width+i*4+1];
                            tmp[j * 4 * tex_size.width + i * 4 + 2] = 255; // source.data[j*4*tex_size.width+i*4+2];
                            tmp[j * 4 * tex_size.width + i * 4 + 3] = 255; // source.data[j*4*tex_size.width+i*4+3];
                        }
                    }
                }
            }

            dynamic_texture_.textures_.at(layer_number)->UnlockRect(0);

            effect_->SetTexture(texture_handle_.at(layer_number),
                                dynamic_texture_.textures_.at(layer_number).get());
        }
    }

    void base_mesh::set_dynamic_texture_position(
        const int &layer_number, const D3DXVECTOR2 &position)
    {
        dynamic_texture_.positions_.at(layer_number).x = position.x;
        dynamic_texture_.positions_.at(layer_number).y = position.y;
    }

    void base_mesh::set_dynamic_texture_opacity(
        const int &layer_number, const float &opacity)
    {
        dynamic_texture_.opacities_.at(layer_number) = opacity;
    }

    void base_mesh::flip_dynamic_texture(const int &layer_number)
    {
        string query{"SELECT DATA FROM IMAGE WHERE FILENAME = '" +
                          dynamic_texture_.filename_.at(layer_number) + "';"};

        vector<char> buffer{get_resource(query)};
        vector<::uchar> cv_buffer{buffer.cbegin(), buffer.cend()};
        cv::Mat source{cv::imdecode(cv::Mat(cv_buffer), cv::IMREAD_UNCHANGED)};
        if (!dynamic_texture_.flipped_.at(layer_number))
        {
            cv::flip(source, source, 1);
        }
        // flip flipped flag.
        dynamic_texture_.flipped_.at(layer_number) = !dynamic_texture_.flipped_.at(layer_number);

        cv_buffer = resize_with_margin(source);

        LPDIRECT3DTEXTURE9 temp_texture{};
        if (FAILED(D3DXCreateTextureFromFileInMemory(
                d3d_device_.get(),
                &cv_buffer[0],
                static_cast<UINT>(cv_buffer.size()),
                &temp_texture)))
        {
            THROW_WITH_TRACE("texture file is not found.");
        }
        else
        {
            dynamic_texture_.textures_.at(layer_number).reset(temp_texture, custom_deleter{});

            effect_->SetTexture(texture_handle_.at(layer_number),
                                dynamic_texture_.textures_.at(layer_number).get());
        }
    }

    void base_mesh::clear_dynamic_texture(const int &layer_number)
    {
        cv::Size tex_size = dynamic_texture_.tex_size_.at(layer_number);

        D3DLOCKED_RECT locked_rect{};
        dynamic_texture_.textures_.at(layer_number)->LockRect(0, &locked_rect, nullptr, D3DLOCK_DISCARD);

        std::fill(static_cast<int *>(locked_rect.pBits),
                  static_cast<int *>(locked_rect.pBits) +
                      static_cast<std::size_t>(tex_size.height) * tex_size.width,
                  0x00000000);

        dynamic_texture_.textures_.at(layer_number)->UnlockRect(0);
    }

    void base_mesh::set_dynamic_message(
        const int &layer_number,
        const string &message,
        const bool &is_animated,
        const cv::Rect &rect,
        const DWORD &color,
        const string &fontname,
        const int &size,
        const int &weight,
        const BYTE &charset,
        const bool &proportional)
    {
        dynamic_texture_.opacities_.at(layer_number) = 1.0f;
        dynamic_texture_.tex_size_.at(layer_number) =
            cv::Size{constants::TEXTURE_PIXEL_SIZE, constants::TEXTURE_PIXEL_SIZE};
        message_writer *writer{new_crt message_writer{
            d3d_device_,
            dynamic_texture_.textures_.at(layer_number),
            message,
            is_animated,
            rect,
            color,
            fontname,
            size,
            weight,
            charset,
            proportional}};

        effect_->SetTexture(texture_handle_.at(layer_number),
                            dynamic_texture_.textures_.at(layer_number).get());

        dynamic_texture_.tex_animation_finished_.at(layer_number) = false;
        dynamic_texture_.writer_.at(layer_number).reset(writer);
    }

    void base_mesh::set_dynamic_message_color(const int &layer_number,
                                              const D3DXVECTOR4 &color)
    {
        dynamic_texture_.colors_.at(layer_number) = color;
    }

    bool base_mesh::is_tex_animation_finished(const int layer_number)
    {
        return dynamic_texture_.tex_animation_finished_.at(layer_number);
    }

    void base_mesh::set_position(const D3DXVECTOR3 &position)
    {
        position_ = position;
    }

    void base_mesh::set_rotation(const D3DXVECTOR3 &rotation)
    {
        rotation_ = rotation;
    }

    void base_mesh::set_animation(const string &animation_set)
    {
        animation_strategy_->set_animation(animation_set);
    }

    void base_mesh::set_default_animation(const string &animation_name)
    {
        animation_strategy_->set_default_animation(animation_name);
    }

    void base_mesh::set_animation_config(
        const string &animation_name,
        const bool &loop,
        const float &duration)
    {
        animation_strategy_->set_animation_config(animation_name, loop, duration);
    }

    void base_mesh::render(
        const D3DXMATRIX &view_matrix,
        const D3DXMATRIX &projection_matrix,
        const D3DXVECTOR4 &light_normal,
        const float &brightness)
    {
        effect_->SetVector(light_normal_handle_, &light_normal);
        effect_->SetFloat(brightness_handle_, brightness);

        if (dynamic_texture_.texture_shaker_)
        {
            (*dynamic_texture_.texture_shaker_)(*this);
        }
        if (dynamic_texture_.texture_fader_)
        {
            (*dynamic_texture_.texture_fader_)(*this);
        }

        effect_->SetVectorArray(
            texture_position_handle_,
            &dynamic_texture_.positions_[0],
            dynamic_texture::LAYER_NUMBER);

        effect_->SetFloatArray(
            texture_opacity_handle_,
            &dynamic_texture_.opacities_[0],
            dynamic_texture::LAYER_NUMBER);

        for (std::size_t i{}; i < dynamic_texture::LAYER_NUMBER; ++i)
        {
            if (dynamic_texture_.writer_.at(i))
            {
                bool is_finished = (*dynamic_texture_.writer_.at(i))();
                if (is_finished)
                {
                    dynamic_texture_.tex_animation_finished_.at(i) = true;
                }
            }
        }
        render(view_matrix, projection_matrix);
    }

    void base_mesh::set_shake_texture()
    {
        dynamic_texture_.texture_shaker_.reset(
            new_crt base_mesh::dynamic_texture::texture_shaker());
    }

    const std::array<
        D3DXVECTOR2,
        base_mesh::dynamic_texture::texture_shaker::SHAKE_POSITIONS_SIZE>
        base_mesh::dynamic_texture::texture_shaker::SHAKING_POSITIONS{
            D3DXVECTOR2{0.0f, 0.0f},
            D3DXVECTOR2{0.02f, 0.02f},
            D3DXVECTOR2{0.02f, -0.01f},
            D3DXVECTOR2{-0.02f, 0.02f},
            D3DXVECTOR2{-0.01f, -0.02f},
            D3DXVECTOR2{0.01f, 0.02f},
            D3DXVECTOR2{0.02f, -0.02f},
            D3DXVECTOR2{0.0f, -0.02f},
            D3DXVECTOR2{0.0f, 0.0f},
            D3DXVECTOR2{0.02f, 0.02f},
            D3DXVECTOR2{0.02f, -0.01f},
            D3DXVECTOR2{-0.02f, 0.02f},
            D3DXVECTOR2{-0.01f, -0.02f},
            D3DXVECTOR2{0.01f, 0.02f},
            D3DXVECTOR2{0.02f, -0.02f},
            D3DXVECTOR2{0.0f, -0.02f},
        };

    const int base_mesh::dynamic_texture::texture_shaker::SHAKE_FRAME = 4;
    const int base_mesh::dynamic_texture::texture_shaker::SHAKE_DURATION = 30;

    base_mesh::dynamic_texture::texture_shaker::texture_shaker()
        : count_{0},
          current_position_{0.0f, 0.0f},
          previous_position_{0.0f, 0.0f} {}

    void base_mesh::dynamic_texture::texture_shaker::operator()(
        base_mesh &base_mesh)
    {
        if (count_ > SHAKE_DURATION)
        {
            return;
        }
        else if (count_ == SHAKE_DURATION)
        {
            for (int i{0}; i < LAYER_NUMBER; ++i)
            {
                base_mesh.set_dynamic_texture_position(i, D3DXVECTOR2{0.0f, 0.0f});
            }
            count_ = SHAKE_DURATION + 1;
            return;
        }

        if (count_ % SHAKE_FRAME == 0)
        {
            std::size_t shaking_positions_index{
                static_cast<std::size_t>(count_ / SHAKE_FRAME % (SHAKE_POSITIONS_SIZE - 1))};
            previous_position_ = SHAKING_POSITIONS.at(shaking_positions_index);
            current_position_ = SHAKING_POSITIONS.at(shaking_positions_index + 1);
        }

        // 1/4 -> 2/4 -> 3/4 -> 4/4 -> 1/4 -> 2/4 ->...
        float loop_counter{static_cast<float>(count_ % SHAKE_FRAME + 1) / SHAKE_FRAME};
        for (int i{}; i < LAYER_NUMBER; ++i)
        {
            base_mesh.set_dynamic_texture_position(
                i,
                previous_position_ + (current_position_ - previous_position_) * loop_counter);
        }

        ++count_;
    }

    const int base_mesh::dynamic_texture::texture_fader::FADE_DURATION =
        static_cast<int>(1.0f / constants::ANIMATION_SPEED);

    void base_mesh::set_fade_in()
    {
        dynamic_texture_.tex_animation_finished_.at(dynamic_texture::FADE_LAYER) = false;
        dynamic_texture_.texture_fader_.reset(
            new_crt dynamic_texture::texture_fader(
                dynamic_texture::texture_fader::fade_type::FADE_IN));
    }

    void base_mesh::set_fade_out()
    {
        dynamic_texture_.tex_animation_finished_.at(dynamic_texture::FADE_LAYER) = false;
        dynamic_texture_.texture_fader_.reset(
            new_crt dynamic_texture::texture_fader(
                dynamic_texture::texture_fader::fade_type::FADE_OUT));
    }

    base_mesh::dynamic_texture::texture_fader::texture_fader(
        const base_mesh::dynamic_texture::texture_fader::fade_type &fade_type)
        : count_{0},
          fade_type_{fade_type} {}

    void base_mesh::dynamic_texture::texture_fader::operator()(base_mesh &base_mesh)
    {
        if (fade_type_ == fade_type::FADE_IN)
        {
            if (count_ > FADE_DURATION)
            {
                base_mesh.dynamic_texture_.tex_animation_finished_.at(FADE_LAYER) = true;
                return;
            }
            else if (count_ == FADE_DURATION)
            {
                base_mesh.set_dynamic_texture_opacity(FADE_LAYER, 0.0f);
                count_ = FADE_DURATION + 1;
                return;
            }
            else
            {
                base_mesh.set_dynamic_texture_opacity(
                    FADE_LAYER, 1.0f - static_cast<float>(count_) / FADE_DURATION);
            }
        }
        else if (fade_type_ == fade_type::FADE_OUT)
        {
            if (count_ > FADE_DURATION)
            {
                base_mesh.dynamic_texture_.tex_animation_finished_.at(FADE_LAYER) = true;
                return;
            }
            else if (count_ == FADE_DURATION)
            {
                base_mesh.set_dynamic_texture_opacity(FADE_LAYER, 1.0f);
                count_ = FADE_DURATION + 1;
                return;
            }
            else
            {
                base_mesh.set_dynamic_texture_opacity(
                    FADE_LAYER, static_cast<float>(count_) / FADE_DURATION);
            }
        }
        ++count_;
    }

} // namespace early_go
