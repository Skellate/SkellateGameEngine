@echo off
REM Vulkan Engine Build Script
echo Building Vulkan Engine...

REM Add MSYS2 to PATH
set PATH=%PATH%;C:\msys64\ucrt64\bin

REM Compile shaders first
echo Compiling shaders...
C:\VulkanSDK\1.4.321.1\Bin\glslc.exe shaders\simpleShader.vert -o shaders\simpleShader.vert.spv
C:\VulkanSDK\1.4.321.1\Bin\glslc.exe shaders\simpleShader.frag -o shaders\simpleShader.frag.spv

REM Build the application
echo Building C++ application...
g++ -fdiagnostics-color=always -g -DDEBUG main.cpp ve_window.cpp ve_pipeline.cpp ve_device.cpp ve_swap_chain.cpp ve_model.cpp ve_transform.cpp ve_game_object.cpp ve_camera.cpp keyboard_movement_controller.cpp simple_game.cpp geometry_builder.cpp -o main.exe -I "C:\VulkanSDK\1.4.321.1\Include" -I "C:\glfw-3.4.bin.WIN64\include" -I "C:\glew-2.1.0\include" -I "C:\msys64\ucrt64\include" -I "." -L "C:\VulkanSDK\1.4.321.1\Lib" -lvulkan-1 -L "C:\glew-2.1.0\lib\Release\x64" -L "C:\glfw-3.4.bin.WIN64\lib-mingw-w64" -lglfw3dll -lglew32 -lopengl32

if %ERRORLEVEL% EQU 0 (
    echo Build successful! You can now run main.exe
) else (
    echo Build failed!
)
