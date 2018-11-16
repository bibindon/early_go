#ifndef ANIMATION_STRATEGY_HPP
#define ANIMATION_STRATEGY_HPP
#include "stdafx.hpp"

namespace early_go {

class animation_strategy {
public:
  virtual void operator()() = 0;
  virtual void set_animation(const std::size_t&){};
  virtual void set_animation(const std::string&){};
  virtual void set_default_animation(const std::string&){};
  virtual void set_animation_config(const std::string&,
                                    const bool&,
                                    const float&){};

protected:
  std::vector<
      std::unique_ptr<
          ::ID3DXAnimationSet, custom_deleter
      >
  > animation_sets_;

  std::unique_ptr<::ID3DXAnimationController, custom_deleter>
      animation_controller_;
};

class no_animation : public animation_strategy {
public:
  void operator()() override
  {
    // do nothing
  };
};

class normal_animation : public animation_strategy {
public:
  normal_animation(::LPD3DXANIMATIONCONTROLLER controller)
  : default_animation_{""},
    animation_time_{},
    is_playing_{false},
    playing_animation_{""}
  {
    animation_controller_.reset(controller);
    ::DWORD animation_count{animation_controller_->GetNumAnimationSets()};

    std::vector<std::unique_ptr<::ID3DXAnimationSet, custom_deleter> >
        animation_sets(animation_count);

    animation_sets_.swap(animation_sets);

    for (::DWORD i{}; i < animation_count; ++i) {
      ::LPD3DXANIMATIONSET temp_animation_set{};
      animation_controller_->GetAnimationSet(i, &temp_animation_set);
      animation_sets_.at(i).reset(temp_animation_set);
    }
  }

  void set_animation(const std::string& animation_set) override
  {
    std::vector<
        std::unique_ptr<
            ::ID3DXAnimationSet, custom_deleter
        >
    >::const_iterator kit;

    kit = std::find_if(
        animation_sets_.cbegin(),
        animation_sets_.cend(),
        [&](const std::unique_ptr<::ID3DXAnimationSet, custom_deleter>& a){
          return animation_set == a->GetName();
    });

    if (animation_sets_.cend() == kit) {
      THROW_WITH_TRACE("An illegal animation set was sent.: " + animation_set);
    }

    animation_controller_->SetTrackAnimationSet(0, kit->get());
    animation_controller_->SetTrackPosition(0, 0.0f);

    if (animation_configs_.find(animation_set) == animation_configs_.end()) {
      return;
    }
    if (animation_set != default_animation_ &&
        !animation_configs_.at(animation_set).loop_) {
      is_playing_ = true;
      playing_animation_ = animation_set;
    }
  };
  void operator()() override
  {
    animation_controller_->AdvanceTime(constants::ANIMATION_SPEED, nullptr);
    if (is_playing_) {
      animation_time_ += constants::ANIMATION_SPEED;
      float duration{animation_configs_.at(playing_animation_).duration_};
      if (animation_time_ >= duration) {
        set_animation(default_animation_);
        is_playing_ = false;
        animation_time_ = 0;
      }
    }
  };
  void set_default_animation(const std::string& animation_name) override
  {
    default_animation_ = animation_name;
    set_animation(default_animation_);
  }
  void set_animation_config(const std::string& animation_name,
                            const bool&        loop,
                            const float&       duration) override
  {
    animation_configs_.emplace(
        animation_name, animation_config{loop, duration});
  }

private:
  std::string default_animation_;
  float animation_time_;
  bool is_playing_;
  std::string playing_animation_;

  struct animation_config {
    bool  loop_{true};
    float duration_{1.0f};
  };
  std::unordered_map<std::string, animation_config> animation_configs_;
};
}

#endif
