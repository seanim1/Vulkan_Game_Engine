#include "WireframeSystem.h"
namespace VulkanEngine {
	struct ShapePushConstant {
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	WireframeSystem::WireframeSystem(VulkanDevice& device, VkRenderPass renderPass,
		VkDescriptorSetLayout globalSetLayout) : vulkanDevice{ device }
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	WireframeSystem::~WireframeSystem()
	{
		vkDestroyPipelineLayout(vulkanDevice.device(), pipelineLayout, nullptr);
	}
	void WireframeSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(ShapePushConstant);

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
	void WireframeSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		pipelineConfig.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		pipelineConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;
		pipelineConfig.rasterizationInfo.lineWidth = 5.0f;

		vulkanPipeline = std::make_unique<VulkanPipeline>(
			vulkanDevice,
			"shaders/wireframe_shader.vert.spv",
			"shaders/wireframe_shader.frag.spv",
			pipelineConfig);
	}

	void WireframeSystem::render(FrameInfo& frameInfo)
	{
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
		for (auto& kv : frameInfo.gameMeshObjects) {
			auto& obj = kv.second;
			//std::cout << "gameObjects1: " << kv.first << "\n";
			//if (obj.model == nullptr) continue;

			ShapePushConstant push{};
			push.modelMatrix = obj.transform.mat4();
			//push.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(ShapePushConstant),
				&push);
			obj.model->bind(frameInfo.commandBuffer);
			obj.model->draw(frameInfo.commandBuffer);
		}
	}
}