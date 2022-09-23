#pragma once

#include "VulkanModel.h"
// std
#include <memory>
#include <vector>
namespace VulkanEngine {
	namespace Equations {
		void plane(VulkanModel::Vertex& vertex, glm::vec3 coefficients, int x_param, int y_param, float interval_size);
		float plane_y(glm::vec3 coefficients, float x_param, float y_param, float interval_size, glm::vec3 displacement);

		void equation_1(VulkanModel::Vertex& vertex, int x_param, int y_param, float interval_size, float frequency, float amplitude);
		float equation_1_y(float x_param, float y_param, float frequency, float amplitude, glm::vec3 displacement);
	}
}
