#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP
#include "stdafx.hpp"

#include <boost/exception/all.hpp>
#include <boost/stacktrace.hpp>

namespace early_go
{
    /*
     * A utilities of exception.
     *
     * Usage
     *
     *   void f()
     *   {
     *     THROW_WITH_TRACE("aiueo");
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
     */

    extern boost::exception_ptr exception_reserve;

    using additional_info = boost::error_info<struct tag_stacktrace, std::string>;
    using traced =
        boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace>;

    class custom_exception : virtual public boost::exception, virtual public std::exception
    {
    public:
        custom_exception() : custom_exception("There is not any messages.") {}
        custom_exception(const std::string &message)
            : std::exception(), message_(message) {}
        custom_exception &operator=(const custom_exception &rhs)
        {
            if (this != &rhs)
            {
                std::string *sz = const_cast<std::string *>(&message_);
                *sz = rhs.message_;
            }
            return *this;
        }

        virtual const char *what() const noexcept
        {
            return message_.c_str();
        }

    private:
        const std::string message_;
    };

#define THROW_WITH_TRACE(...)                     \
    custom_exception e{__VA_ARGS__};              \
    e << traced{boost::stacktrace::stacktrace()}; \
    BOOST_THROW_EXCEPTION(e)

} // namespace early_go 
#endif
