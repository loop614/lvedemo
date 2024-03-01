#include "lve_app.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <stdexcept>

namespace lve {
    struct SimplePushConstantData {
        glm::mat2 transform{1.f};
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    LveApp::LveApp() {
        loadGameObjects();
        createPipelineLayout();
        createPipeline();
    }

    LveApp::~LveApp() {
        vkDestroyPipelineLayout(this->lveDevice.device(), this->pipelineLayout, nullptr);
    }

    void LveApp::run() {
        while(!this->lveWindow.shouldClose()) {
            glfwPollEvents();
            if (VkCommandBuffer commandBuffer = this->lveRenderer.beginFrame()) {
                this->lveRenderer.beginSwapChainRenderPass(commandBuffer);
                this->renderGameObjects(commandBuffer);
                this->lveRenderer.endSwapChainRenderPass(commandBuffer);
                this->lveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(this->lveDevice.device());
    };

    void LveApp::loadGameObjects() {
        std::vector<LveModel::Vertex> vertices {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };
        std::shared_ptr<LveModel> lveModel = std::make_shared<LveModel>(this->lveDevice, vertices);

        LveGameObject triangle = LveGameObject::createGameObject();
        triangle.model = lveModel;
        triangle.color = {.1f, .8f, .1f};
        triangle.transform2d.translation.x = .2f;
        triangle.transform2d.scale = {2.f, .5f};
        triangle.transform2d.rotation = .25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
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
        assert(this->pipelineLayout != nullptr && "cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = this->lveRenderer.getSwapChainRenderPass();
        pipelineConfig.pipelineLayout = this->pipelineLayout;
        this->lvePipeline = std::make_unique<LvePipeline>(
            this->lveDevice,
            "shaders/vert.spv",
            "shaders/frag.spv",
            pipelineConfig
        );
    }

    void LveApp::renderGameObjects(VkCommandBuffer commandBuffer) {
        this->lvePipeline->bind(commandBuffer);

        for (auto& obj : gameObjects) {
            obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());
            SimplePushConstantData push{};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            push.transform = obj.transform2d.mat2();

            vkCmdPushConstants(
                commandBuffer,
                this->pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }
}
