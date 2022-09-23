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
	class PlayerSystem {
	public:

		PlayerSystem(
			VulkanDevice& device,
			VkRenderPass renderPass,
			VkDescriptorSetLayout globalSetLayout
		);
		~PlayerSystem();

		PlayerSystem(const PlayerSystem&) = delete; // deleting copy constructors
		PlayerSystem& operator=(const PlayerSystem&) = delete;

		void update(FrameInfo& frameInfo);
		void render(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		VulkanDevice& vulkanDevice;

		std::unique_ptr<VulkanPipeline> vulkanPipeline;
		VkPipelineLayout pipelineLayout;
	};
}
