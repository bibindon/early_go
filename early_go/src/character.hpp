#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "stdafx.hpp"
#include "base_mesh.hpp"
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/at_key.hpp>

namespace early_go {

class base_mesh;

class character
{
public:
  character(const std::shared_ptr<::IDirect3DDevice9>&,
            const ::D3DXVECTOR3&,
            const ::D3DXVECTOR3&,
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
  void set_rotation(const ::D3DXVECTOR3&);
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
                           const ::RECT& = {
                               0,
                               0,
                               static_cast<::LONG>(
                                   base_mesh::TEXTURE_PIXEL_SIZE) - 1,
                               static_cast<::LONG>(
                                   base_mesh::TEXTURE_PIXEL_SIZE) - 1 },
                           const int& = RGB(0xff, 0xff, 0xff),
                           const std::string& = "‚l‚r ‚oƒSƒVƒbƒN",
                           const int& = 40,
                           const int& = 0);
  void set_dynamic_message_color(
      const std::string&, const int&, const ::D3DXVECTOR4&);

  void set_animation(const std::string&);

  void set_shake_texture(const std::string&);
  void set_fade_in(const std::string&);
  void set_fade_out(const std::string&);
  enum DIRECTION {
    FRONT,
    LEFT,
    BACK,
    RIGHT,
    NONE,
  };
  void set_step_action(const DIRECTION&);
  void set_rotate_action(const DIRECTION&);
private:
  std::string create_animation_fullname(const std::string&, const std::string&);
  std::unordered_map<std::string, std::shared_ptr<base_mesh> > mesh_map_;
  std::unordered_map<std::string, float>     duration_map_;
  const std::shared_ptr<::IDirect3DDevice9>& d3d_device_;

  struct action {
    action(character& outer, const DIRECTION&);
    virtual bool operator()() = 0;
    virtual ~action(){}
    int              count_;
    character&       outer_;
    const DIRECTION  direction_;
  };

  struct step : public action {
    step(character&, const DIRECTION&);
    bool operator()() override;
    std::vector<float> destinations_;
  };
  struct rotate : public action {
    rotate(character&, const DIRECTION&);
    bool operator()() override;
    ~rotate();
  };
  struct step_and_rotate : public action {
    step_and_rotate(character&, const DIRECTION&, const DIRECTION&);
    bool operator()() override;
    ~step_and_rotate();
    step step_;
    rotate rotate_;
  };

  std::shared_ptr<action> current_action_;
  std::shared_ptr<action> next_action_;

  ::D3DXVECTOR3 position_;

  struct x{};
  struct y{};
  struct z{};

  boost::fusion::map<
      boost::fusion::pair<x, int>,
      boost::fusion::pair<y, int>,
      boost::fusion::pair<z, int>
  > grid_position_;

  ::D3DXVECTOR3 rotation_;
  DIRECTION     direction_;
  float         size_;
};
} /* namespace early_go */

#endif
