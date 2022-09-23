#include "VulkanModel.h"
#include "VulkanUtility.h"
#include "Equations.h"
#include "print_utility.h"

//libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
//std
#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <cmath>

namespace std {
	template <>
	struct hash<VulkanEngine::VulkanModel::Vertex> {
		size_t operator()(VulkanEngine::VulkanModel::Vertex const& vertex) const {
			size_t seed = 0;
			VulkanEngine::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace VulkanEngine {
	VulkanModel::VulkanModel(VulkanDevice& device, const VulkanModel::Builder& builder) : vulkanDevice{ device }
	{
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);
	}

	VulkanModel::~VulkanModel() {}

	std::unique_ptr<VulkanModel> VulkanModel::createModelFromFile(VulkanDevice& device, const std::string& filepath)
	{
		Builder builder{};
		builder.loadModel(filepath);
		std::cout << "Model's Vertex count: " << builder.vertices.size() << "\n";
		std::cout << "Model's Index count: " << builder.indices.size() << "\n";
		return std::make_unique<VulkanModel>(device, builder);
	}

	std::unique_ptr<VulkanModel> VulkanModel::createModelFromEquation(VulkanDevice& device, 
		uint8_t eqn_num, glm::vec3 coefficients, int lower_x, int upper_x, int lower_y, int upper_y, float interval_density,
		glm::vec3 color, std::vector<Vertex>& retriever_of_vertices)
	{
		Builder builder{};
		/*  IMPORTANT: Keep in mind to show 10 intervals, 11 vertices are needed,
			so we must add 1 for upper_x and upper_y. */
		retriever_of_vertices = builder.graphModel(eqn_num, coefficients,
			lower_x * interval_density, (upper_x + 1) * interval_density,
			lower_y * interval_density, (upper_y + 1) * interval_density, interval_density, color);
		std::cout << "Graph's Vertex count: " << builder.vertices.size() << "\n";
		std::cout << "Graph's Index count: " << builder.indices.size() << "\n";
		return std::make_unique<VulkanModel>(device, builder);
	}

	std::unique_ptr<VulkanModel> VulkanModel::createNormalForModel(VulkanDevice& device, std::vector<Vertex> model_vertices)
	{
		Builder builder{};
		builder.visualizeNormal(model_vertices);
		std::cout << "Normal Visualizer's Vertex count: " << builder.vertices.size() << "\n";
		std::cout << "Normal Visualizer's Index count: " << builder.indices.size() << "\n";
		return std::make_unique<VulkanModel>(device, builder);
	}

	void VulkanModel::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void VulkanModel::draw(VkCommandBuffer commandBuffer)
	{
		if (hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		} else {
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
	}

	void VulkanModel::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);

		VulkanBuffer stagingBuffer{
			vulkanDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());

		vertexBuffer = std::make_unique<VulkanBuffer>(
			vulkanDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		vulkanDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
	}
	void VulkanModel::createIndexBuffers(const std::vector<uint32_t>& indices)
	{
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;
		if (!hasIndexBuffer) {
			return;
		}

		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
		uint32_t indexSize = sizeof(indices[0]);

		VulkanBuffer stagingBuffer{
			vulkanDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());

		indexBuffer = std::make_unique<VulkanBuffer>(
			vulkanDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		vulkanDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
	}

	std::vector<VkVertexInputBindingDescription> VulkanModel::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}
	std::vector<VkVertexInputAttributeDescription> VulkanModel::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

		return attributeDescriptions;
	}

	void VulkanModel::Builder::loadModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};
				}

				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

	std::vector<VulkanModel::Vertex> VulkanModel::Builder::graphModel(
		uint8_t eqn_num, glm::vec3 coefficients, 
		int lower_x, int upper_x, int lower_y, int upper_y, float interval_density, glm::vec3 color)
	{
		vertices.clear();
		indices.clear();

		// we assume it looks like rectangle when projected onto a plane.

		/* total number of vertices to be rendered for each axis */
		int num_x = upper_x - lower_x;
		int num_y = upper_y - lower_y;

		for (int x = lower_x; x < upper_x; x++) {
			for (int y = lower_y; y < upper_y; y++) {
				Vertex vertex{};

				switch (eqn_num) {
				case 0:
					Equations::plane(vertex, coefficients, x, y, interval_density);
					break;
				case 1: 
					Equations::equation_1(vertex, x, y, interval_density, 2.f, 0.25f);
					break;
				}
				
				vertex.color = color;
				vertex.uv = { 0.0, 0.0 };

				vertices.push_back(vertex);
			}
		}

		for (int y = 0; y < num_y - 1; y++) {
			for (int x = 0; x < num_x - 1; x++) {
				int index = (num_x)*y + x;
				// We render one quad each iteration, so two triangles
				/*  Triangle 1: (index, index + 1 + num_x, index + num_x),
					Triangle 2: (index + 1 + num_x, index, index + 1) */
				/*	  4 --- 5			(index + num_x) --- (index + 1 + num_x)
					  |   / |	 so,	|				/   |
					  | /   |			|		/			|
					  0 --- 1			(index)			--- (index + 1)			*/
				indices.push_back(index);
				indices.push_back(index + 1 + num_x);
				indices.push_back(index + num_x);
				indices.push_back(index + 1 + num_x);
				indices.push_back(index);
				indices.push_back(index + 1);
			}
		}

		// the number of triangles would be (2*(num_of_vertices_x - 1)*(num_of_vertices_y - 1))
		/*int num_triangles = 2 * (num_x - 1) * (num_y - 1);
		std::cout << "Triangles Count: " << num_triangles << "\n";*/

		return vertices;
	}

	void VulkanModel::Builder::visualizeNormal(std::vector<Vertex> model_vertices)
	{
		vertices.clear();
		indices.clear();
		int index = 0;
		// Loop through every vertex
		for (auto& model_vert : model_vertices) {
			Vertex vertex{};
			/* To draw a line that represents the normal, 
				we need a starting point and the end point.
				By default, 
				Vertex.normal's starting point = (0, 0, 0) 
				Vertex.normal's end point = Vertex.normal.
				
				We need to add both points by Vertex.position and draw a line,
				so our normal line sits right on top of the vertex that it wants to represent. */
			/* We will push in Starting Point first, then End Point */
			vertex.position = model_vert.position;
			vertices.push_back(vertex);
			vertex.position = (model_vert.position + 0.2f * normalize(model_vert.normal));
			vertex.color = { 1.0, 1.0, 1.0 };
			vertices.push_back(vertex);
			/* Every pair of vertices (starting and end point), needs an index */
			indices.push_back(index++);
			indices.push_back(index++);
		}

	}

}