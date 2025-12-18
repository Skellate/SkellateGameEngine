#pragma once

#include "ve_swap_chain.hpp"
#include "ve_model.hpp"
#include "ve_device.hpp"
#include "ve_pipeline.hpp"
#include "ve_window.hpp"
#include "string"

#include <memory>
#include <vector>

namespace lve
{
    class FirstVulkanApp{

        public:
        static constexpr int WIDTH = 900;
        static constexpr int HEIGHT = 660;

        FirstVulkanApp();
        ~FirstVulkanApp();

        FirstVulkanApp(const FirstVulkanApp &) = delete;
        FirstVulkanApp &operator=(const FirstVulkanApp &) = delete;

        void run();

        private:
            void loadModels();
            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();
            void drawFrame();

            ve_window veWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
            LveDevice lveDevice{veWindow};
            LveSwapChain lveSwapChain{lveDevice, veWindow.getExtent()};
            
            std::unique_ptr<vePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
            std::unique_ptr<LveModel> lveModel;

    };
}