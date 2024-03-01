#include "lve_app.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <array>
#include <stdexcept>

namespace lve {
    struct SimplePushConstantData {
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    LveApp::LveApp() {
        loadModels();
        createPipelineLayout();
        recreateSwapChain();
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
        std::vector<LveModel::Vertex> vertices {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };
        this->lveModel = std::make_unique<LveModel>(this->lveDevice, vertices);
    }

    void LveApp::createPipelineLayout() {
        VkPushConstantRange pushConstantRange;
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(this->lveDevice.device(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void LveApp::createPipeline() {
        assert(this->lveSwapchain != nullptr && "cannot create pipeline before swap chain");
        assert(this->pipelineLayout != nullptr && "cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = this->lveSwapchain->getRenderPass();
        pipelineConfig.pipelineLayout = this->pipelineLayout;
        this->lvePipeline = std::make_unique<LvePipeline>(
            this->lveDevice,
            "shaders/vert.spv",
            "shaders/frag.spv",
            pipelineConfig
        );
    }

    void LveApp::recreateSwapChain() {
        VkExtent2D extent = this->lveWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = this->lveWindow.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(this->lveDevice.device());

        if (this->lveSwapchain == nullptr) {
            this->lveSwapchain = std::make_unique<LveSwapChain>(this->lveDevice, extent);
        } else {
            this->lveSwapchain = std::make_unique<LveSwapChain>(this->lveDevice, extent, std::move(this->lveSwapchain));
            if (this->lveSwapchain->imageCount() != this->commandBuffers.size()) {
                this->freeCommandBuffers();
                this->createCommandBuffers();
            }
        }

        createPipeline();
    }

    void LveApp::createCommandBuffers() {
        this->commandBuffers.resize(this->lveSwapchain->imageCount());

        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = this->lveDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(this->commandBuffers.size());

        if (vkAllocateCommandBuffers(this->lveDevice.device(), &allocInfo, this->commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void LveApp::freeCommandBuffers() {
        vkFreeCommandBuffers(
            this->lveDevice.device(),
            this->lveDevice.getCommandPool(),
            static_cast<uint32_t>(this->commandBuffers.size()),
            this->commandBuffers.data()
        );
        this->commandBuffers.clear();
    }

    void LveApp::recordCommandBuffer(int imageIndex) {
        static int frame = 30;
        frame = (frame + 1) % 100;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(this->commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = this->lveSwapchain->getRenderPass();
        renderPassInfo.framebuffer = this->lveSwapchain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = this->lveSwapchain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(this->commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(this->lveSwapchain->getSwapChainExtent().width);
            viewport.height = static_cast<float>(this->lveSwapchain->getSwapChainExtent().height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            VkRect2D scissor{{0, 0}, this->lveSwapchain->getSwapChainExtent()};
            vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
            vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

            this->lvePipeline->bind(this->commandBuffers[imageIndex]);
            this->lveModel->bind(this->commandBuffers[imageIndex]);

            for (int j = 0; j < 4; j++) {
                SimplePushConstantData push{};
                push.offset = {-0.5f + frame * 0.02, -0.4f + j * 0.25f};
                push.color = {0.0f, 0.0f, 0.2f + 0.2f * j};

                vkCmdPushConstants(
                    this->commandBuffers[imageIndex],
                    this->pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(SimplePushConstantData),
                    &push
                );
                this->lveModel->draw(this->commandBuffers[imageIndex]);
            }
        vkCmdEndRenderPass(this->commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(this->commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer");
        }
    }

    void LveApp::drawFrame() {
        uint32_t imageIndex;
        VkResult result = this->lveSwapchain->acquireNextImage(&imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            this->recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to aquire swap chain image");
        }

        this->recordCommandBuffer(imageIndex);
        result = this->lveSwapchain->submitCommandBuffers(&this->commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || this->lveWindow.wasWindowResized()) {
            this->lveWindow.resetWindowResizedFlag();
            this->recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image");
        }
    }
}
