#pragma once

#include "lve_pipeline.hpp"
#include "lve_window.hpp"

namespace lve {
    class FirstApp {
        public:
            static constexpr int HEIGHT = 600;
            static constexpr int WIDTH = 800;
            void run();

        private:
            LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
            LvePipeline lvePipeline{"shaders/vert.spv", "shaders/frag.spv"};
    };
}
