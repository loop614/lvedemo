#include "little_vulkan_engine/lve_app.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
    lve::LveApp app{};
    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
