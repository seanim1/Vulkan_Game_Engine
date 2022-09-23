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
	class SimpleRenderSystem {
	public:

		SimpleRenderSystem(
			VulkanDevice& device, 
			VkRenderPass renderPass, 
			VkDescriptorSetLayout globalSetLayout
		);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete; // deleting copy constructors
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(FrameInfo& frameInfo);

	private:
		// Simple Render System - Anything that acts upon a subset of a game object components is a system
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		VulkanDevice& vulkanDevice;

		std::unique_ptr<VulkanPipeline> vulkanPipeline;
		VkPipelineLayout pipelineLayout;
	};
}
