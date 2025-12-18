@echo off
REM Launch script for Vulkan FPS Game
echo Launching Vulkan FPS Game...

REM Add MSYS2 runtime to PATH
set PATH=C:\msys64\ucrt64\bin;%PATH%

REM Launch the game
main.exe

pause
