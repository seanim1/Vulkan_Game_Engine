#pragma once

#include <iostream>
#include <glm/glm.hpp>
namespace VulkanEngine {
	static void printVec3(glm::vec3 vec) {
		std::cout << "<" << vec.x << ", " << vec.y << ", " << vec.z << ">" << "\n";
	}
}