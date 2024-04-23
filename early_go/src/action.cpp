#include "action.hpp"

using std::shared_ptr;
using std::string;

namespace early_go
{
action::action(character2 &chara, const direction direction)
    : count_ { },
      chara_ { chara }
{
    params_.push_back(direction);
}
static float get_sine_curve(int theta, int duration)
{
    float sine_curve { std::sin((static_cast<float>(theta) / duration * D3DX_PI) - (D3DX_PI / 2)) };
    sine_curve += 1.0f;
    sine_curve /= 2.0f;
    return sine_curve;
}
step::step(character2 &chara, const direction dir)
    : action { chara, dir },
      relative_direction_ { direction::NONE }
{
    for (int i { 0 }; i < constants::ACTION_INTERVAL_FRAME + 1; ++i)
    {
        float y { get_sine_curve(i, constants::ACTION_INTERVAL_FRAME) * constants::GRID_LENGTH };

        destinations_.push_back(y);
    }
    switch (params_.at(0))
    {
    case direction::FRONT:
        switch (chara_.get_direction())
        {
        case direction::FRONT:
            relative_direction_ = direction::FRONT;
            break;
        case direction::LEFT:
            relative_direction_ = direction::RIGHT;
            break;
        case direction::BACK:
            relative_direction_ = direction::BACK;
            break;
        case direction::RIGHT:
            relative_direction_ = direction::RIGHT;
            break;
        }
        break;
    case direction::LEFT:
        switch (chara_.get_direction())
        {
        case direction::FRONT:
            relative_direction_ = direction::LEFT;
            break;
        case direction::LEFT:
            relative_direction_ = direction::FRONT;
            break;
        case direction::BACK:
            relative_direction_ = direction::RIGHT;
            break;
        case direction::RIGHT:
            relative_direction_ = direction::BACK;
            break;
        }
        break;
    case direction::BACK:
        switch (chara_.get_direction())
        {
        case direction::FRONT:
            relative_direction_ = direction::BACK;
            break;
        case direction::LEFT:
            relative_direction_ = direction::LEFT;
            break;
        case direction::BACK:
            relative_direction_ = direction::FRONT;
            break;
        case direction::RIGHT:
            relative_direction_ = direction::RIGHT;
            break;
        }
        break;
    case direction::RIGHT:
        switch (chara_.get_direction())
        {
        case direction::FRONT:
            relative_direction_ = direction::LEFT;
            break;
        case direction::LEFT:
            relative_direction_ = direction::BACK;
            break;
        case direction::BACK:
            relative_direction_ = direction::RIGHT;
            break;
        case direction::RIGHT:
            relative_direction_ = direction::FRONT;
            break;
        }
        break;
    }
}
operation::behavior_state step::update(const shared_ptr<stage>&)
{
    if (count_ == 0)
    {
        vec3i pos { chara_.get_grid_position() };
        switch (params_.at(0))
        {
        case direction::FRONT:
            pos.z += 1;
            break;
        case direction::LEFT:
            pos.x += -1;
            break;
        case direction::BACK:
            pos.z += -1;
            break;
        case direction::RIGHT:
            pos.x += 1;
            break;
        }
        chara_.set_grid_position(pos);
        switch (relative_direction_)
        {
        case direction::FRONT:
            chara_.set_animation("Step_Front");
            break;
        case direction::LEFT:
            chara_.set_animation("Step_Left");
            break;
        case direction::BACK:
            chara_.set_animation("Step_Back");
            break;
        case direction::RIGHT:
            chara_.set_animation("Step_Right");
            break;
        }
    }
    if (constants::ACTION_INTERVAL_FRAME-2 <= count_ &&
        (count_)*constants::ANIMATION_SPEED < chara_.get_duration_map().at("Step_Left"))
    {
        ++count_;
        return operation::behavior_state::ALLOW_NEXT_ACTION;
    }
    else if (chara_.get_duration_map().at("Step_Left") <= (count_)*constants::ANIMATION_SPEED)
    {
        return operation::behavior_state::FINISH;
    }
    D3DXVECTOR3 position { chara_.get_pos() };
    float delta { 0.0f };
    size_t count_index{static_cast<size_t>(count_)};
    if (count_index + 1 < destinations_.size())
    {
        delta = destinations_.at(count_index + 1) - destinations_.at(count_index);
    }
    switch (params_.at(0))
    {
    case direction::FRONT:
        position.z += delta;
        break;
    case direction::LEFT:
        position.x -= delta;
        break;
    case direction::BACK:
        position.z -= delta;
        break;
    case direction::RIGHT:
        position.x += delta;
        break;
    }
    chara_.set_pos(position);
    ++count_;
    return operation::behavior_state::PLAY;
}
step::~step()
{
    if (count_ != 0)
    {
        chara_.set_pos(chara_.get_pos());
    }
}
void step::cancel()
{
    if (count_ != 0)
    {
        vec3i pos = chara_.get_grid_position();
        switch (params_.at(0))
        {
        case direction::FRONT:
            pos.z -= 1;
            break;
        case direction::LEFT:
            pos.x += 1;
            break;
        case direction::BACK:
            pos.z += 1;
            break;
        case direction::RIGHT:
            pos.x -= 1;
            break;
        }
        chara_.set_grid_position(pos);
    }
}
rotate::rotate(character2 &chara, const direction dir)
    : action { chara, dir }
    , relative_direction_ { direction::NONE }
    , back_up_direction_ { chara.get_direction() }
{
    switch (chara_.get_direction())
    {
    case direction::FRONT:
        switch (params_.at(0))
        {
        case direction::FRONT:
            relative_direction_ = direction::NONE;
            break;
        case direction::LEFT:
            relative_direction_ = direction::LEFT;
            break;
        case direction::BACK:
            relative_direction_ = direction::BACK;
            break;
        case direction::RIGHT:
            relative_direction_ = direction::RIGHT;
            break;
        }
        break;
    case direction::LEFT:
        switch (params_.at(0))
        {
        case direction::FRONT:
            relative_direction_ = direction::RIGHT;
            break;
        case direction::LEFT:
            relative_direction_ = direction::NONE;
            break;
        case direction::BACK:
            relative_direction_ = direction::LEFT;
            break;
        case direction::RIGHT:
            relative_direction_ = direction::BACK;
            break;
        }
        break;
    case direction::BACK:
        switch (params_.at(0))
        {
        case direction::FRONT:
            relative_direction_ = direction::BACK;
            break;
        case direction::LEFT:
            relative_direction_ = direction::RIGHT;
            break;
        case direction::BACK:
            relative_direction_ = direction::NONE;
            break;
        case direction::RIGHT:
            relative_direction_ = direction::LEFT;
            break;
        }
        break;
    case direction::RIGHT:
        switch (params_.at(0))
        {
        case direction::FRONT:
            relative_direction_ = direction::LEFT;
            break;
        case direction::LEFT:
            relative_direction_ = direction::BACK;
            break;
        case direction::BACK:
            relative_direction_ = direction::RIGHT;
            break;
        case direction::RIGHT:
            relative_direction_ = direction::NONE;
            break;
        }
        break;
    }
}
operation::behavior_state rotate::update(const shared_ptr<stage>& stage)
{
    if (count_ == 0)
    {
        chara_.set_direction(params_.at(0));
        switch (relative_direction_)
        {
        case direction::LEFT:
            chara_.set_animation("Rotate_Left");
            break;
        case direction::BACK:
            chara_.set_animation("Rotate_Back");
            break;
        case direction::RIGHT:
            chara_.set_animation("Rotate_Right");
            break;
        case direction::NONE:
            break;
        }
    }
    if (constants::ACTION_INTERVAL_FRAME <= count_ &&
        (count_)*constants::ANIMATION_SPEED < chara_.get_duration_map().at("Rotate_Left"))
    {
        ++count_;
        return operation::behavior_state::ALLOW_NEXT_ACTION;
    }
    else if (chara_.get_duration_map().at("Rotate_Left") <= (count_)*constants::ANIMATION_SPEED)
    {
        return operation::behavior_state::FINISH;
    }
    ++count_;
    return operation::behavior_state::PLAY;
}
rotate::~rotate()
{
    if (count_ != 0)
    {
        chara_.set_direction(chara_.get_direction());
    }
}
void rotate::cancel()
{
    chara_.set_direction(back_up_direction_);
}
step_and_rotate::step_and_rotate(
    character2 &chara, const direction step_dir, const direction rotate_dir)
    : action { chara, direction::NONE }
    , step_ { chara, step_dir }
    , rotate_ { chara, rotate_dir }
{
}
operation::behavior_state step_and_rotate::update(const shared_ptr<stage>& stage)
{
    step_.update(stage);
    operation::behavior_state result = rotate_.update(stage);
    count_ = rotate_.count_;
    return result;
}
step_and_rotate::~step_and_rotate()
{
}
void step_and_rotate::cancel()
{
    step_.cancel();
    rotate_.cancel();
}

attack::attack(character2 &chara)
    : action { chara, direction::NONE },
      availability_ { true }
{
}
operation::behavior_state attack::update(const shared_ptr<stage>& stage)
{
    if (availability_)
    {
        int x = chara_.get_grid_position().x;
        int z = chara_.get_grid_position().z;
        switch (chara_.get_direction())
        {
        case direction::FRONT:
            ++z;
            break;
        case direction::LEFT:
            --x;
            break;
        case direction::BACK:
            --z;
            break;
        case direction::RIGHT:
            ++x;
            break;
        }
        stage->set_offensive_position(x, z);
    }

    if (count_ == 0)
    {
        chara_.set_animation("Attack");
    }
    if (constants::ACTION_INTERVAL_FRAME <= count_ &&
        (count_)*constants::ANIMATION_SPEED < chara_.get_duration_map().at("Attack"))
    {
        ++count_;
        return operation::behavior_state::ALLOW_NEXT_ACTION;
    }
    if ((count_)*constants::ANIMATION_SPEED >= chara_.get_duration_map().at("Attack"))
    {
        return operation::behavior_state::FINISH;
    }
    ++count_;
    return operation::behavior_state::PLAY;
}
void attack::cancel()
{
    availability_ = false;
}
attack::~attack()
{
}

}

