#include "basic_window.hpp"
#include "exception.hpp"

int WINAPI WinMain(_In_ ::HINSTANCE a_hinstance,
                   _In_opt_ ::HINSTANCE, _In_ ::LPSTR, _In_ ::INT)
try {
  ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF      |
                   _CRTDBG_DELAY_FREE_MEM_DF |
                   _CRTDBG_CHECK_ALWAYS_DF   |
                   _CRTDBG_LEAK_CHECK_DF);
  early_go::basic_window _basic_window{a_hinstance};

  return _basic_window();
} catch (const std::exception& a_expception) {
  early_go::log_liner{} << a_expception.what();
  // TODO: The Boost.Stacktrace feature to show source file name and its line
  // number is not supported on msvc. It's necessary to write fullscratch
  // though ridiculous.
//  std::unique_ptr<const boost::stacktrace::stacktrace> _up_stacktrace {
//      boost::get_error_info<early_go::traced>(a_expception) };

//  if (_up_stacktrace.get()) {
//    early_go::log_liner{} << *_up_stacktrace;

    // std::ostringstream _ostringstream;
    // _ostringstream << *_up_stacktrace << std::endl;
    // ::MessageBox(0, _ostringstream.str().c_str(), nullptr, MB_OK);
//  }
  return EXIT_FAILURE;
}
