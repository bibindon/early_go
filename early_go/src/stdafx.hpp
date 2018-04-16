/* precompiled header */
#ifndef STDAFX_HPP
#define STDAFX_HPP

#define D3D_DEBUG_INFO
#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE

#if defined(DEBUG) || defined(_DEBUG)
#include <crtdbg.h>
#endif
#include <d3d9.h>
#include <d3dx9.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <windows.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <exception>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "constants.hpp"
#include "exception.hpp"
#include "inline_macro.hpp"

#pragma comment(lib,"d3d9.lib")
#if defined(DEBUG) || defined(_DEBUG)
# pragma comment( lib, "d3dx9d.lib" )
#else
# pragma comment( lib, "d3dx9.lib" )
#endif
#pragma comment(lib, "sqlite3.lib")

/*
 * This macro is for replacing 'malloc' and 'new' to detect memory leaks. This
 * must be at the buttom of a header file like a here because this macro causes
 * the build error if this is at above include line of the specific header.
 */
#if defined(DEBUG) || defined(_DEBUG)
# define malloc_crt(x) ::_malloc_dbg((x), _NORMAL_BLOCK, __FILE__, __LINE__)
# define _aligned_malloc_crt(x, y) \
    ::_aligned_malloc_dbg((x), (y), __FILE__, __LINE__)
# define new_crt ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
# define malloc_crt(x) _malloc((x))
# define _aligned_malloc_crt(x, y) _aligned_malloc((x), (y))
# define new_crt ::new
#endif

#endif
