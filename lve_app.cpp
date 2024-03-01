#include "lve_app.hpp"

#include <array>
#include <stdexcept>

namespace lve {
    LveApp::LveApp() {
        loadModels();
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }
    LveApp::~LveApp() {
        vkDestroyPipelineLayout(this->lveDevice.device(), this->pipelineLayout, nullptr);
    }

    void LveApp::run() {
        while(!this->lveWindow.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(this->lveDevice.device());
    };

    void LveApp::loadModels() {
        std::vector<LveModel::Vertex> vertices {{{0.0f, -0.5f}},{{0.5f, 0.5f}},{{-0.5f, 0.5f}}};
        this->lveModel = std::make_unique<LveModel>(this->lveDevice, vertices);
    }

    void LveApp::createPipelineLayout() {
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

    void LveApp::createPipeline() {
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

    void LveApp::createCommandBuffers() {
        this->commandBuffers.resize(this->lveSwapchain.imageCount());

        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = this->lveDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(this->commandBuffers.size());

        if (vkAllocateCommandBuffers(this->lveDevice.device(), &allocInfo, this->commandBuffers.data()) != VK_SUCCESS) {
            std::runtime_error("failed to allocate command buffers");
        }

        for (int i = 0; i < this->commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(this->commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = this->lveSwapchain.getRenderPass();
            renderPassInfo.framebuffer = this->lveSwapchain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = this->lveSwapchain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(this->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            this->lvePipeline->bind(this->commandBuffers[i]);
            this->lveModel->bind(this->commandBuffers[i]);
            this->lveModel->draw(this->commandBuffers[i]);

            vkCmdEndRenderPass(this->commandBuffers[i]);
            if (vkEndCommandBuffer(this->commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer");
            }
        }
    }

    void LveApp::drawFrame() {
        uint32_t imageIndex;
        auto result = this->lveSwapchain.acquireNextImage(&imageIndex);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to aquire swap chain image");
        }

        result = this->lveSwapchain.submitCommandBuffers(&this->commandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image");
        }
    }
}
