#include "lve_app.hpp"
#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "lve_buffer.hpp"

#include "lve_render_system.hpp"
#include "point_light_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <stdexcept>
#include <chrono>

namespace lve
{
    LveApp::LveApp()
    {
        this->globalPool = LveDescriptorPool::Builder(this->lveDevice)
                               .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
                               .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
                               .build();

        this->loadGameObjects();
    }

    LveApp::~LveApp() {}

    void LveApp::run()
    {
        std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<LveBuffer>(
                this->lveDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

            uboBuffers[i]->map();
        }

        std::unique_ptr<LveDescriptorSetLayout> globalSetLayout = LveDescriptorSetLayout::Builder(this->lveDevice)
                                                                      .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                                                                      .build();

        std::vector<VkDescriptorSet> globalDescriptorSets{LveSwapChain::MAX_FRAMES_IN_FLIGHT};
        for (int i = 0; i < globalDescriptorSets.size(); i++)
        {
            VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->descriptorInfo();
            LveDescriptorWriter(*globalSetLayout, *this->globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        LveRenderSystem renderSystem{this->lveDevice, this->lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        PointLightSystem pointLightSystem{this->lveDevice, this->lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        LveCamera camera{};
        // camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewerObject = LveGameObject::createGameObject();
        viewerObject.transform.translation.z = -2;

        KeyboardMovementController cameraController{};
        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!this->lveWindow.shouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(this->lveWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = this->lveRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 30.f);

            if (VkCommandBuffer commandBuffer = this->lveRenderer.beginFrame())
            {
                int frameIndex = this->lveRenderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                    this->gameObjects};

                // update
                GlobalUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                pointLightSystem.update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                this->lveRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);
                this->lveRenderer.endSwapChainRenderPass(commandBuffer);
                this->lveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(this->lveDevice.device());
    };

    void LveApp::loadGameObjects()
    {
        std::shared_ptr<LveModel> flatVaseModel = LveModel::createModelFromFile(this->lveDevice, "models/flat_vase.obj");
        LveGameObject flatVase = LveGameObject::createGameObject();
        flatVase.model = flatVaseModel;
        flatVase.transform.translation = {-0.5f, .5f, 0.0f};
        flatVase.transform.scale = glm::vec3{3.f, 1.5f, 3.f};
        this->gameObjects.emplace(flatVase.getId(), std::move(flatVase));

        std::shared_ptr<LveModel> smoothVaseModel = LveModel::createModelFromFile(this->lveDevice, "models/smooth_vase.obj");
        LveGameObject smoothVase = LveGameObject::createGameObject();
        smoothVase.model = smoothVaseModel;
        smoothVase.transform.translation = {.5f, .5f, 0.0f};
        smoothVase.transform.scale = glm::vec3{3.f, 1.5f, 3.f};
        this->gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

        std::shared_ptr<LveModel> floorModel = LveModel::createModelFromFile(this->lveDevice, "models/quad.obj");
        LveGameObject floor = LveGameObject::createGameObject();
        floor.model = floorModel;
        floor.transform.translation = {.5f, .5f, 0.0f};
        floor.transform.scale = glm::vec3{3.f, 1.5f, 3.f};
        this->gameObjects.emplace(floor.getId(), std::move(floor));

        auto pointLight = LveGameObject::makePointLight(0.2f);
        this->gameObjects.emplace(pointLight.getId(), std::move(pointLight));
        std::vector<glm::vec3> lightColors{
            {1.f, .1f, .1f},
            {.1f, .1f, 1.f},
            {.1f, 1.f, .1f},
            {1.f, 1.f, .1f},
            {.1f, 1.f, 1.f},
            {1.f, 1.f, 1.f}
        };

        for (int i = 0; i < lightColors.size(); i++) {
            auto pointLight = LveGameObject::makePointLight(0.2f);
            pointLight.color = lightColors[i];
            auto rotateLight = glm::rotate(
                glm::mat4(1.f),
                (i * glm::two_pi<float>()) / lightColors.size(),
                {0.f, -1.f, 0.f});
            pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
            gameObjects.emplace(pointLight.getId(), std::move(pointLight));
        }
    }
}
