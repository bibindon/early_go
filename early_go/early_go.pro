# I wrote this, but I don't know why this works :(

# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

# TARGET = early_go

SOURCES = $$files(*.cpp, true)
HEADERS = $$files(*.hpp, true)

INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.16299.0/ucrt"
INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.16299.0/ucrt"
INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.16299.0/um"
INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.16299.0/shared"
INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.16299.0/winrt"
INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/NETFXSDK/4.6.1/Include/um"
INCLUDEPATH += "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.13.26128/include"
INCLUDEPATH += "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.13.26128/atlmfc/include"
INCLUDEPATH += "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Auxiliary/VS/include"
INCLUDEPATH += "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include"

INCLUDEPATH += "$$PWD/../packages/boost.1.66.0.0/lib/native/include"

DEFINES = _MBCS
DEFINES += DEBUG
QMAKE_CXXFLAGS += /std:c++17

LIBS += -L"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\um\x64" -lshell32
LIBS += -L"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\um\x64" -lkernel32
LIBS += -L"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\um\x64" -luser32
LIBS += -L"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\um\x64" -lgdi32
LIBS += -L"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0\ucrt\x64" -lucrtd
LIBS += -L"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.13.26128\lib\x64" -lmsvcprtd
LIBS += -L"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x64" -ld3dx9d

