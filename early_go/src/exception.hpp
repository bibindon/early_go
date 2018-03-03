#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <boost/exception/all.hpp>

namespace early_go {
/*
 * A utilities of exception. 
 *
 * Usage
 *  
 *   void f()
 *   {
 *     BOOST_THROW_EXCEPTION(custom_exception{"hogehoge"});
 *   }
 *
 *   void g()
 *   {
 *     try {
 *       f();
 *     } catch (boost::exception& e) {
 *       e << additional_info("aiueo");
 *       throw;
 *     }
 *   }
 *
 *   try {
 *     void g();
 *   } catch (const std::exception& e) {
 *     early_go::log_liner{} << boost::diagnostic_information(e);
 *   }
 *
 * The reason why not use the Stacktrace:
 *   The MSVC cannot use the Boost.StackTrace to show a function name and line
 *   number. Also, the combination of the 'Visual Studio 2017' and 'Windows 7'
 *   cannot use the WinAPI for stack trace.
 */

using additional_info = boost::error_info<struct tag_stacktrace, std::string>;

class custom_exception :
    virtual public boost::exception, virtual public std::exception
{
public:
  custom_exception() : custom_exception("There is not any messages.") {}
  custom_exception(const std::string& a_krsz)
      : std::exception(), ksz_(a_krsz) {}

  virtual const char* what() const noexcept
  {
    return this->ksz_.c_str();
  }

private:
  const std::string ksz_;
};

} /* namespace early_go */
#endif
