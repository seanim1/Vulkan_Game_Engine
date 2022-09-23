#pragma once
#include "VulkanCamera.h"
#include "VulkanPipeline.h"
#include "VulkanDevice.h"
#include "VulkanGameObject.h"
#include "VulkanFrameInfo.h"

// std
#include <vector>

namespace VulkanEngine {
	class WireframeSystem {
	public:
		WireframeSystem(
			VulkanDevice& device,
			VkRenderPass renderPass,
			VkDescriptorSetLayout globalSetLayout
		);

		~WireframeSystem();

		WireframeSystem(const WireframeSystem&) = delete; // deleting copy constructors
		WireframeSystem& operator=(const WireframeSystem&) = delete;

	void render(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		VulkanDevice& vulkanDevice;

		std::unique_ptr<VulkanPipeline> vulkanPipeline;
		VkPipelineLayout pipelineLayout;
	};
}