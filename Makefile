# Vulkan Engine Makefile
# Compiler and flags
CXX = C:/msys64/ucrt64/bin/g++.exe
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
DEBUGFLAGS = -g -DDEBUG

# Include directories
INCLUDES = -I"C:/VulkanSDK/1.4.321.1/Include" \
           -I"C:/glfw-3.4.bin.WIN64/include" \
           -I"C:/glew-2.1.0/include" \
           -I"."

# Library directories and libraries
LIBDIRS = -L"C:/VulkanSDK/1.4.321.1/Lib" \
          -L"C:/glew-2.1.0/lib/Release/x64" \
          -L"C:/glfw-3.4.bin.WIN64/lib-mingw-w64"

LIBS = -lvulkan-1 -lglfw3dll -lglew32 -lopengl32

# Source files
SOURCES = main.cpp \
          ve_window.cpp \
          ve_pipeline.cpp \
          ve_device.cpp \
          ve_swap_chain.cpp \
          ve_model.cpp \
          ve_transform.cpp \
          ve_game_object.cpp \
          ve_camera.cpp \
          keyboard_movement_controller.cpp \
          simple_game.cpp

# Object files (replace .cpp with .o)
OBJECTS = $(SOURCES:.cpp=.o)

# Shader files
VERTEX_SHADERS = shaders/simpleShader.vert
FRAGMENT_SHADERS = shaders/simpleShader.frag
COMPILED_SHADERS = $(VERTEX_SHADERS:.vert=.vert.spv) $(FRAGMENT_SHADERS:.frag=.frag.spv)

# Target executable
TARGET = main.exe

# Shader compiler
GLSLC = C:/VulkanSDK/1.4.321.1/Bin/glslc.exe

# Default target
.PHONY: all clean shaders debug release run

all: shaders release

# Release build
release: CXXFLAGS += -DNDEBUG
release: $(TARGET)

# Debug build
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBDIRS) $(LIBS)
	@echo "Build complete!"

# Compile source files to object files
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile shaders
shaders: $(COMPILED_SHADERS)

%.vert.spv: %.vert
	@echo "Compiling vertex shader $<..."
	$(GLSLC) $< -o $@

%.frag.spv: %.frag
	@echo "Compiling fragment shader $<..."
	$(GLSLC) $< -o $@

# Run the application
run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJECTS) $(TARGET) $(COMPILED_SHADERS)
	@echo "Clean complete!"

# Force rebuild
rebuild: clean all

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build shaders and release version (default)"
	@echo "  release  - Build optimized release version"
	@echo "  debug    - Build debug version with debug symbols"
	@echo "  shaders  - Compile GLSL shaders to SPIR-V"
	@echo "  run      - Build and run the application"
	@echo "  clean    - Remove all build artifacts"
	@echo "  rebuild  - Clean and build everything"
	@echo "  help     - Show this help message"

# Dependencies (optional - helps with incremental builds)
main.o: main.cpp simple_game.hpp
simple_game.o: simple_game.cpp simple_game.hpp ve_window.hpp ve_device.hpp ve_pipeline.hpp ve_swap_chain.hpp ve_model.hpp ve_game_object.hpp ve_camera.hpp keyboard_movement_controller.hpp
ve_window.o: ve_window.cpp ve_window.hpp
ve_device.o: ve_device.cpp ve_device.hpp
ve_pipeline.o: ve_pipeline.cpp ve_pipeline.hpp ve_device.hpp
ve_swap_chain.o: ve_swap_chain.cpp ve_swap_chain.hpp ve_device.hpp
ve_model.o: ve_model.cpp ve_model.hpp ve_device.hpp
ve_transform.o: ve_transform.cpp ve_transform.hpp
ve_game_object.o: ve_game_object.cpp ve_game_object.hpp ve_model.hpp ve_transform.hpp
ve_camera.o: ve_camera.cpp ve_camera.hpp
keyboard_movement_controller.o: keyboard_movement_controller.cpp keyboard_movement_controller.hpp ve_game_object.hpp
