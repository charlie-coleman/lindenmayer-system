@echo off 

set MINGW_PATH=C:\msys64\mingw64\

xcopy /y .\src\*.ini .\build\

%MINGW_PATH%bin\g++.exe -std=c++17 -g .\src\*.cpp -I .\inc -I .\inc\SDL2 -I .\inc\GL -I %MINGW_PATH%include -L .\lib -L %MINGW_PATH%lib -l opengl32 -l SDL2 -l glew32 -l stdc++fs -l png -o .\build\lsystem.exe