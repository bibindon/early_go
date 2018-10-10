#ifndef ANIMATION_STRATEGY_HPP
#define ANIMATION_STRATEGY_HPP
#include "stdafx.hpp"

namespace early_go {

struct animation_strategy {
  std::vector<
      std::unique_ptr<
          ::ID3DXAnimationSet, custom_deleter
      >
  > animation_sets_;

  std::unique_ptr<::ID3DXAnimationController, custom_deleter>
      animation_controller_;

  virtual void operator()(const std::size_t&) = 0;
  virtual void operator()(const std::string&) = 0;
};

struct no_animation : animation_strategy {
  void operator()(const std::size_t&) override {
    // do nothing
  };
  void operator()(const std::string&) override {
    // do nothing
  };
};

struct normal_animation : animation_strategy {
  normal_animation(::LPD3DXANIMATIONCONTROLLER a) {
    animation_controller_.reset(a);
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

  void operator()(const std::size_t& animation_set) override {
    if (animation_set >= animation_sets_.size()) {
      BOOST_THROW_EXCEPTION(
          custom_exception{"An illegal animation set was sent."});
    }
    animation_controller_->SetTrackAnimationSet(
        0, animation_sets_.at(animation_set).get());
  };
  void operator()(const std::string& animation_set) override {
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
      BOOST_THROW_EXCEPTION(
          custom_exception{"An illegal animation set was sent."});
    }

    animation_controller_->SetTrackAnimationSet(0, kit->get());
  };
};
}

#endif
