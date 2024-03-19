#pragma once

#include "lve_camera.hpp"
#include "lve_pipeline.hpp"
#include "lve_game_object.hpp"
#include "lve_device.hpp"
#include "lve_frame_info.hpp"

#include <memory>
#include <vector>

namespace lve
{
    class LveRenderSystem
    {
    public:
        LveRenderSystem(LveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~LveRenderSystem();

        LveRenderSystem(const LveRenderSystem &) = delete;
        LveRenderSystem &operator=(const LveRenderSystem &) = delete;

        void renderGameObjects(
            FrameInfo &frameInfo,
            std::vector<LveGameObject> &gameObjects);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        LveDevice &lveDevice;

        std::unique_ptr<LvePipeline> lvePipeline;
        VkPipelineLayout pipelineLayout;
    };
}
