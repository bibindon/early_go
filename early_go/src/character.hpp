#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "stdafx.hpp"
#include "base_mesh.hpp"
#include "operation.hpp"
#include <boost/variant.hpp>

namespace early_go {

class base_mesh;

class character
{
public:
  character(const std::shared_ptr<::IDirect3DDevice9>&,
            const std::shared_ptr<operation>&,
            const cv::Point3i&,
            const direction&,
            const float&);
  virtual ~character();
  template <typename T>
  void add_mesh(const std::string& xfile_name)
  {
    mesh_map_.emplace(xfile_name,
        new_crt T{d3d_device_, xfile_name, position_, rotation_, size_});
  }
  void set_default_animation(const std::string&);
  void set_animation_config(const std::string&, const bool&, const float&);

  void render(const ::D3DXMATRIX&,
      const ::D3DXMATRIX&, const ::D3DXVECTOR4&, const float&);
  inline void set_size(const float& size)
  {
    size_ = size;
  }

  void set_position(const ::D3DXVECTOR3&);
  void set_position(const cv::Point3i&);
  void set_rotation(const direction&);

  void set_dynamic_texture(const std::string&,
                           const std::string&,
                           const int&,
                           const base_mesh::combine_type&);
  void set_dynamic_texture_position(const std::string&,
                                    const int&,
                                    const ::D3DXVECTOR2&);
  void set_dynamic_texture_opacity(const std::string&,
                                   const int&,
                                   const float&);
  void flip_dynamic_texture(const std::string&,
                            const int&);
  void clear_dynamic_texture(const std::string&,
                             const int&);
  void set_dynamic_message(const std::string&,
                           const int&,
                           const std::string&,
                           const bool& = false,
                           const cv::Rect& = {
                               0,
                               0,
                               constants::TEXTURE_PIXEL_SIZE - 1,
                               constants::TEXTURE_PIXEL_SIZE - 1 },
                           const ::DWORD& = D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff),
                           const std::string& = "���S�V�b�N",
                           const int& = 40,
                           const int& = 0,
                           const ::BYTE& = SHIFTJIS_CHARSET,
                           const bool& = true);
  void set_dynamic_message_color(
      const std::string&, const int&, const ::D3DXVECTOR4&);

  void set_animation(const std::string&);

  void set_shake_texture(const std::string&);
  void set_fade_in(const std::string&);
  void set_fade_out(const std::string&);
  void set_step_action(const direction&);
  void set_rotate_action(const direction&);
  void set_step_and_rotate_action(const direction&, const direction&);
  void cancel_action();
  direction get_direction() const;
  cv::Point3i get_grid_position() const;
  ::D3DXVECTOR3 get_position() const;

  struct action {
    action(character& outer, const direction&);

    virtual operation::behavior_state operator()() = 0;
    virtual void cancel() = 0;
    virtual ~action(){}
    std::vector<
        boost::variant<direction>
    > params_;
    std::vector<boost::variant<direction> > get_params()
    {
      return params_;
    }
    int              count_;
    character&       outer_;
  };

  struct step : public action {
    step(character&, const direction&);
    operation::behavior_state operator()() override;
    void cancel() override;
    ~step() override;
    std::vector<float> destinations_;
    direction relative_direction_;
  };
  struct rotate : public action {
    rotate(character&, const direction&);
    operation::behavior_state operator()() override;
    void cancel() override;
    ~rotate() override;
    direction relative_direction_;
    const direction back_up_direction_;
  };
  struct step_and_rotate : public action {
    step_and_rotate(character&, const direction&, const direction&);
    operation::behavior_state operator()() override;
    void cancel() override;
    ~step_and_rotate() override;
    step step_;
    rotate rotate_;
  };
  struct attack : public action {
    attack(character&);
    operation::behavior_state operator()() override;
    void cancel() override;
    ~attack();
    bool availability_;
  };

  int get_health() const;
  void set_health(const int&);
  int get_max_health() const;
  void set_max_health(const int&);
  
  void set_normal_move(const std::string&, const int&, const int&);
  std::vector<std::string> get_normal_move();
  std::pair<int, int> get_normal_move_power(const std::string&);

  void set_special_move(const std::string&, const int&, const int&);
  std::vector<std::string> get_special_move();
  std::pair<int, int> get_special_move_power(const std::string&);
private:
  void set_rotation(const ::D3DXVECTOR3&);
  std::string create_animation_fullname(const std::string&, const std::string&);
  std::unordered_map<std::string, std::shared_ptr<base_mesh> > mesh_map_;
  std::unordered_map<std::string, float>     duration_map_;
  const std::shared_ptr<::IDirect3DDevice9>& d3d_device_;

  std::shared_ptr<action> current_action_;

  ::D3DXVECTOR3 position_;
  cv::Point3i grid_position_;

  ::D3DXVECTOR3 rotation_;
  direction     direction_;
  float         size_;

  const std::shared_ptr<operation>& operation_;

  int           health_;
  int           max_health_;

  struct normal_move {
    normal_move(const std::string&, const int&, const int&);
    const std::string name_;
    int power_;
    int max_power_;
  };
  std::vector<normal_move> normal_move_;

  struct special_move : normal_move {
    special_move(const std::string&, const int&, const int&);
  };
  std::vector<special_move> special_move_;
  
};
} /* namespace early_go */

#endif
