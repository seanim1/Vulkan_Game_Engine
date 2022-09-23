#include "keyboard_movement_controller.h"
#include "print_utility.h"

#include <iostream>

namespace VulkanEngine {
	glm::vec3 KeyboardMovementController::getInputDirection(GLFWwindow* window, float dt, VulkanGameObject& gameObject)
	{
		glm::vec3 rotate{ 0 };
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) rotate.y += 1.f;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) rotate.y -= 1.f;
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) rotate.x += 1.f;
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) rotate.x -= 1.f;

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
			gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
		}

		// limit pitch values between about +/- 85ish degrees
		gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
		gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

		float yaw = gameObject.transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
		const glm::vec3 upDir{ 0.f, -1.f, 0.f };

		glm::vec3 moveDir{ 0.f };
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) moveDir -= upDir;

		return moveDir;
	}
	void KeyboardMovementController::moveInPlaneXZ(glm::vec3 inputDir, float dt, VulkanGameObject& gameObject)
	{
		if (glm::dot(inputDir, inputDir) > std::numeric_limits<float>::epsilon()) {
			if (veloMag < maxSpeed) {
				veloMag += accelMag * dt;
			}
			accelDir = inputDir;
		}
		else {
			if (veloMag > 0) {
				veloMag -= frictionMag * dt;
			}
		}
	}
	void KeyboardMovementController::physics(float dt, VulkanGameObject& gameObject)
	{
		gameObject.transform.translation += veloMag * dt * glm::normalize(accelDir);
	}
}

