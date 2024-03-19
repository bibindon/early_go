#include "stdafx.hpp"

#include "camera.hpp"

namespace early_go
{
const D3DXVECTOR3 camera::UPWARD{0.0f, 1.0f, 0.0f};

camera::camera(const D3DXVECTOR3 &eye_position,
               const D3DXVECTOR3 &look_at_position)
    : eye_position_{eye_position},
      look_at_position_{look_at_position},
      view_angle_{D3DX_PI / 4},
      animation_queue_{}
{
}

void camera::operator()()
{
    if (flexible_animation_ != nullptr)
    {
        if ((*flexible_animation_)(*this))
        {
            flexible_animation_.reset();
        }
    }
    else if (!animation_queue_.empty())
    {
        if ((*animation_queue_.front())(*this))
        {
            animation_queue_.pop();
        }
    }
}

// for debug
void camera::move_position(const D3DXVECTOR3 &delta)
{
    eye_position_ += delta;
    look_at_position_ += delta;
}

void camera::move_position(const D3DXVECTOR3 &delta, const float &duration)
{
    D3DXVECTOR3 remain{0.0f, 0.0f, 0.0f};
    if (flexible_animation_ != nullptr)
    {
        remain = flexible_animation_->start_eye_position_ +
                 flexible_animation_->delta_eye_position_ - eye_position_;
    }

    flexible_animation_.reset(new_crt flexible_animation{
        0, eye_position_, look_at_position_,
        delta + remain, delta + remain, duration});
}

bool camera::flexible_animation::operator()(camera &a_camera)
{
    ++animation_count_;
    if (duration_ < animation_count_ * constants::ANIMATION_SPEED)
    {
        return true;
    }

    float sine_curve{get_sine_curve(
        animation_count_ * constants::ANIMATION_SPEED, duration_)};

    a_camera.eye_position_ =
        start_eye_position_ + delta_eye_position_ * sine_curve;

    a_camera.look_at_position_ =
        start_look_at_position_ + delta_look_at_position_ * sine_curve;

    return false;
}

D3DXVECTOR3 camera::get_position() const
{
    return eye_position_;
}

D3DXMATRIX camera::get_view_matrix()
{
    D3DXMATRIX view_matrix{};
    D3DXMatrixLookAtLH(&view_matrix,
                       &eye_position_,
                       &look_at_position_,
                       &UPWARD);
    return view_matrix;
}

D3DXMATRIX camera::get_projection_matrix()
{
    D3DXMATRIX projection_matrix{};
    D3DXMatrixPerspectiveFovLH(
        &projection_matrix,
        view_angle_,
        static_cast<float>(constants::WINDOW_WIDTH) / constants::WINDOW_HEIGHT,
        0.1f,
        3000.0f);
    return projection_matrix;
}

void camera::set_to_behind_animation()
{
    std::queue<std::shared_ptr<key_animation>>().swap(animation_queue_); // clear

    std::shared_ptr<key_animation> tmp{
        new_crt teleport{D3DXVECTOR3{0.0f, 1.2998f, 0.3f},
                         D3DXVECTOR3{0.0f, 1.2998f, -1.0f},
                         D3DX_PI / 100}};
    animation_queue_.push(tmp);

    tmp = std::shared_ptr<key_animation>(
        new_crt transfer{D3DXVECTOR3{0.0f, 1.2998f, 0.3f},
                         D3DXVECTOR3{0.0f, 1.2998f, -1.0f},
                         D3DX_PI / 100,
                         D3DXVECTOR3{0.0f, 2.0f, 3.95f},
                         D3DXVECTOR3{0.0f, 1.2998f, 0.0f},
                         D3DX_PI / 20,
                         4.0f});
    animation_queue_.push(tmp);

    tmp = std::shared_ptr<key_animation>(
        new_crt orbit{D3DXVECTOR3{0.0f, 2.0f, 3.95f},
                      D3DXVECTOR3{0.0f, 1.2998f, 0.0f},
                      D3DX_PI / 20,
                      D3DX_PI / 2,
                      D3DXVECTOR3{0.0f, 1.2998f, 0.0f},
                      D3DXVECTOR3{0.0f, 1.2998f, 0.0f},
                      D3DX_PI / 4,
                      D3DX_PI * 3 / 2,
                      4.0f});
    animation_queue_.push(tmp);
}

void camera::set_to_close_up_animation()
{
    set_to_close_up_animation(D3DXVECTOR3{0.0f, 0.0f, 0.0f});
}

void camera::set_to_close_up_animation(const D3DXVECTOR3 &pos)
{
    std::queue<std::shared_ptr<key_animation>>().swap(animation_queue_); // clear

    std::shared_ptr<key_animation> tmp = std::shared_ptr<key_animation>(
        new_crt orbit{D3DXVECTOR3{pos.x + 0.0f, 2.0f, pos.z - 3.95f},
                      D3DXVECTOR3{pos.x + 0.0f, 1.2998f, pos.z + 0.0f},
                      D3DX_PI / 4,
                      D3DX_PI * 3 / 2,
                      D3DXVECTOR3{pos.x + 0.0f, 1.2998f, pos.z + 0.0f},
                      D3DXVECTOR3{pos.x + 0.0f, 1.2998f, pos.z + 0.0f},
                      D3DX_PI / 20,
                      D3DX_PI / 2,
                      4.0f});
    animation_queue_.push(tmp);

    tmp = std::shared_ptr<key_animation>(
        new_crt transfer{D3DXVECTOR3{pos.x + 0.0f, 2.0f, pos.z + 3.95f},
                         D3DXVECTOR3{pos.x + 0.0f, 1.2998f, pos.z + 0.0f},
                         D3DX_PI / 20,
                         D3DXVECTOR3{pos.x + 0.0f, 1.2998f, pos.z + 0.3f},
                         D3DXVECTOR3{pos.x + 0.0f, 1.2998f, pos.z - 1.0f},
                         D3DX_PI / 100,
                         4.0f});
    animation_queue_.push(tmp);

    tmp = std::shared_ptr<key_animation>(
        new_crt teleport{D3DXVECTOR3{pos.x + 0.0f, 1.2998f, pos.z + 0.3f},
                         D3DXVECTOR3{pos.x + 0.0f, 1.2998f, pos.z - 1.0f},
                         D3DX_PI / 100});
    animation_queue_.push(tmp);
}

camera::teleport::teleport(const D3DXVECTOR3 &eye_position,
                           const D3DXVECTOR3 &look_at_position,
                           const float &view_angle)
    : eye_position_{eye_position},
      look_at_position_{look_at_position},
      view_angle_{view_angle} {}

bool camera::teleport::operator()(camera &camera)
{
    camera.eye_position_ = eye_position_;
    camera.look_at_position_ = look_at_position_;
    camera.view_angle_ = view_angle_;
    return true;
}

camera::transfer::transfer(const D3DXVECTOR3 &start_eye_position,
                           const D3DXVECTOR3 &start_look_at_position,
                           const float &start_view_angle,
                           const D3DXVECTOR3 &goal_eye_position,
                           const D3DXVECTOR3 &goal_look_at_position,
                           const float &goal_view_angle,
                           const float &duration)
    : animation_count_{},
      start_eye_position_{start_eye_position},
      start_look_at_position_{start_look_at_position},
      start_view_angle_{start_view_angle},
      delta_eye_position_{goal_eye_position - start_eye_position},
      delta_look_at_position_{goal_look_at_position - start_look_at_position},
      delta_view_angle_{goal_view_angle - start_view_angle},
      duration_{duration} {}

bool camera::transfer::operator()(camera &camera)
{
    ++animation_count_;
    if (animation_count_ * constants::ANIMATION_SPEED > duration_)
    {
        return true;
    }

    float sine_curve{get_sine_curve(
        animation_count_ * constants::ANIMATION_SPEED, duration_)};

    camera.eye_position_ = start_eye_position_ + delta_eye_position_ * sine_curve;

    camera.look_at_position_ =
        start_look_at_position_ + delta_look_at_position_ * sine_curve;

    camera.view_angle_ = start_view_angle_ + delta_view_angle_ * sine_curve;

    return false;
}

camera::orbit::orbit(const D3DXVECTOR3 &start_eye_position,
                     const D3DXVECTOR3 &start_look_at_position,
                     const float &start_view_angle,
                     const float &start_rotate_angle,
                     const D3DXVECTOR3 &center_coordinate,
                     const D3DXVECTOR3 &goal_look_at_position,
                     const float &goal_view_angle,
                     const float &goal_rotate_angle,
                     const float &duration)
    : animation_count_{},
      start_eye_position_{start_eye_position},
      start_look_at_position_{start_look_at_position},
      start_view_angle_{start_view_angle},
      start_rotate_angle_{start_rotate_angle},
      center_coordinate_{center_coordinate},
      delta_look_at_position_{goal_look_at_position - start_look_at_position},
      delta_view_angle_{goal_view_angle - start_view_angle},
      delta_rotate_angle_{goal_rotate_angle - start_rotate_angle},
      duration_{duration}
{
    D3DXVECTOR3 length{start_eye_position_ - center_coordinate_};
    radius_ = D3DXVec3Length(&length);
}

bool camera::orbit::operator()(camera &camera)
{
    ++animation_count_;
    if (animation_count_ * constants::ANIMATION_SPEED > duration_)
    {
        return true;
    }

    float sine_curve{get_sine_curve(
        animation_count_ * constants::ANIMATION_SPEED, duration_)};

    float current_rotate_angle{
        start_rotate_angle_ + delta_rotate_angle_ * sine_curve};

    camera.eye_position_.x =
        std::cos(current_rotate_angle) * radius_ + center_coordinate_.x;

    camera.eye_position_.y = start_eye_position_.y;

    camera.eye_position_.z =
        std::sin(current_rotate_angle) * radius_ + center_coordinate_.z;

    camera.look_at_position_ =
        start_look_at_position_ + delta_look_at_position_ * sine_curve;

    camera.view_angle_ = start_view_angle_ + delta_view_angle_ * sine_curve;

    return false;
}

}
