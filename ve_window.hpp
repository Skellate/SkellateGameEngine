#pragma once

#define GLFW_INCLUDE_VULKAN /*This includes Vulkan header in GLFW header files*/
#include <GLFW/glfw3.h>

#include <string>
namespace lve
{
    class ve_window
    {
        public:
            ve_window(int w, int h, std::string name);
            ~ve_window();

        ve_window(const ve_window &) = delete;
        ve_window &operator=(const ve_window &) = delete;

        bool shouldClose(){return glfwWindowShouldClose(window); }
        VkExtent2D getExtent(){return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};}
        bool wasWindowResized(){return framebufferResized;}
        void resetWindowResizedFlag(){ framebufferResized = false;}
        GLFWwindow *getGLFWwindow() const { return window; }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

        private:
        static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
        void initWindow();

        int width;
        int height;
        bool framebufferResized = false;

        std::string windowName;
        GLFWwindow *window;
    };
}

