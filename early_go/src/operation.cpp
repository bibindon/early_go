#include "stdafx.hpp"
#include "key.hpp"
#include "character.hpp"
#include "operation.hpp"
#include <boost/type_erasure/typeid_of.hpp>

using boost::fusion::at_key;

namespace early_go {

template<
    class     CurrentKeyAction,
    direction CurrentKeyDirection,
    class     RecentKeyAction,
    direction RecentKeyDirection,
    std::enable_if_t<
        !std::is_same<CurrentKeyAction, character::step_and_rotate>::value &&
        !std::is_same<CurrentKeyAction, character::attack>::value,
        std::nullptr_t
    > = nullptr
>
void set_state(operation&,
               character&,
               std::shared_ptr<operation::behavior_concept>&,
               std::shared_ptr<operation::behavior_concept>&){}
template<
    class     CurrentKeyAction,
    direction CurrentKeyDirection,
    class     RecentKeyAction,
    direction RecentKeyDirection,
    std::enable_if_t<
        std::is_same<CurrentKeyAction, character::step_and_rotate>::value &&
        !std::is_same<CurrentKeyAction, character::attack>::value,
        std::nullptr_t
    > = nullptr
>
void set_state(operation&,
               character&,
               std::shared_ptr<operation::behavior_concept>&,
               std::shared_ptr<operation::behavior_concept>&);
template<
    class     CurrentKeyAction,
    direction CurrentKeyDirection,
    class     RecentKeyAction,
    direction RecentKeyDirection,
    std::enable_if_t<
        !std::is_same<CurrentKeyAction, character::step_and_rotate>::value &&
        std::is_same<CurrentKeyAction, character::attack>::value,
        std::nullptr_t
    > = nullptr
>
void set_state(operation&,
               character&,
               std::shared_ptr<operation::behavior_concept>&,
               std::shared_ptr<operation::behavior_concept>&);

const std::chrono::milliseconds operation::DOUBLE_DOWN_CHANCE_FRAME
    = std::chrono::milliseconds(50);

operation::operation()
  : current_behavior_{nullptr},
    reserved_behavior_{nullptr}
{
  for (int z = 0; z < constants::GRID_NUM_HEIGHT; ++z) {
    for (int x = -(constants::GRID_NUM_WIDTH-1)/2;
        x < (constants::GRID_NUM_WIDTH+1)/2; ++x) {
      offensive_area_[z][x] = false;
    }
  }
}

void operation::set_offensive_position(const int& x, const int& z)
{
  if (offensive_area_.find(z) == offensive_area_.end()) {
    return;
  }
  if (offensive_area_.at(z).find(x) == offensive_area_.at(z).end()) {
    return;
  }
  offensive_area_.at(z).at(x) = true;
}

void operation::operator()(basic_window& outer)
{
  character& main_character{*outer.get_main_character()};

  if (key::is_down('Q')) {
    ::PostQuitMessage(0);
  }
  if (key::is_down('W')) {
    if (current_behavior_ == nullptr) {
      current_behavior_.reset(new_crt behavior_concept{
          character::step{main_character, direction::FRONT}});
    } else {
      if (check_recent_keycode('A')) {
        set_state<character::attack, direction::NONE,
                  character::step,   direction::LEFT>(
            *this, main_character, current_behavior_, reserved_behavior_);
      } else if (check_recent_keycode('I')) {
        set_state<character::step_and_rotate, direction::FRONT,
                  character::rotate,          direction::FRONT>(
            *this, main_character, current_behavior_, reserved_behavior_);
      } else if (check_recent_keycode('J')) {
        set_state<character::step_and_rotate, direction::FRONT,
                  character::rotate,          direction::LEFT>(
            *this, main_character, current_behavior_, reserved_behavior_);
      } else if (check_recent_keycode('K')) {
        set_state<character::step_and_rotate, direction::FRONT,
                  character::rotate,          direction::BACK>(
            *this, main_character, current_behavior_, reserved_behavior_);
      } else if (check_recent_keycode('L')) {
        set_state<character::step_and_rotate, direction::FRONT,
                  character::rotate,          direction::RIGHT>(
            *this, main_character, current_behavior_, reserved_behavior_);
      } else {
        reserved_behavior_.reset(new_crt behavior_concept{
            character::step{main_character, direction::FRONT}});
      }
    }
  }
  if (key::is_down('A')) {
    if (current_behavior_ == nullptr) {
      current_behavior_.reset(new_crt behavior_concept{
          character::step{main_character, direction::LEFT}});
    } else {
      if (check_recent_keycode('W')) {
        set_state<character::attack, direction::NONE,
                  character::step,   direction::FRONT>(
            *this, main_character, current_behavior_, reserved_behavior_);
      } else if (check_recent_keycode('I')) {
        set_state<character::step_and_rotate, direction::LEFT,
                  character::rotate,          direction::FRONT>(
            *this, main_character, current_behavior_, reserved_behavior_);
      } else if (check_recent_keycode('J')) {
        set_state<character::step_and_rotate, direction::LEFT,
                  character::rotate,          direction::LEFT>(
            *this, main_character, current_behavior_, reserved_behavior_);
      } else if (check_recent_keycode('K')) {
        set_state<character::step_and_rotate, direction::LEFT,
                  character::rotate,          direction::BACK>(
            *this, main_character, current_behavior_, reserved_behavior_);
      } else if (check_recent_keycode('L')) {
        set_state<character::step_and_rotate, direction::LEFT,
                  character::rotate,          direction::RIGHT>(
            *this, main_character, current_behavior_, reserved_behavior_);
      } else {
        reserved_behavior_.reset(new_crt behavior_concept{
            character::step{main_character, direction::LEFT}});
      }
    }
  }
  if (key::is_down('S')) {
    if (current_behavior_ == nullptr) {
      current_behavior_.reset(new_crt behavior_concept{
          character::step{main_character, direction::BACK}});
    } else {
      if (check_recent_keycode('I')) {
        set_state<character::step_and_rotate, direction::BACK,
                  character::rotate, direction::FRONT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('J')) {
        set_state<character::step_and_rotate, direction::BACK,
                  character::rotate, direction::LEFT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('K')) {
        set_state<character::step_and_rotate, direction::BACK,
                  character::rotate, direction::BACK>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('L')) {
        set_state<character::step_and_rotate, direction::BACK,
                  character::rotate, direction::RIGHT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else {
        reserved_behavior_.reset(new_crt behavior_concept{
            character::step{main_character, direction::BACK}});
      }
    }
  }
  if (key::is_down('D')) {
    if (current_behavior_ == nullptr) {
      current_behavior_.reset(new_crt behavior_concept{
          character::step{main_character, direction::RIGHT}});
    } else {
      if (check_recent_keycode('I')) {
        set_state<character::step_and_rotate,   direction::RIGHT,
                  character::rotate, direction::FRONT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('J')) {
        set_state<character::step_and_rotate,   direction::RIGHT,
                  character::rotate, direction::LEFT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('K')) {
        set_state<character::step_and_rotate,   direction::RIGHT,
                  character::rotate, direction::BACK>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('L')) {
        set_state<character::step_and_rotate,   direction::RIGHT,
                  character::rotate, direction::RIGHT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else {
        reserved_behavior_.reset(new_crt behavior_concept{
            character::step{main_character, direction::RIGHT}});
      }
    }
  }
  if (key::is_down('I')) {
    if (current_behavior_ == nullptr) {
      current_behavior_.reset(new_crt behavior_concept{
          character::rotate{main_character, direction::FRONT}});
    } else {
      if (check_recent_keycode('W')) {
        set_state<character::step_and_rotate, direction::FRONT,
                  character::step,   direction::FRONT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('A')) {
        set_state<character::step_and_rotate, direction::FRONT,
                  character::step,   direction::LEFT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('S')) {
        set_state<character::step_and_rotate, direction::FRONT,
                  character::step,   direction::BACK>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('D')) {
        set_state<character::step_and_rotate, direction::FRONT,
                  character::step,   direction::RIGHT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else {
        reserved_behavior_.reset(new_crt behavior_concept{
            character::rotate{main_character, direction::FRONT}});
      }
    }
  }
  if (key::is_down('J')) {
    if (current_behavior_ == nullptr) {
      current_behavior_.reset(new_crt behavior_concept{
          character::rotate{main_character, direction::LEFT}});
    } else {
      if (check_recent_keycode('W')) {
        set_state<character::step_and_rotate, direction::LEFT,
                  character::step,   direction::FRONT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('A')) {
        set_state<character::step_and_rotate, direction::LEFT,
                  character::step,   direction::LEFT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('S')) {
        set_state<character::step_and_rotate, direction::LEFT,
                  character::step,   direction::BACK>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('D')) {
        set_state<character::step_and_rotate, direction::LEFT,
                  character::step,   direction::RIGHT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else {
        reserved_behavior_.reset(new_crt behavior_concept{
            character::rotate{main_character, direction::LEFT}});
      }
    }
  }
  if (key::is_down('K')) {
    if (current_behavior_ == nullptr) {
      current_behavior_.reset(new_crt behavior_concept{
          character::rotate{main_character, direction::BACK}});
    } else {
      if (check_recent_keycode('W')) {
        set_state<character::step_and_rotate, direction::BACK,
                  character::step,   direction::FRONT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('A')) {
        set_state<character::step_and_rotate, direction::BACK,
                  character::step,   direction::LEFT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('S')) {
        set_state<character::step_and_rotate, direction::BACK,
                  character::step,   direction::BACK>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('D')) {
        set_state<character::step_and_rotate, direction::BACK,
                  character::step,   direction::RIGHT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else {
        reserved_behavior_.reset(new_crt behavior_concept{
            character::rotate{main_character, direction::BACK}});
      }
    }
  }
  if (key::is_down('L')) {
    if (current_behavior_ == nullptr) {
      current_behavior_.reset(new_crt behavior_concept{
          character::rotate{main_character, direction::RIGHT}});
    } else {
      if (check_recent_keycode('W')) {
        set_state<character::step_and_rotate, direction::RIGHT,
                  character::step,   direction::FRONT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('A')) {
        set_state<character::step_and_rotate, direction::RIGHT,
                  character::step,   direction::LEFT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('S')) {
        set_state<character::step_and_rotate, direction::RIGHT,
                  character::step,   direction::BACK>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else if (check_recent_keycode('D')) {
        set_state<character::step_and_rotate, direction::RIGHT,
                  character::step,   direction::RIGHT>(*this, main_character,
                                                       current_behavior_,
                                                       reserved_behavior_);
      } else {
        reserved_behavior_.reset(new_crt behavior_concept{
            character::rotate{main_character, direction::RIGHT}});
      }
    }
  }

  for (auto&& z : offensive_area_) {
    for (auto&& x : z.second) {
      x.second = false;
    }
  }

  if (current_behavior_ != nullptr) {
    switch ((*current_behavior_)()) {
    case behavior_state::PLAY:
      break;
    case behavior_state::CANCELABLE:
      if (reserved_behavior_ != nullptr) {
        current_behavior_ = reserved_behavior_;
        reserved_behavior_.reset();
        (*current_behavior_)();
      }
      break;
    case behavior_state::FINISH:
      if (reserved_behavior_ != nullptr) {
        current_behavior_ = reserved_behavior_;
        reserved_behavior_.reset();
        (*current_behavior_)();
      } else {
        current_behavior_.reset();
      }
      break;
    }
  }

  // collision detection
  std::shared_ptr<character> enemy = outer.get_enemy_character();

  grid_coordinate enemy_pos = enemy->get_position();
  if (offensive_area_.at(at_key<tag_z>(enemy_pos))
                     .at(at_key<tag_x>(enemy_pos))) {
    enemy->set_animation("Damaged");
    current_behavior_->cancel();
  }
}

template<
    class     CurrentKeyAction,
    direction CurrentKeyDirection,
    class     RecentKeyAction,
    direction RecentKeyDirection,
    std::enable_if_t<
        std::is_same<CurrentKeyAction, character::step_and_rotate>::value &&
        !std::is_same<CurrentKeyAction, character::attack>::value,
        std::nullptr_t
    >
>
void set_state(operation&, character& a_character,
    std::shared_ptr<operation::behavior_concept>& current_behavior,
    std::shared_ptr<operation::behavior_concept>& reserved_behavior)
{
  constexpr direction step_direction{
      std::is_same<RecentKeyAction, character::rotate>::value
          ? CurrentKeyDirection
          : RecentKeyDirection};
  constexpr direction rotate_direction{
      std::is_same<RecentKeyAction, character::rotate>::value
          ? RecentKeyDirection
          : CurrentKeyDirection};

  if (reserved_behavior != nullptr && 
      boost::type_erasure::typeid_of(*reserved_behavior) ==
          typeid(RecentKeyAction) &&
      boost::get<direction>(reserved_behavior->get_params().at(0)) ==
          RecentKeyDirection) {
    reserved_behavior.reset(new_crt operation::behavior_concept{
        character::step_and_rotate{
            a_character, step_direction, rotate_direction}});
  } else if (boost::type_erasure::typeid_of(*current_behavior) ==
          typeid(character::step_and_rotate)) {
    std::conditional_t<
        std::is_same_v<RecentKeyAction, character::step>,
        character::rotate,
        character::step
    > step_or_rotate{a_character, CurrentKeyDirection};
    reserved_behavior.reset(
        new_crt operation::behavior_concept{step_or_rotate});
  } else {
    current_behavior->cancel();
    current_behavior.reset(new_crt operation::behavior_concept{
        character::step_and_rotate{
            a_character, step_direction, rotate_direction}});
  }
}
template<
    class     CurrentKeyAction,
    direction CurrentKeyDirection,
    class     RecentKeyAction,
    direction RecentKeyDirection,
    std::enable_if_t<
        !std::is_same<CurrentKeyAction, character::step_and_rotate>::value &&
        std::is_same<CurrentKeyAction, character::attack>::value,
        std::nullptr_t
    >
>
void set_state(operation& a_operation, character& a_character,
    std::shared_ptr<operation::behavior_concept>& current_behavior,
    std::shared_ptr<operation::behavior_concept>& reserved_behavior)
{
  if (current_behavior == nullptr && reserved_behavior == nullptr) {
    current_behavior.reset(new_crt operation::behavior_concept{
        character::attack{a_character, a_operation}});
  } else if (current_behavior != nullptr && reserved_behavior == nullptr) {
    if (boost::type_erasure::typeid_of(*current_behavior) ==
            typeid(RecentKeyAction) &&
        boost::get<direction>(current_behavior->get_params().at(0)) ==
            RecentKeyDirection) {
      current_behavior->cancel();
      current_behavior.reset(new_crt operation::behavior_concept{
          character::attack{a_character, a_operation}});
    } else {
      reserved_behavior.reset(new_crt operation::behavior_concept{
          character::attack{a_character, a_operation}});
    }
  } else {
    reserved_behavior.reset(new_crt operation::behavior_concept{
        character::attack{a_character, a_operation}});
  }
}

bool operation::check_recent_keycode(const int& keycode)
{
  auto time_out_it = std::find_if(
      std::begin(key::key_deque_), std::end(key::key_deque_),
      [&] (const std::pair<
          std::chrono::system_clock::time_point,
          std::array<int, key::KEY_TABLE_LENGTH> >& x) {
        std::chrono::system_clock::time_point time_out{
            key::key_deque_.at(0).first - DOUBLE_DOWN_CHANCE_FRAME};
        return time_out > x.first;
  });
  auto keycode_it = std::find_if(std::begin(key::key_deque_), time_out_it,
      [&] (const std::pair<
          std::chrono::system_clock::time_point,
          std::array<int, key::KEY_TABLE_LENGTH> >& x) {
        return key::is_down(x.first, keycode);
  });
  if (keycode_it != time_out_it) {
    return true;
  }
  return false;
}

} /* namespace early_go */
