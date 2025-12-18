#include <iostream>
#include <vulkan/vulkan.h>

int main() {
    std::cout << "Testing Vulkan..." << std::endl;
    
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Test App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkInstance instance;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    
    if (result == VK_SUCCESS) {
        std::cout << "Vulkan instance created successfully!" << std::endl;
        vkDestroyInstance(instance, nullptr);
        return 0;
    } else {
        std::cout << "Failed to create Vulkan instance. Error: " << result << std::endl;
        return 1;
    }
}
