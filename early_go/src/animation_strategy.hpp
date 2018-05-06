#ifndef ANIMATION_STRATEGY_HPP
#define ANIMATION_STRATEGY_HPP
#include "stdafx.hpp"

namespace early_go {

struct animation_strategy {
  std::vector<
      std::unique_ptr<
          ::ID3DXAnimationSet, custom_deleter
      >
  > vecup_animation_set_;

  std::unique_ptr<::ID3DXAnimationController, custom_deleter>
      up_d3dx_animation_controller_;

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
    this->up_d3dx_animation_controller_.reset(a);
    ::DWORD dw_animation_number{
        this->up_d3dx_animation_controller_->GetNumAnimationSets()};

    std::vector<std::unique_ptr<::ID3DXAnimationSet, custom_deleter> >
        temp_vecup(dw_animation_number);

    this->vecup_animation_set_.swap(temp_vecup);

    for (::DWORD i{}; i < dw_animation_number; ++i) {
      ::LPD3DXANIMATIONSET p_temp{};
      this->up_d3dx_animation_controller_->GetAnimationSet(i, &p_temp);
      this->vecup_animation_set_.at(i).reset(p_temp);
    }
  }

  void operator()(const std::size_t& a_kr_animation_set) override {
    if (a_kr_animation_set >= this->vecup_animation_set_.size()) {
      BOOST_THROW_EXCEPTION(
          custom_exception{"An illegal animation set was sent."});
    }
    this->up_d3dx_animation_controller_->SetTrackAnimationSet(
        0, this->vecup_animation_set_.at(a_kr_animation_set).get());
  };
  void operator()(const std::string& a_kr_animation_set) override {
    std::vector<
        std::unique_ptr<
            ::ID3DXAnimationSet, custom_deleter
        >
    >::const_iterator kit;

    kit = std::find_if(
        this->vecup_animation_set_.cbegin(),
        this->vecup_animation_set_.cend(),
        [&](const std::unique_ptr<::ID3DXAnimationSet, custom_deleter>& a){
          return a_kr_animation_set == a->GetName();
    });

    if (this->vecup_animation_set_.cend() == kit) {
      BOOST_THROW_EXCEPTION(
          custom_exception{"An illegal animation set was sent."});
    }

    this->up_d3dx_animation_controller_->SetTrackAnimationSet(0, kit->get());
  };
};
}

#endif
