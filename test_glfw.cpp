#include <iostream>
#include <GLFW/glfw3.h>

int main() {
    std::cout << "Testing GLFW initialization..." << std::endl;
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    std::cout << "GLFW initialized successfully!" << std::endl;
    
    // Try to create a window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Test Window", nullptr, nullptr);
    
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    std::cout << "Window created successfully!" << std::endl;
    
    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    
    std::cout << "Test completed successfully!" << std::endl;
    return 0;
}
