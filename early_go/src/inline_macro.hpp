#ifndef INLINE_MACRO_HPP
#define INLINE_MACRO_HPP

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

/*
 * A class making logging simple.
 *
 * Usages:
 *   early_go::log_liner{} << 42 << std::to_string(42); // 'std::endl' is added
 *                                                      // automatically.
 */
struct log_liner
{
#if defined(DEBUG) || defined(_DEBUG)
  std::ostringstream ostringstream_;

  /* c'tor */
  log_liner() : ostringstream_{} {}

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
    this->ostringstream_ << std::endl;
    ::OutputDebugString(this->ostringstream_.str().c_str());
  }
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
