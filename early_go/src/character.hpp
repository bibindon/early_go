#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "stdafx.hpp"
#include "base_mesh.hpp"

namespace early_go {

class base_mesh;

class character
{
public:
  character(const std::shared_ptr<::IDirect3DDevice9>&,
            const ::D3DXVECTOR3&,
            const float& );
  virtual ~character();
  template <typename T>
  void add_mesh(const std::string& xfile_name)
  {
    mesh_map_.emplace(xfile_name,
        new_crt T{d3d_device_, xfile_name, position_, size_});
  }

  void render(const ::D3DXMATRIX&,
      const ::D3DXMATRIX&, const ::D3DXVECTOR4&, const float&);
  inline void set_size(const float& size)
  {
    size_ = size;
  }

  void set_position(const ::D3DXVECTOR3&);
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
  void set_dynamic_message(const std::string&,
                           const int&,
                           const std::string&,
                           const bool& = false,
                           const ::RECT& = {
                               0,
                               0,
                               base_mesh::TEXTURE_PIXEL_SIZE - 1,
                               base_mesh::TEXTURE_PIXEL_SIZE - 1 },
                           const int& = RGB(0xff, 0xff, 0xff),
                           const std::string& = "�l�r �o�S�V�b�N",
                           const int& = 40,
                           const int& = 0);
  void set_dynamic_message_color(
      const std::string&, const int&, const ::D3DXVECTOR4&);

  void play_animation_set(const std::string&);
private:
  std::unordered_map<std::string, std::shared_ptr<base_mesh> > mesh_map_;
  const std::shared_ptr<::IDirect3DDevice9>& d3d_device_;
  ::D3DXVECTOR3 position_;
  float         size_;
};
} /* namespace early_go */

#endif