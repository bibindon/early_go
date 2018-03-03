#include "basic_window.hpp"
#include "exception.hpp"

int WINAPI WinMain(_In_ ::HINSTANCE a_hinstance,
                   _In_opt_ ::HINSTANCE, _In_ ::LPSTR, _In_ ::INT)
try {
#if defined(DEBUG) || defined(_DEBUG)
  ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF      |
                   _CRTDBG_DELAY_FREE_MEM_DF |
                   _CRTDBG_CHECK_ALWAYS_DF   |
                   _CRTDBG_LEAK_CHECK_DF);
#endif
  early_go::basic_window _basic_window{a_hinstance};

  return _basic_window();
} catch (const std::exception& a_expception) {
  early_go::log_liner{} << boost::diagnostic_information(a_expception);
  return EXIT_FAILURE;
}
