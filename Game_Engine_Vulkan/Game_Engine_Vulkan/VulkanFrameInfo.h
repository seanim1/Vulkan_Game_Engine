#pragma once

#include "VulkanCamera.h"
#include "VulkanGameObject.h"
// lib
#include <vulkan/vulkan.h>

namespace VulkanEngine {

	#define MAX_LIGHTS 10

	struct PointLight {
		glm::vec3 position{};  // ignore w
		alignas(16) glm::vec4 color{};     // w is intensity
	};

	struct GlobalUbo {
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inverseView{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };  // w is intensity
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
		float t; // time
	};

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		VulkanCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		VulkanGameObject::Map& gameLightObjects;
		VulkanGameObject::Map& gameMeshObjects;
		VulkanGameObject& gamePlayer;
	};
}  // namespace lve