@echo off
REM build file for win32_noise project
REM build environment for windows needs to be sourced before

set msvc_compileFlags=/Zi /Odi /W3 /MP4 /EHsc /EHa- /std:c++latest /wd5105
set linker=user32.lib gdi32.lib Winmm.lib Dsound.lib Ole32.lib

set sources=..\src\main.cpp ..\src\noise.cpp
 
pushd build
cl /nologo %msvc_compileFlags% %sources% /link /DEBUG:FULL %linker%
popd