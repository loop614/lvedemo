#pragma once

#include "lve_device.hpp"

#include <string>
#include <vector>

namespace lve
{
    struct PipelineConfigInfo
    {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;

        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class LvePipeline
    {
    public:
        LvePipeline(
            LveDevice &device,
            const std::string &vertFilePath,
            const std::string &fragFilePath,
            const PipelineConfigInfo &configInfo);

        ~LvePipeline();
        LvePipeline(const LvePipeline&) = delete;
        LvePipeline& operator=(const LvePipeline&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

    private:
        LveDevice &lveDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;

        static std::vector<char> readFile(const std::string &filePath);
        void createGraphicsPipeline(
            const std::string &vertFilePath,
            const std::string &fragFilePath,
            const PipelineConfigInfo &configInfo);
        void createShaderModule(const std::vector<char> &code, VkShaderModule* shaderModule);
    };
}
