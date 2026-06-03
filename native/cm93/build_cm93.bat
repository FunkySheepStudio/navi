@echo off
rmdir /s /q build
mkdir build
cd build
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
cmake .. -G "NMake Makefiles"
cmake --build .
