#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#define BOOST_STACKTRACE_LINK
#define BOOST_STACKTRACE_USE_WINDBG
#define BOOST_STACKTRACE_USE_ADDR2LINE

#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>

#pragma comment(lib, "libboost_stacktrace_windbg-vc141-mt-gd-x32-1_66.lib")

namespace early_go {
/*
 * Usage
 *
 * try {
 *   throw_with_trace(std::logic_error("hogehoge"));
 * } catch (const std::exception& e) {
 *   std::cerr << e.what() << '\n';
 *   const boost::stacktrace::stacktrace* st =
 *       boost::get_error_info<traced>(e);
 *
 *   if (st) {
 *     std::cerr << *st << '\n';
 *   }
 * }
 */
typedef boost::error_info<
    struct tag_stacktrace, boost::stacktrace::stacktrace> traced;

template <class E>
void throw_with_trace(const E& e) {
  throw boost::enable_error_info(e)
      << traced(boost::stacktrace::stacktrace());
}
} /* namespace early_go */
#endif
