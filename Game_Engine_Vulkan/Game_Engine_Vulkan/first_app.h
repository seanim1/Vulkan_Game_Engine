#pragma once

#include "keyboard_movement_controller.h"

#include "VulkanDevice.h"
#include "VulkanWindow.h"
#include "VulkanGameObject.h"
#include "VulkanRenderer.h"
#include "VulkanBuffer.h"
#include "VulkanDescriptors.h"

// std
#include <memory>
#include <vector>
namespace VulkanEngine {
	class FirstApp {
	public:
		struct KeyCommand {
			bool free_camera_mode = true;
		};

		static constexpr int WIDTH = 1280;
		static constexpr int HEIGHT = 720;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete; // deleting copy constructors
		FirstApp& operator=(const FirstApp&) = delete;

		void run();
	private:

		// Simple Render System - Anything that acts upon a subset of a game object components is a system
		void loadGameObjects();
		VulkanWindow vulkanWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		VulkanDevice vulkanDevice{ vulkanWindow };
		VulkanRenderer vulkanRenderer{ vulkanWindow, vulkanDevice };
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		//static bool free_camera_mode = false;
		// note: order of declarations matters
		std::unique_ptr<VulkanDescriptorPool> globalPool{};
		VulkanGameObject::Map gameMeshObjects;
		VulkanGameObject::Map gameWireframeObjects;
		VulkanGameObject::Map gameLightObjects;
		VulkanGameObject gamePlayer = VulkanGameObject::createGameObject();

	};
}
