#pragma once

#include <memory>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "lve_pipeline.hpp"
#include "lve_game_object.hpp"
#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_renderer.hpp"

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
            void loadGameObjects();
            void createPipelineLayout();
            void createPipeline();
            void renderGameObjects(VkCommandBuffer commandBuffer);

            LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
            LveDevice lveDevice{lveWindow};
            LveRenderer lveRenderer{lveWindow, lveDevice};

            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<LveGameObject> gameObjects;
    };
}
