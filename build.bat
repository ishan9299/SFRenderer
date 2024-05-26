@echo off

setlocal

if not exist build mkdir build

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 > nul

pushd build

set WIN_LIBS=user32.lib gdi32.lib xinput.lib

set WIN_HEADER_INCLUDE="C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\um"
set WIN_HEADER_SHARED="C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\shared"
set WIN_UCRT_INCLUDE="C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\ucrt"
set MSVC_INCLUDE="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\include"

set INTERNAL_INCLUDES=/I%WIN_HEADER_INCLUDE% /I%WIN_HEADER_SHARED% /I%MSVC_INCLUDE% /I%WIN_UCRT_INCLUDE%

cl ..\main.c /Z7 %WIN_LIBS% %INTERNAL_INCLUDES%

popd

endlocal
