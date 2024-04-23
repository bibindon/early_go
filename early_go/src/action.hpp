#pragma once
#include "stdafx.hpp"

#include "character2.hpp"

namespace early_go
{
class action
{
public:
    action(character2& chara, const direction);

    virtual operation::behavior_state update(const std::shared_ptr<stage>&) = 0;
    virtual void cancel() = 0;
    virtual ~action() {}
    std::vector<direction> params_;
    std::vector<direction> get_params()
    {
        return params_;
    }
    int count_;
    character2 &chara_;
};
class step : public action
{
public:
    step(character2 &, const direction);
    operation::behavior_state update(const std::shared_ptr<stage>&) override;
    void cancel() override;
    ~step() override;
    std::vector<float> destinations_;
    direction relative_direction_;
};
class rotate : public action
{
public:
    rotate(character2 &, const direction);
    operation::behavior_state update(const std::shared_ptr<stage>&) override;
    void cancel() override;
    ~rotate() override;
    direction relative_direction_;
    const direction back_up_direction_;
};
class step_and_rotate : public action
{
public:
    step_and_rotate(character2 &, const direction, const direction);
    operation::behavior_state update(const std::shared_ptr<stage>&) override;
    void cancel() override;
    ~step_and_rotate() override;
    step step_;
    rotate rotate_;
};
class attack : public action
{
public:
    attack(character2 &);
    operation::behavior_state update(const std::shared_ptr<stage>&) override;
    void cancel() override;
    ~attack();
    bool availability_;
};

} // namespace early_go 

