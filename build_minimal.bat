@echo off
REM Minimal Vulkan Engine Build Script for Testing
echo Building minimal Vulkan Engine...

REM Add MSYS2 to PATH
set PATH=%PATH%;C:\msys64\ucrt64\bin

REM Build with minimal dependencies
echo Building C++ application with minimal deps...
g++ -fdiagnostics-color=always -g main.cpp -o main_minimal.exe -I "C:\VulkanSDK\1.4.321.1\Include" -I "C:\glfw-3.4.bin.WIN64\include" -I "C:\msys64\ucrt64\include" -I "." -L "C:\VulkanSDK\1.4.321.1\Lib" -L "C:\glfw-3.4.bin.WIN64\lib-mingw-w64" -lglfw3dll

if %ERRORLEVEL% EQU 0 (
    echo Minimal build successful! You can now run main_minimal.exe
) else (
    echo Minimal build failed!
    pause
)
