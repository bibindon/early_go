#include "stdafx.hpp"

#include <regex>
#include <cctype>
#include <typeinfo>

#include "base_mesh.hpp"
#include "basic_window.hpp"
#include "character.hpp"

namespace early_go {
character::character(const std::shared_ptr<::IDirect3DDevice9>& d3d_device,
                     const ::D3DXVECTOR3&                       position,
                     const ::D3DXVECTOR3&                       rotation,
                     const float&                               size)
  : d3d_device_{d3d_device},
    current_action_{nullptr},
    next_action_{nullptr},
    position_{position},
    grid_position_{0, 0, 0},
    rotation_{rotation},
    direction_{DIRECTION::FRONT},
    size_{size}
{
}

character::~character()
{
}

void character::set_default_animation(const std::string& animation_name)
{
  for (const auto& x : mesh_map_) {
    std::string animation_fullname{
        create_animation_fullname(animation_name, x.first)};

    x.second->set_default_animation(animation_fullname);
  }
}

void character::set_animation_config(const std::string& animation_name,
                                     const bool&        loop,
                                     const float&       duration)
{
  duration_map_.emplace(animation_name, duration);
  for (const auto& x : mesh_map_) {
    std::string animation_fullname{
        create_animation_fullname(animation_name, x.first)};

    x.second->set_animation_config(animation_fullname, loop, duration);
  }
}

void character::render(const ::D3DXMATRIX&  view_matrix,
                       const ::D3DXMATRIX&  projection_matrix,
                       const ::D3DXVECTOR4& normal_light,
                       const float&         brightness)
{
  if (current_action_ == nullptr && next_action_ != nullptr) {
    current_action_.swap(next_action_);
    next_action_.reset();
    (*current_action_)();
  } else if (current_action_ != nullptr && next_action_!= nullptr &&
      current_action_->count_ >= constants::ACTION_INTERVAL_FRAME) {
    current_action_.swap(next_action_);
    next_action_.reset();
    (*current_action_)();
  } else if (current_action_ != nullptr && !(*current_action_)()) {
    current_action_.swap(next_action_);
    next_action_.reset();
    if (current_action_ != nullptr) {
      (*current_action_)();
    }
  }

  for (const auto& x : mesh_map_) {
    x.second->render(view_matrix, projection_matrix, normal_light, brightness);
  }
}

void character::set_position(const ::D3DXVECTOR3& position)
{
  position_ = position;
  for (const auto& x : mesh_map_) {
    x.second->set_position(position_);
  }
}

void character::set_rotation(const ::D3DXVECTOR3& rotation)
{
  rotation_ = rotation;
  for (const auto& x : mesh_map_) {
    x.second->set_rotation(rotation_);
  }
}

void character::set_dynamic_texture(const std::string& x_filename,
                                    const std::string& texture_filename,
                                    const int&         layer_number,
                                    const base_mesh::combine_type& combine_type)
{
  if (mesh_map_.find(x_filename) != mesh_map_.end()) {
    mesh_map_.at(x_filename)->set_dynamic_texture(
        texture_filename, layer_number, combine_type);
  }
}

void character::set_dynamic_texture_position(const std::string&   x_filename,
                                             const int&           layer_number,
                                             const ::D3DXVECTOR2& position)
{
  if (mesh_map_.find(x_filename) != mesh_map_.end()) {
    mesh_map_.at(x_filename)->set_dynamic_texture_position(
        layer_number, position);
  }
}

void character::set_dynamic_texture_opacity(const std::string& x_filename,
                                            const int&         layer_number,
                                            const float&       opacity)
{
  if (mesh_map_.find(x_filename) != mesh_map_.end()) {
    mesh_map_.at(x_filename)->set_dynamic_texture_opacity(
        layer_number, opacity);
  }
}

void character::flip_dynamic_texture(const std::string& x_filename,
                                     const int&         layer_number)
{
  if (mesh_map_.find(x_filename) != mesh_map_.end()) {
    mesh_map_.at(x_filename)->flip_dynamic_texture(layer_number);
  }
}

void character::clear_dynamic_texture(const std::string& x_filename,
                                      const int&         layer_number)
{
  if (mesh_map_.find(x_filename) != mesh_map_.end()) {
    mesh_map_.at(x_filename)->clear_dynamic_texture(layer_number);
  }
}

void character::set_dynamic_message(const std::string& x_filename,
                                    const int&         layer_number,
                                    const std::string& message,
                                    const bool&        animation,
                                    const ::RECT&      rect,
                                    const int&         color,
                                    const std::string& fontname,
                                    const int&         size,
                                    const int&         weight)
{
  if (mesh_map_.find(x_filename) != mesh_map_.end()) {
    mesh_map_.at(x_filename)->set_dynamic_message(
        layer_number, message, animation, rect, color, fontname, size, weight);
  }
}

void character::set_dynamic_message_color(const std::string& x_filename,
                                          const int& layer_number,
                                          const::D3DXVECTOR4& color)
{
  if (mesh_map_.find(x_filename) != mesh_map_.end()) {
    mesh_map_.at(x_filename)->set_dynamic_message_color(layer_number, color);
  }
}

void character::set_animation(const std::string& animation_set)
{
  for (const auto& x : mesh_map_) {
    std::string animation_fullname{
        create_animation_fullname(animation_set, x.first)};

    x.second->set_animation(animation_fullname);
  }
}

void character::set_shake_texture(const std::string& x_filename)
{
  if (mesh_map_.find(x_filename) != mesh_map_.end()) {
    mesh_map_.at(x_filename)->set_shake_texture();
  }
}

void character::set_fade_in(const std::string& x_filename)
{
  if (mesh_map_.find(x_filename) != mesh_map_.end()) {
    mesh_map_.at(x_filename)->set_fade_in();
  }
}

void character::set_fade_out(const std::string& x_filename)
{
  if (mesh_map_.find(x_filename) != mesh_map_.end()) {
    mesh_map_.at(x_filename)->set_fade_out();
  }
}

void character::set_step_action(const character::DIRECTION& step_dir)
{
  if (next_action_ == nullptr) {
    next_action_.reset(new_crt step{*this, step_dir});
  } else {
    if (typeid(*next_action_.get()) == typeid(rotate)) {
      DIRECTION rotate_dir = next_action_->direction_;
      next_action_.reset(new_crt step_and_rotate{*this, step_dir, rotate_dir});
    }
  }
}

void character::set_rotate_action(const character::DIRECTION& rotate_dir)
{
  if (direction_ == rotate_dir) {
    return;
  }
  if (next_action_ == nullptr) {
    next_action_.reset(new_crt rotate{*this, rotate_dir});
  } else {
    if (typeid(*next_action_.get()) == typeid(step)) {
      DIRECTION step_dir = next_action_->direction_;
      next_action_.reset(new_crt step_and_rotate{*this, step_dir, rotate_dir});
    }

  }
}

// "Idle" + "hoge/piyo/hair.x" -> "Idle_Hair"
std::string character::create_animation_fullname(
    const std::string& former_name, const std::string& model_fullname)
{
  std::regex reg{R"(.*/(.*)\.x)"};
  std::smatch match{};
  std::string model_filename{};
  std::string animation_fullname{};

  if (!std::regex_match(model_fullname, match, reg) || match.size() != 2) {
    THROW_WITH_TRACE("A model filename doesn't match regular expression.: " +
        model_fullname);
  } else {
    model_filename = match[1].str();
    unsigned char c = std::toupper(model_filename.at(0));
    model_filename.erase(0, 1);
    model_filename.insert(model_filename.begin(), c);
    animation_fullname = former_name + "_" + model_filename;
  }
  return animation_fullname;
}

character::action::action(character& outer, const DIRECTION& direction)
  : count_{},
    outer_{outer},
    direction_{direction} {}

character::step::step(character& outer, const DIRECTION& direction)
  : action{outer, direction}
{
  for (int i{}; i < constants::ACTION_INTERVAL_FRAME+1; ++i) {
    float y = std::sqrt(static_cast<float>(i)/constants::ACTION_INTERVAL_FRAME)
        * constants::GRID_LENGTH;
    destinations_.push_back(y);
  }
}

bool character::step::operator()()
{
  if (count_ >= constants::ACTION_INTERVAL_FRAME) {
    return false;
  }
  if (count_ == 0) {
    switch (direction_) {
    case DIRECTION::FRONT:
      boost::fusion::at_key<z>(outer_.grid_position_) += 1;
      switch (outer_.direction_) {
      case DIRECTION::FRONT:
        outer_.set_animation("Step_Front");
        break;
      case DIRECTION::LEFT:
        outer_.set_animation("Step_Right");
        break;
      case DIRECTION::BACK:
        outer_.set_animation("Step_Back");
        break;
      case DIRECTION::RIGHT:
        outer_.set_animation("Step_Left");
        break;
      }
      break;
    case DIRECTION::LEFT:
      boost::fusion::at_key<x>(outer_.grid_position_) -= 1;
      switch (outer_.direction_) {
      case DIRECTION::FRONT:
        outer_.set_animation("Step_Left");
        break;
      case DIRECTION::LEFT:
        outer_.set_animation("Step_Front");
        break;
      case DIRECTION::BACK:
        outer_.set_animation("Step_Right");
        break;
      case DIRECTION::RIGHT:
        outer_.set_animation("Step_Back");
        break;
      }
      break;
    case DIRECTION::BACK:
      boost::fusion::at_key<z>(outer_.grid_position_) -= 1;
      switch (outer_.direction_) {
      case DIRECTION::FRONT:
        outer_.set_animation("Step_Back");
        break;
      case DIRECTION::LEFT:
        outer_.set_animation("Step_Left");
        break;
      case DIRECTION::BACK:
        outer_.set_animation("Step_Front");
        break;
      case DIRECTION::RIGHT:
        outer_.set_animation("Step_Right");
        break;
      }
      break;
    case DIRECTION::RIGHT:
      boost::fusion::at_key<x>(outer_.grid_position_) += 1;
      switch (outer_.direction_) {
      case DIRECTION::FRONT:
        outer_.set_animation("Step_Left");
        break;
      case DIRECTION::LEFT:
        outer_.set_animation("Step_Back");
        break;
      case DIRECTION::BACK:
        outer_.set_animation("Step_Right");
        break;
      case DIRECTION::RIGHT:
        outer_.set_animation("Step_Front");
        break;
      }
      break;
    }
  }
  ::D3DXVECTOR3 position{outer_.position_};
  float delta = destinations_.at(static_cast<int64_t>(count_)+1)
      - destinations_.at(count_);
  switch (direction_) {
  case DIRECTION::FRONT:
    position.z += delta;
    break;
  case DIRECTION::LEFT:
    position.x -= delta;
    break;
  case DIRECTION::BACK:
    position.z -= delta;
    break;
  case DIRECTION::RIGHT:
    position.x += delta;
    break;
  }
  outer_.set_position(position);
  ++count_;
  return true;
}

character::rotate::rotate(character& outer, const DIRECTION& direction)
  : action{outer, direction} { }

bool character::rotate::operator()()
{
  if ((count_)*constants::ANIMATION_SPEED >=
      outer_.duration_map_.at("Rotate_Left")) {
    return false;
  }
  if (count_ == 0) {
    switch (outer_.direction_) {
    case DIRECTION::FRONT:
      switch (direction_) {
      case DIRECTION::FRONT:
        break;
      case DIRECTION::LEFT:
        outer_.set_animation("Rotate_Left");
        break;
      case DIRECTION::BACK:
        outer_.set_animation("Rotate_Back");
        break;
      case DIRECTION::RIGHT:
        outer_.set_animation("Rotate_Right");
        break;
      }
      break;
    case DIRECTION::LEFT:
      switch (direction_) {
      case DIRECTION::FRONT:
        outer_.set_animation("Rotate_Right");
        break;
      case DIRECTION::LEFT:
        break;
      case DIRECTION::BACK:
        outer_.set_animation("Rotate_Left");
        break;
      case DIRECTION::RIGHT:
        outer_.set_animation("Rotate_Back");
        break;
      }
      break;
    case DIRECTION::BACK:
      switch (direction_) {
      case DIRECTION::FRONT:
        outer_.set_animation("Rotate_Back");
        break;
      case DIRECTION::LEFT:
        outer_.set_animation("Rotate_Right");
        break;
      case DIRECTION::BACK:
        break;
      case DIRECTION::RIGHT:
        outer_.set_animation("Rotate_Left");
        break;
      }
      break;
    case DIRECTION::RIGHT:
      switch (direction_) {
      case DIRECTION::FRONT:
        outer_.set_animation("Rotate_Left");
        break;
      case DIRECTION::LEFT:
        outer_.set_animation("Rotate_Back");
        break;
      case DIRECTION::BACK:
        outer_.set_animation("Rotate_Right");
        break;
      case DIRECTION::RIGHT:
        break;
      }
      break;
    }
    outer_.direction_ = direction_;
  }
  ++count_;
  return true;
}

character::rotate::~rotate()
{
  switch (direction_) {
  case DIRECTION::FRONT:
    outer_.set_rotation(::D3DXVECTOR3{D3DX_PI, 0.0f, 0.0f});
    break;
  case DIRECTION::LEFT:
    outer_.set_rotation(::D3DXVECTOR3{D3DX_PI/2, 0.0f, 0.0f});
    break;
  case DIRECTION::BACK:
    outer_.set_rotation(::D3DXVECTOR3{0.0f, 0.0f, 0.0f});
    break;
  case DIRECTION::RIGHT:
    outer_.set_rotation(::D3DXVECTOR3{D3DX_PI*3/2, 0.0f, 0.0f});
    break;
  }

}

character::step_and_rotate::step_and_rotate(character& outer,
                                            const DIRECTION& step_dir,
                                            const DIRECTION& rotate_dir)
  : action{outer, DIRECTION::NONE},
    step_{outer, step_dir},
    rotate_{outer, rotate_dir} { }

bool character::step_and_rotate::operator()()
{
  step_();
  bool result = rotate_();
  count_ = rotate_.count_;
  return result;
}

character::step_and_rotate::~step_and_rotate()
{
}

}
