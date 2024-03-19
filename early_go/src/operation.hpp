#ifndef OPERATION_HPP
#define OPERATION_HPP

#include "stdafx.hpp"
#include "key.hpp"
#include "main_window.hpp"

#undef interface
#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/callable.hpp>
#include <boost/type_erasure/member.hpp>
#include <boost/type_erasure/relaxed.hpp>
#include <boost/variant.hpp>
#include <boost/mpl/vector.hpp>

BOOST_TYPE_ERASURE_MEMBER((early_go)(has_get_params), get_params, 0)
BOOST_TYPE_ERASURE_MEMBER((early_go)(has_cancel), cancel, 0)

#define interface __STRUCT__

namespace early_go
{
class camera;

class operation
{
public:
    operation(const std::shared_ptr<camera> &);
    void operator()(main_window &);

    enum class behavior_state
    {
        // READY,
        PLAY,
        // STOP,
        CANCELABLE,
        FINISH,
    };

    typedef boost::type_erasure::any<
        boost::mpl::vector<
            has_get_params<std::vector<boost::variant<direction>>(void)>,
            has_cancel<void(void)>,
            boost::type_erasure::callable<behavior_state()>,
            boost::type_erasure::copy_constructible<>,
            boost::type_erasure::relaxed>
    > behavior_concept;

    void set_offensive_position(const int &, const int &);
    std::shared_ptr<camera> get_camera() const;

private:
    std::shared_ptr<camera> camera_;
    struct event
    {
        std::vector<boost::variant<direction>> params_;
        std::vector<boost::variant<direction>> get_params() { return params_; }
        int count_{0};
        virtual behavior_state operator()() = 0;
        virtual ~event(){};
        virtual void cancel() {}
    };

    struct move_next_stage : event
    {
        move_next_stage(operation &, main_window &);
        behavior_state operator()() override;
        ~move_next_stage() override {}
        operation &outer_;
        main_window &main_window_;
        float main_chara_x_;
        float main_chara_z_;
        float delta_main_chara_x_;
        float delta_main_chara_z_;
        float enemy_x_;
        float enemy_z_;
        float delta_enemy_x_;
        float delta_enemy_z_;
        bool enemy_move_called_;
        bool main_chara_move_called_;
    };

    std::shared_ptr<behavior_concept> current_behavior_;
    std::shared_ptr<behavior_concept> reserved_behavior_;

    std::unordered_map<int, std::unordered_map<int, bool>> offensive_area_;

    int current_stage_;
};
} // namespace early_go 
#endif
