#pragma once

#include "VulkanGameObject.h"
#include "VulkanWindow.h"

namespace VulkanEngine {
	class KeyboardMovementController {
	public:
		glm::vec3 getInputDirection(GLFWwindow* window, float dt, VulkanGameObject& gameObject);
		void moveInPlaneXZ(glm::vec3 moveDir, float dt, VulkanGameObject& gameObject);
		void physics(float dt, VulkanGameObject& gameObject);

		glm::vec3 veloDir{ 0, 0, -1 };
		float veloMag{ 0.f };
		glm::vec3 accelDir{ 0, 0, -1 };
		float accelMag{ 15.0f };

		float frictionMag{ 50.0f };

		const float maxSpeed{ 25.f };
		const float lookSpeed{ 1.5f };
	};
}