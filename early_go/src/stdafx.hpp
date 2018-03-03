/* precompiled header */
#ifndef STDAFX_HPP
#define STDAFX_HPP

#define D3D_DEBUG_INFO
#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE

#include <crtdbg.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include <algorithm>
#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#pragma comment(lib,"d3d9.lib")
#if defined(DEBUG) || defined(_DEBUG)
# pragma comment( lib, "d3dx9d.lib" )
#else
# pragma comment( lib, "d3dx9.lib" )
#endif

/*
 * This macro is for replacing 'malloc' and 'new' to detect memory leaks. This
 * must be at the buttom of a header file like a here because this macro causes
 * the build error if this is at above include line of the specific header.
 */
#if defined(DEBUG) || defined(_DEBUG)
# define malloc(X) _malloc_dbg(X,_NORMAL_BLOCK,__FILE__,__LINE__) 
# define new ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#endif
