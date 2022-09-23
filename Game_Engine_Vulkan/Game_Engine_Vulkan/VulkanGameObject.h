#pragma once
#include "VulkanModel.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include <memory>
namespace VulkanEngine {
	struct TransformComponent {
		glm::vec3 translation{};  // (position offset)
		/*glm::vec3 veloDir{};
		glm::vec3 veloMag{};
		glm::vec3 accelDir{};
		glm::vec3 accelMag{};*/
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};
        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	struct PointLightComponent {
		float lightIntensity = 1.0f;
	};

	class VulkanGameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, VulkanGameObject>;

		static VulkanGameObject createGameObject() {
			static id_t currentId = 0;
			return VulkanGameObject{ currentId++ };
		}

		static VulkanGameObject makePointLight(
			float intensity = 5.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f)
		);

		VulkanGameObject(const VulkanGameObject&) = delete;
		VulkanGameObject& operator=(const VulkanGameObject&) = delete;
		VulkanGameObject(VulkanGameObject&&) = default;
		VulkanGameObject& operator=(VulkanGameObject&&) = default;

		id_t getId() { return id; }

		glm::vec3 color{};
		TransformComponent transform{};

		// Optional pointer components.
		std::shared_ptr<VulkanModel> model{}; // used by: SimpleRender
		// ex) Floor and Objects are not point light, so they don't need these.
		std::unique_ptr<PointLightComponent> pointLight = nullptr; // used by: Light

	private:
		VulkanGameObject(id_t parameter_id) : id{ parameter_id } {}

		id_t id;
	};
}