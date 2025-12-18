#include "first_app.hpp"

namespace lve
{
    void FirstVulkanApp::run()
    {
        while (!veWindow.shouldClose())
        {
            glfwPollEvents();
        }
        
    }
}

