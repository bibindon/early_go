#ifndef OPERATION_HPP
#define OPERATION_HPP

#include "stdafx.hpp"
#include "key.hpp"
#include "basic_window.hpp"

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

namespace early_go {

class operation {
public:
  operation();
  void operator()(basic_window&);

  enum behavior_state {
    //READY,
    PLAY,
    //STOP,
    CANCELABLE,
    FINISH,
  };

  typedef boost::type_erasure::any<
      boost::mpl::vector<
          has_get_params<
              std::vector<
                  boost::variant<direction>
              >(void)
          >,
          has_cancel<void(void)>,
          boost::type_erasure::callable<behavior_state()>,
          boost::type_erasure::copy_constructible<>,
          boost::type_erasure::relaxed
      >
  > behavior_concept;

  void set_offensive_position(const int&, const int&);
private:
  struct state {
    std::vector<
        boost::variant<direction>
    > params_;
    int count_{0};
    virtual bool operator()(basic_window&) = 0;
    virtual ~state() {};
  };

  std::shared_ptr<behavior_concept> current_behavior_;
  std::shared_ptr<behavior_concept> reserved_behavior_;

  std::unordered_map<int, std::unordered_map<int, bool> > offensive_area_;

  const static std::chrono::milliseconds DOUBLE_DOWN_CHANCE_FRAME;
  bool check_recent_keycode(const int&);

};
} /* namespace early_go */
#endif
