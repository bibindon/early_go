#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "stdafx.hpp"

namespace early_go {
class camera {
public:
  camera(const ::D3DXVECTOR3&,
         const ::D3DXVECTOR3&);
  void operator()();
  void move_position(const D3DXVECTOR3&);
  ::D3DXMATRIX get_view_matrix();
  ::D3DXMATRIX get_projection_matrix();
  void set_to_behind_animation();
  void set_to_close_up_animation();
private:
  const static ::D3DXVECTOR3 UPWARD;
  ::D3DXVECTOR3 eye_position_;
  ::D3DXVECTOR3 look_at_position_;
  float         view_angle_;

  class key_animation {
  public:
    // Return true when an animation is finished.
    virtual bool operator()(camera&) = 0;
  private:

  };
  class teleport : public key_animation {
  public:
    teleport(const ::D3DXVECTOR3&, const ::D3DXVECTOR3&, const float&);
    bool operator()(camera&) override;
  private:
    const ::D3DXVECTOR3 eye_position_;
    const ::D3DXVECTOR3 look_at_position_;
    const float         view_angle_;
  };
  class transfer : public key_animation {
  public:
    transfer(const ::D3DXVECTOR3&, const ::D3DXVECTOR3&, const float&,
             const ::D3DXVECTOR3&, const ::D3DXVECTOR3&, const float&,
             const int&);
    bool operator()(camera&) override;
  private:
    int                 animation_count_;
    const ::D3DXVECTOR3 start_eye_position_;
    const ::D3DXVECTOR3 start_look_at_position_;
    const float         start_view_angle_;
    const ::D3DXVECTOR3 delta_eye_position_;
    const ::D3DXVECTOR3 delta_look_at_position_;
    const float         delta_view_angle_;
    const int           duration_;
  };
  class orbit : public key_animation {
  public:
    orbit(
        const ::D3DXVECTOR3&, const ::D3DXVECTOR3&, const float&, const float&,
        const ::D3DXVECTOR3&, const ::D3DXVECTOR3&, const float&, const float&,
        const int&);
    bool operator()(camera&) override;
  private:
    int                 animation_count_;
    const ::D3DXVECTOR3 start_eye_position_;
    const ::D3DXVECTOR3 start_look_at_position_;
    const float         start_view_angle_;
    const float         start_rotate_angle_;
    const ::D3DXVECTOR3 center_coordinate_;
    const ::D3DXVECTOR3 delta_look_at_position_;
    const float         delta_view_angle_;
    const float         delta_rotate_angle_;
    const int           duration_;
    float               radius_;
  };
  std::queue<std::shared_ptr<key_animation> > animation_queue_;
};
}

#endif
