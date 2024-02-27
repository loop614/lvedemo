#include "first_app.hpp"

namespace lve {
    void FirstApp::run() {
        while(!this->lveWindow.shouldClose()) {
            glfwPollEvents();
        }
    };
}
