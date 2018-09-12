#ifndef INLINE_MACRO_HPP
#define INLINE_MACRO_HPP
#include "stdafx.hpp"
#include <fstream>

namespace early_go {
/*
 * If a custom deleter for smart-pointer is necessary, use the following
 * 'custom_deleter' instead of these.
 *
 * These template function, like a safe_*****, is for freeing memory. The reason
 * why the argument is pointer-reference is because of executing the line of
 * "p = nullptr;" and what executing the line makes the arguments of a client
 * which uses this function effected. The real purpose of "p = nullptr;" line is
 * to prevent from a double-free and being used the argument by the client.
 */
template <typename T>
inline void safe_delete(T*& p) {
  /*
   * A null check is not necessary because the behavior is defined by standards
   * which to execute 'delete' to nullptr does not do anything.
   */
  delete p;
  p = nullptr;
}
template <typename T>
inline void safe_delete_array(T*& p) {
  /*
   * A null check is not necessary because the behavior is defined by standards
   * which to execute 'delete' to nullptr does not do anything.
   */
  delete[] p;
  p = nullptr;
}
template <typename T>
inline void safe_release(T*& p) {
  if (p) {
    p->Release();
    p = nullptr;
  }
}
/*
 * A custom deleter for smart-pointer. The reason why a function object not
 * a template function is because not to have to write a type.
 *
 * Usages:
 *
 *   void sample()
 *   {
 *     ::LPDIRECT3D9 _p_direct3d9{::Direct3DCreate9(D3D_SDK_VERSION)};
 *     std::unique_ptr<::LPDIRECT3D9, custom_deleter> up{
 *         _p_direct3d9, custom_deleter{}};
 *   }
 */
struct custom_deleter
{
  template <typename T>
  void operator()(T p)
  {
    /*
     * A null assignment is not necessary because a null assignment is for
     * preventing from a segmentation fault by a double-free and a smart pointer
     * does not free only once and that behavior is the reason why this is used.
     */
    p->Release();
  }
};

inline std::vector<char> get_resource(const std::string& a_kr_query)
{
#if 0

  std::size_t begin = a_kr_query.find_first_of("'")+1;
  std::size_t end = a_kr_query.find_first_of("'", begin);
  std::string filename{a_kr_query.substr(begin, end-begin)};

  std::string path{"res/" + filename};
  std::ifstream file(path, std::ios::binary);
  std::ostringstream ss;
  ss << file.rdbuf();
  const std::string& s = ss.str();
  return std::vector<char>(s.begin(), s.end());

#else
  std::vector<char> _ret;
  ::sqlite3* db = nullptr;
  if (::sqlite3_open(constants::DATABASE_NAME.c_str(), &db) != SQLITE_OK) {
    ::sqlite3_close(db);
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to open a database."});
  }
  ::sqlite3_stmt* _statement = nullptr;
  ::sqlite3_prepare_v2(db, a_kr_query.c_str(), -1, &_statement, nullptr);
  bool _b_find{false};
  while (::sqlite3_step(_statement) == SQLITE_ROW) {
    if (!_b_find) {
      _b_find = true;
    } else {
      ::sqlite3_finalize(_statement);
      ::sqlite3_close(db);
      BOOST_THROW_EXCEPTION(
          custom_exception{"There are multiple specified resources.\n"
                           " query: " + a_kr_query});
    }
    const char* _blob = (char*)sqlite3_column_blob(_statement, 0);
    int _data_len = sqlite3_column_bytes(_statement, 0);
    _ret.reserve(_data_len);
    _ret.insert(_ret.begin(), _blob, _blob+_data_len);
  }
  ::sqlite3_finalize(_statement);
  ::sqlite3_close(db);
  if (!_b_find) {
    BOOST_THROW_EXCEPTION(custom_exception{"Failed to find a resource.\n"
                          " query: " + a_kr_query});
  }
  return _ret;
#endif
}

/*
 * A class making logging simple.
 *
 * Usages:
 *   early_go::log_liner{} << 42 << std::to_string(42); // 'std::endl' is added
 *                                                      // automatically.
 */
struct log_liner
{
//#if defined(DEBUG) || defined(_DEBUG)
#if 1
  /* c'tor */
  log_liner(const std::string& op = "") : ostringstream_{}, option_{op} {}

  /* A definition of the "<<" operator. */
  template <typename T>
  log_liner& operator<<(T a)
  {
    this->ostringstream_ << a;
    return *this;
  }

  /* d'tor */
  ~log_liner()
  {
    if (this->option_ != "-n") {
      this->ostringstream_ << std::endl;
    }
    ::OutputDebugString(this->ostringstream_.str().c_str());
  }
private:
  std::ostringstream ostringstream_;
  const std::string option_;
#else
  template <typename T>
  log_liner& operator<<(T)
  {
    return *this;
  }
#endif
};
}
#endif
