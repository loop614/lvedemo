#include "point_light_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <stdexcept>

namespace lve
{
    PointLightSystem::PointLightSystem(LveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : lveDevice{device}
    {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    PointLightSystem::~PointLightSystem()
    {
        vkDestroyPipelineLayout(this->lveDevice.device(), this->pipelineLayout, nullptr);
    }

    void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
    {
        // VkPushConstantRange pushConstantRange;
        // pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        // pushConstantRange.offset = 0;
        // pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        // pipelineLayoutInfo.pushConstantRangeCount = 1;
        // pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(this->lveDevice.device(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void PointLightSystem::createPipeline(VkRenderPass renderPass)
    {
        assert(this->pipelineLayout != nullptr && "cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = this->pipelineLayout;
        this->lvePipeline = std::make_unique<LvePipeline>(
            this->lveDevice,
            "shaders_point/vert.spv",
            "shaders_point/frag.spv",
            pipelineConfig);
    }

    void PointLightSystem::render(FrameInfo &frameInfo)
    {
        this->lvePipeline->bind(frameInfo.commandBuffer);
        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);

        vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }
}
