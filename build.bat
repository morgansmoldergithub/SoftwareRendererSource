@echo off

set CompilerFlags=-MT -nologo -Gm- -GR- -O2 -Oi -W4 -Zi -FC -EHsc -wd4201 -wd4100 -wd4389

set LinkerFlags=-incremental:no -opt:ref user32.lib Gdi32.lib Winmm.lib ../lib/SDL2.lib

pushd .\build

rem 64 Bit
cl %CompilerFlags% ../src/main.cpp /link %LinkerFlags%

popd
