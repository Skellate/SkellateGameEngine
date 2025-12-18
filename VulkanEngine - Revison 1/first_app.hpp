#pragma once

#include "ve_device.hpp"
#include "ve_pipeline.hpp"
#include "ve_window.hpp"
#include "string"

namespace lve
{
    class FirstVulkanApp{

        public:
        static constexpr int WIDTH = 900;
        static constexpr int HEIGHT = 660;

        void run();

        private:
            ve_window veWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
            LveDevice lveDevice{veWindow};
            vePipeline lvePipeline{lveDevice, "shaders\\simpleShader.vert.spv", "shaders\\simpleShader.frag.spv", vePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};

    };
}