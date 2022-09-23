#pragma once

#include "VulkanCamera.h"
#include "VulkanPipeline.h"
#include "VulkanDevice.h"
#include "VulkanGameObject.h"
#include "VulkanFrameInfo.h"

// std
#include <memory>
#include <vector>
namespace VulkanEngine {
	class PointLightSystem {
	public:

		PointLightSystem(
			VulkanDevice& device,
			VkRenderPass renderPass,
			VkDescriptorSetLayout globalSetLayout
		);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete; // deleting copy constructors
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo& ubo);
		void render(FrameInfo& frameInfo);

	private:
		// Simple Render System - Anything that acts upon a subset of a game object components is a system
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		VulkanDevice& vulkanDevice;

		std::unique_ptr<VulkanPipeline> vulkanPipeline;
		VkPipelineLayout pipelineLayout;
	};
}
