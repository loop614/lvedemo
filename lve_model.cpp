#include "lve_model.hpp"

#include <cassert>
#include <cstring>
#include <iostream>

namespace lve
{
    LveModel::LveModel(LveDevice &device, const std::vector<Vertex> &vertices) : lveDevice(device)
    {
        createVertexBuffers(vertices);
    }

    LveModel::~LveModel()
    {
        vkDestroyBuffer(this->lveDevice.device(), this->vertexBuffer, nullptr);
        vkFreeMemory(this->lveDevice.device(), this->vertexBufferMemory, nullptr);
    }

    void LveModel::createVertexBuffers(const std::vector<Vertex> &vertices)
    {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        this->lveDevice.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            this->vertexBuffer,
            this->vertexBufferMemory);

        void *data;
        vkMapMemory(this->lveDevice.device(), this->vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(this->lveDevice.device(), this->vertexBufferMemory);
    }

    void LveModel::draw(VkCommandBuffer commandBuffer)
    {
        vkCmdDraw(commandBuffer, this->vertexCount, 1, 0, 0);
    }

    void LveModel::bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = {this->vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    std::vector<VkVertexInputBindingDescription> LveModel::Vertex::getBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> LveModel::Vertex::getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = 0;

        return attributeDescriptions;
    }
}
