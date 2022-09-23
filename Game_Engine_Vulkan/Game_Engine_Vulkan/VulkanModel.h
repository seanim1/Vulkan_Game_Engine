#pragma once

#include "VulkanDevice.h"
#include "VulkanBuffer.h"
// libs
#define GLM_FORCE_RADIAN
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
//std
#include <memory>
#include <vector>

namespace VulkanEngine {
	class VulkanModel {
	public:
		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
			bool operator==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal &&
					uv == other.uv;
			}
		};
		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
			std::vector<VulkanModel::Vertex> graphModel(uint8_t eqn_num, glm::vec3 coefficients,
				int lower_x, int upper_x, int lower_y, int upper_y, float interval_size, glm::vec3 color);
			void visualizeNormal(std::vector<Vertex> model_vertices);
		};

		VulkanModel(VulkanDevice& device, const VulkanModel::Builder& builder);
		~VulkanModel();

		VulkanModel(const VulkanModel&) = delete; // deleting copy constructors
		VulkanModel& operator=(const VulkanModel&) = delete;

		static std::unique_ptr<VulkanModel> createModelFromFile(VulkanDevice& device, const std::string& filepath);
		static std::unique_ptr<VulkanModel> createModelFromEquation(VulkanDevice& device, uint8_t eqn_num,
			glm::vec3 coefficients, int lower_x, int upper_x, int lower_y, int upper_y, float interval_size,
			glm::vec3 color, std::vector<Vertex>& retriever_of_vertices);
		static std::unique_ptr<VulkanModel> createNormalForModel(VulkanDevice& device, std::vector<Vertex> model_vertices);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);
		VulkanDevice& vulkanDevice;
		std::unique_ptr<VulkanBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<VulkanBuffer> indexBuffer;
		uint32_t indexCount;
	};
}