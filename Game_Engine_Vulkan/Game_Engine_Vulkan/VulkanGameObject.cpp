#include "VulkanGameObject.h"

namespace VulkanEngine {
    glm::mat4 TransformComponent::mat4() {
        const float Cz = glm::cos(rotation.z);
        const float Sz = glm::sin(rotation.z);
        const float Cx = glm::cos(rotation.x);
        const float Sx = glm::sin(rotation.x);
        const float Cy = glm::cos(rotation.y);
        const float Sy = glm::sin(rotation.y);
        return glm::mat4{
            {
                scale.x * (Cy * Cz + Sy * Sx * Sz),
                scale.x * (Cx * Sz),
                scale.x * (Cy * Sx * Sz - Cz * Sy),
                0.0f,
            },
            {
                scale.y * (Cz * Sy * Sx - Cy * Sz),
                scale.y * (Cx * Cz),
                scale.y * (Cy * Cz * Sx + Sy * Sz),
                0.0f,
            },
            {
                scale.z * (Cx * Sy),
                scale.z * (-Sx),
                scale.z * (Cy * Cx),
                0.0f,
            },
            {
                translation.x,
                translation.y,
                translation.z,
                1.0f
            }
        };
    }

    glm::mat3 TransformComponent::normalMatrix()
    {
        const float Cz = glm::cos(rotation.z);
        const float Sz = glm::sin(rotation.z);
        const float Cx = glm::cos(rotation.x);
        const float Sx = glm::sin(rotation.x);
        const float Cy = glm::cos(rotation.y);
        const float Sy = glm::sin(rotation.y);
        const glm::vec3 invScale = 1.0f / scale;

        return glm::mat3{
            {
                invScale.x * (Cy * Cz + Sy * Sx * Sz),
                invScale.x * (Cx * Sz),
                invScale.x * (Cy * Sx * Sz - Cz * Sy),
            },
            {
                invScale.y * (Cz * Sy * Sx - Cy * Sz),
                invScale.y * (Cx * Cz),
                invScale.y * (Cy * Cz * Sx + Sy * Sz),
            },
            {
                invScale.z * (Cx * Sy),
                invScale.z * (-Sx),
                invScale.z * (Cy * Cx),
            },
        };
    }
    VulkanGameObject VulkanGameObject::makePointLight(float intensity, float radius, glm::vec3 color)
    {
        VulkanGameObject gameObj = VulkanGameObject::createGameObject();
        gameObj.color = color;
        gameObj.transform.scale.x = radius;
        gameObj.pointLight = std::make_unique<PointLightComponent>();
        gameObj.pointLight->lightIntensity = intensity;
        return gameObj;
    }
}