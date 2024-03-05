#include "lve_app.hpp"
#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "lve_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <stdexcept>
#include <chrono>

namespace lve
{
    LveApp::LveApp() { this->loadGameObjects(); }
    LveApp::~LveApp() {}

    void LveApp::run()
    {
        LveRenderSystem renderSystem{this->lveDevice, this->lveRenderer.getSwapChainRenderPass()};
        LveCamera camera{};
        // camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewerObject = LveGameObject::createGameObject();
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
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

            if (VkCommandBuffer commandBuffer = this->lveRenderer.beginFrame())
            {
                this->lveRenderer.beginSwapChainRenderPass(commandBuffer);
                renderSystem.renderGameObjects(commandBuffer, this->gameObjects, camera);
                this->lveRenderer.endSwapChainRenderPass(commandBuffer);
                this->lveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(this->lveDevice.device());
    };

    std::unique_ptr<LveModel> LveApp::createCubeModel(LveDevice &device, glm::vec3 offset)
    {
        LveModel::Builder modelBuilder{};
        modelBuilder.vertices = {
            // west white
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

            // east yellow
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

            // top black
            {{-.5f, -.5f, -.5f}, {0.0f, 0.0f, 0.0f}},
            {{.5f, -.5f, .5f}, {0.0f, 0.0f, 0.0f}},
            {{-.5f, -.5f, .5f}, {0.0f, 0.0f, 0.0f}},
            {{.5f, -.5f, -.5f}, {0.0f, 0.0f, 0.0f}},

            // bottom red
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

            // north blue
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

            // south green
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        };
        for (auto &v : modelBuilder.vertices)
        {
            v.position += offset;
        }

        modelBuilder.indices = {
            0, 1, 2, 0, 3, 1,
            4, 5, 6, 4, 7, 5,
            8, 9, 10, 8, 11, 9,
            12, 13, 14, 12, 15, 13,
            16, 17, 18, 16, 19, 17,
            20, 21, 22, 20, 23, 21
        };

        return std::make_unique<LveModel>(device, modelBuilder);
    }

    void LveApp::loadGameObjects()
    {
        std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(this->lveDevice, "models/colored_cube.obj");
        LveGameObject cube = LveGameObject::createGameObject();
        cube.model = lveModel;
        cube.transform.translation = {.0f, .0f, 2.5f};
        cube.transform.scale = {.5f, .5f, .5f};
        this->gameObjects.push_back(std::move(cube));
    }
}
