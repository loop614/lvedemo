#include "lve_window.hpp"
#include <stdexcept>

namespace lve {
    LveWindow::LveWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
        initWindow();
    }

    LveWindow::~LveWindow() {
        glfwDestroyWindow(this->window);
        glfwTerminate();
    }

    void LveWindow::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        this->window = glfwCreateWindow(this->width, this->height, this->windowName.c_str(), nullptr, nullptr);
    }

    void LveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        if (glfwCreateWindowSurface(instance, this->window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface");
        }
    }
}
