#include "first_app.hpp"
#include <stdexcept>

namespace lve {
    FirstApp::FirstApp() {
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }
    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(this->lveDevice.device(), this->pipelineLayout, nullptr);
    }

    void FirstApp::run() {
        while(!this->lveWindow.shouldClose()) {
            glfwPollEvents();
        }
    };

    void FirstApp::createPipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(this->lveDevice.device(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void FirstApp::createPipeline() {
        PipelineConfigInfo pipelineConfig = LvePipeline::defaultPipelineConfigInfo(this->lveSwapchain.width(), this->lveSwapchain.height());
        pipelineConfig.renderPass = lveSwapchain.getRenderPass();
        pipelineConfig.pipelineLayout = this->pipelineLayout;
        this->lvePipeline = std::make_unique<LvePipeline>(
            this->lveDevice,
            "shaders/vert.spv",
            "shaders/frag.spv",
            pipelineConfig
        );
    }

    void FirstApp::createCommandBuffers() {}
    void FirstApp::drawFrame() {}
}
