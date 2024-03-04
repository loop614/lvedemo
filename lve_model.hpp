#pragma once

#include "lve_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace lve
{
    class LveModel
    {
    public:
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
        };

        LveModel(LveDevice &device, const LveModel::Builder &builder);
        ~LveModel();

        LveModel(const LveModel &) = delete;
        LveModel &operator=(const LveModel &) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        LveDevice &lveDevice;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;

        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexCount;

        bool hasIndexBuffer = false;

        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t> &indices);
    };
}
