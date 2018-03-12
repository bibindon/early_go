# I wrote this, but I don't know why this works :(

# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = early_go

HEADERS = \
   $$PWD/src/animation_mesh.hpp \
   $$PWD/src/animation_mesh_allocator.hpp \
   $$PWD/src/basic_window.hpp \
   $$PWD/src/constants.hpp \
   $$PWD/src/exception.hpp \
   $$PWD/src/inline_macro.hpp \
   $$PWD/src/stdafx.hpp

SOURCES = \
   $$PWD/src/animation_mesh.cpp \
   $$PWD/src/animation_mesh_allocator.cpp \
   $$PWD/src/basic_window.cpp \
   $$PWD/src/main.cpp \
   $$PWD/src/stdafx.cpp

INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.16299.0/ucrt"
INCLUDEPATH += "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include"
INCLUDEPATH += "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.13.26128/include"
INCLUDEPATH += "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.13.26128/atlmfc/include"
INCLUDEPATH += "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Auxiliary/VS/include"
INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.16299.0/ucrt"
INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.16299.0/um"
INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.16299.0/shared"
INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.16299.0/winrt"
INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/NETFXSDK/4.6.1/Include/um"

INCLUDEPATH += "$$PWD/../packages/boost.1.66.0.0/lib/native/include"

DEFINES = _MBCS
DEFINES += DEBUG

QMAKE_CXXFLAGS += /FS /permissive- /GS /W4 /Zc:wchar_t /ZI /Gm- /Od /sdl /Fd"Debug/vc141.pdb" /FI"stdafx.hpp" /Zc:inline /fp:precise /D "_MBCS" /errorReport:prompt /WX /Zc:forScope /RTC1 /Gd /MDd /std:c++17 /FC /Fa"Debug/" /EHsc /nologo /Fo"Debug/" /Fp"Debug/early_go.pch" /diagnostics:classic

LIBS += -L"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\um\x64" -lshell32
LIBS += -L"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\um\x64" -lkernel32
LIBS += -L"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\um\x64" -luser32
LIBS += -L"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\um\x64" -lgdi32
LIBS += -L"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.13.26128\lib\x64" -lmsvcprtd
LIBS += -L"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x64" -ld3dx9d
LIBS += -L"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\ucrt\x64" -lucrtd

