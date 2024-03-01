#pragma once

#include <memory>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "lve_pipeline.hpp"
#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_model.hpp"

namespace lve {
    class LveApp {
        public:
            static constexpr int HEIGHT = 600;
            static constexpr int WIDTH = 800;
            void run();

            LveApp();
            ~LveApp();

            LveApp(const LveApp &) = delete;
            LveApp &operator=(const LveApp &) = delete;

        private:
            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();
            void drawFrame();
            void loadModels();

            LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
            LveDevice lveDevice{lveWindow};
            LveSwapChain lveSwapchain{lveDevice, lveWindow.getExtent()};
            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
            std::unique_ptr<LveModel> lveModel;
    };
}