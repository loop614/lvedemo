#include "lve_model.hpp"
#include "lve_utils.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <cstring>
#include <memory>
#include <unordered_map>

namespace std
{
    template <>
    struct hash<lve::LveModel::Vertex>
    {
        size_t operator()(lve::LveModel::Vertex const &vertex) const
        {
            size_t seed = 0;
            lve::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);

            return seed;
        }
    };
}

namespace lve
{
    LveModel::LveModel(LveDevice &device, const LveModel::Builder &builder) : lveDevice(device)
    {
        createVertexBuffers(builder.vertices);
        createIndexBuffers(builder.indices);
    }

    LveModel::~LveModel()
    {
        vkDestroyBuffer(this->lveDevice.device(), this->vertexBuffer, nullptr);
        vkFreeMemory(this->lveDevice.device(), this->vertexBufferMemory, nullptr);

        if (this->hasIndexBuffer)
        {
            vkDestroyBuffer(this->lveDevice.device(), this->indexBuffer, nullptr);
            vkFreeMemory(this->lveDevice.device(), this->indexBufferMemory, nullptr);
        }
    }

    std::unique_ptr<LveModel> LveModel::createModelFromFile(LveDevice &device, const std::string &filepath)
    {
        Builder builder{};
        builder.loadModel(filepath);

        return std::make_unique<LveModel>(device, builder);
    }

    void LveModel::createVertexBuffers(const std::vector<Vertex> &vertices)
    {
        this->vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        this->lveDevice.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void *data;
        vkMapMemory(this->lveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(this->lveDevice.device(), stagingBufferMemory);

        this->lveDevice.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            this->vertexBuffer,
            this->vertexBufferMemory);

        lveDevice.copyBuffer(stagingBuffer, this->vertexBuffer, bufferSize);

        vkDestroyBuffer(this->lveDevice.device(), stagingBuffer, nullptr);
        vkFreeMemory(this->lveDevice.device(), stagingBufferMemory, nullptr);
    }

    void LveModel::createIndexBuffers(const std::vector<uint32_t> &indices)
    {
        this->indexCount = static_cast<uint32_t>(indices.size());
        this->hasIndexBuffer = indexCount > 0;
        if (!this->hasIndexBuffer)
        {
            return;
        }

        VkDeviceSize bufferSize = sizeof(indices[0]) * this->indexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        this->lveDevice.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void *data;
        vkMapMemory(this->lveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(this->lveDevice.device(), stagingBufferMemory);

        this->lveDevice.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            this->indexBuffer,
            this->indexBufferMemory);

        lveDevice.copyBuffer(stagingBuffer, this->indexBuffer, bufferSize);

        vkDestroyBuffer(this->lveDevice.device(), stagingBuffer, nullptr);
        vkFreeMemory(this->lveDevice.device(), stagingBufferMemory, nullptr);
    }

    void LveModel::draw(VkCommandBuffer commandBuffer)
    {
        if (this->hasIndexBuffer)
        {
            vkCmdDrawIndexed(commandBuffer, this->indexCount, 1, 0, 0, 0);
        }
        else
        {
            vkCmdDraw(commandBuffer, this->vertexCount, 1, 0, 0);
        }
    }

    void LveModel::bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = {this->vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if (this->hasIndexBuffer)
        {
            vkCmdBindIndexBuffer(commandBuffer, this->indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
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
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
        attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});

        return attributeDescriptions;
    }

    void LveModel::Builder::loadModel(const std::string &filePath)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        this->vertices.clear();
        this->indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for (const auto &shape : shapes)
        {
            for (const auto &index : shape.mesh.indices)
            {
                Vertex vertex{};
                if (index.vertex_index >= 0)
                {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]};

                    vertex.color = {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2]};
                }
                if (index.normal_index >= 0)
                {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]};
                }
                if (index.texcoord_index >= 0)
                {
                    vertex.uv = {
                        attrib.normals[2 * index.texcoord_index + 0],
                        attrib.normals[2 * index.texcoord_index + 1]};
                }

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    this->vertices.push_back(vertex);
                }
                this->indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
}
