#pragma once

#include "lve_game_object.hpp"
#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_renderer.hpp"

#include <memory>
#include <vector>

namespace lve {
    class LveApp {
        public:
            static constexpr int HEIGHT = 600;
            static constexpr int WIDTH = 800;
            void run();

            LveApp();
            ~LveApp();

            LveApp(const LveApp &) = delete;
            LveApp &operator=(const LveApp &) = delete;

        private:
            void loadGameObjects();
            LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
            LveDevice lveDevice{lveWindow};
            LveRenderer lveRenderer{lveWindow, lveDevice};
            std::vector<LveGameObject> gameObjects;
    };
}
