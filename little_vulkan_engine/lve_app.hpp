#pragma once

#include "lve_game_object.hpp"
#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_renderer.hpp"
#include "lve_descriptors.hpp"

#include <memory>
#include <vector>

namespace lve
{
    class LveApp
    {
    public:
        static constexpr int HEIGHT = 600;
        static constexpr int WIDTH = 800;

        LveApp();
        ~LveApp();

        LveApp(const LveApp &) = delete;
        LveApp &operator=(const LveApp &) = delete;

        void run();

    private:
        void loadGameObjects();

        LveWindow lveWindow{WIDTH, HEIGHT, "Little Vulkan Engine!"};
        LveDevice lveDevice{lveWindow};
        LveRenderer lveRenderer{lveWindow, lveDevice};

        std::unique_ptr<LveDescriptorPool> globalPool{};
        std::vector<LveGameObject> gameObjects;
    };
}
