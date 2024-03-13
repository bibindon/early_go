/* precompiled header */
#ifndef STDAFX_HPP
#define STDAFX_HPP

#define NOMINMAX
#define D3D_DEBUG_INFO
#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
#define MEMORY_LEAKS

// Library header
#if (defined(DEBUG) || defined(_DEBUG)) && defined(MEMORY_LEAKS)
# include <crtdbg.h>
#endif
#include <d3d9.h>
#include <d3dx9.h>
#include <DXErr.h>

#include <sqlite3.h>

# pragma warning(disable : 4996) // fopen / fopen_s warning
# pragma warning(disable : 4819) // sjis / utf8 warning
#   include <opencv2/opencv.hpp>
# pragma warning(default : 4819)
# pragma warning(default : 4996)

#include <windows.h>

// C Standard Library
#include <stdio.h>
#include <string.h>

// STL
#include <algorithm>
#include <array>
#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
#include <queue>
#include <unordered_map>

// Self made header
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
#pragma comment(lib, "DXErr.lib")

#ifdef DEBUG
#pragma comment(lib, "opencv_core320d.lib")
#pragma comment(lib, "opencv_imgcodecs320d.lib")
#pragma comment(lib, "opencv_imgproc320d.lib")
#pragma comment(lib, "opencv_highgui320d.lib")
#else
#pragma comment(lib, "opencv_core320.lib")
#pragma comment(lib, "opencv_imgcodecs320.lib")
#pragma comment(lib, "opencv_imgproc320.lib")
#pragma comment(lib, "opencv_highgui320.lib")
#endif

/*
 * This macro is for replacing 'malloc' and 'new' to detect memory leaks. This
 * must be at the buttom of a header file like a here because this macro causes
 * the build error if this is at above include line of the specific header.
 */
#if (defined(DEBUG) || defined(_DEBUG))
# define malloc_crt(x) ::_malloc_dbg((x), _NORMAL_BLOCK, __FILE__, __LINE__)
# define _aligned_malloc_crt(x, y) \
    ::_aligned_malloc_dbg((x), (y), __FILE__, __LINE__)
# define new_crt ::new (_NORMAL_BLOCK, __FILE__, __LINE__)
#else
# define malloc_crt(x) _malloc((x))
# define _aligned_malloc_crt(x, y) _aligned_malloc((x), (y))
# define new_crt ::new
#endif

#endif
