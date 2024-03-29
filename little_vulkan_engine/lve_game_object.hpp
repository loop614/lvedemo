#pragma once

#include "lve_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace lve
{
    struct TransformComponent
    {
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};

        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    struct PointLightComponent {
        float lightIntesity = 1.0f;
    };

    class LveGameObject
    {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, LveGameObject>;

        std::shared_ptr<LveModel> model{};
        glm::vec3 color{};
        TransformComponent transform{};

        std::unique_ptr<PointLightComponent> pointLight = nullptr;

        static LveGameObject createGameObject()
        {
            static id_t currentId = 0;
            return LveGameObject{currentId++};
        }

        LveGameObject(const LveGameObject &) = delete;
        LveGameObject &operator=(const LveGameObject &) = delete;
        LveGameObject(LveGameObject &&) = default;
        LveGameObject &operator=(LveGameObject &&) = default;

        id_t getId() { return id; }
        static LveGameObject makePointLight(
            float intesity = 10.f,
            float radius = 0.1f,
            glm::vec3 color = glm::vec3(1.f)
        );

    private:
        LveGameObject(id_t objId) : id{objId} {}
        id_t id;
    };
}
