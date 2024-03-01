#include "lve_app.hpp"
#include "lve_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <stdexcept>

namespace lve {
    LveApp::LveApp() { this->loadGameObjects(); }
    LveApp::~LveApp() {}

    void LveApp::run() {
        LveRenderSystem renderSystem{this->lveDevice, this->lveRenderer.getSwapChainRenderPass()};

        while(!this->lveWindow.shouldClose()) {
            glfwPollEvents();
            if (VkCommandBuffer commandBuffer = this->lveRenderer.beginFrame()) {
                this->lveRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(commandBuffer, this->gameObjects);
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
}
