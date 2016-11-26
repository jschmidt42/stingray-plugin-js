@echo off
if not exist build mkdir build
cd build
cmake ..\src -G "Visual Studio 14 2015 Win64"
cd ..