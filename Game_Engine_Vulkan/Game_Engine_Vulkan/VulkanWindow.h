#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace VulkanEngine {
	class VulkanWindow {
	public:
		VulkanWindow(int width, int height, std::string name); // constructor
		~VulkanWindow(); // destructor

		VulkanWindow(const VulkanWindow&) = delete;
		VulkanWindow& operator=(const VulkanWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }
		GLFWwindow* getGLFWwindow() const { return window; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow(); // helper function for initializing window

		// private variables
		int width;
		int height;
		bool framebufferResized = false;

		std::string windowName;
		GLFWwindow* window; // variable that is a pointer to GLFW window for glfwCreateWindow function

	};
}