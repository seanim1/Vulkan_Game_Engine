#include "print_utility.h"

#include "PlayerSystem.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <stdexcept>
#include <array>

namespace VulkanEngine {
	struct SimplePushConstantData {
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	PlayerSystem::PlayerSystem(VulkanDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
		: vulkanDevice{ device }
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	PlayerSystem::~PlayerSystem()
	{
		vkDestroyPipelineLayout(vulkanDevice.device(), pipelineLayout, nullptr);
	}

	void PlayerSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(vulkanDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout)
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
	void PlayerSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		//pipelineConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;

		vulkanPipeline = std::make_unique<VulkanPipeline>(
			vulkanDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig);
	}

	void PlayerSystem::update(FrameInfo& frameInfo)
	{
		/*float moveBy = frameInfo.frameTime * -0.2f;
		glm::vec3 currentPosition = frameInfo.gamePlayer.transform.translation;
		frameInfo.gamePlayer.transform.translation = 
			{ currentPosition.x, currentPosition.y + moveBy, currentPosition.z};*/
		//printVec3(frameInfo.gamePlayer.transform.translation);
	}

	void PlayerSystem::render(FrameInfo& frameInfo) {
		vulkanPipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr
		);

		auto& obj = frameInfo.gamePlayer;

		SimplePushConstantData push{};
		push.modelMatrix = obj.transform.mat4();
		push.normalMatrix = obj.transform.normalMatrix();
		/*std::cout << "transform.translation.x: " << obj.transform.translation.x << "\n";
		std::cout << "transform.translation.y: " << obj.transform.translation.y << "\n";
		std::cout << "transform.translation.z: " << obj.transform.translation.z << "\n";*/
		vkCmdPushConstants(
			frameInfo.commandBuffer,
			pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(SimplePushConstantData),
			&push
		);
		obj.model->bind(frameInfo.commandBuffer);
		obj.model->draw(frameInfo.commandBuffer);
	}
}