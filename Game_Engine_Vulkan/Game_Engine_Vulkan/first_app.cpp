#include "first_app.h"
#include "VulkanCamera.h"
#include "VulkanBuffer.h"
#include "SimpleRenderSystem.h"
#include "PlayerSystem.h"
#include "WireframeSystem.h"
#include "PointLightSystem.h"
#include "Equations.h"
#include "print_utility.h"

#define _USE_MATH_DEFINES
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <stdexcept>
#include <chrono>
#include <array>
#include <math.h>

namespace VulkanEngine {
	FirstApp::FirstApp() {
		globalPool =
			VulkanDescriptorPool::Builder(vulkanDevice)
			.setMaxSets(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}
	FirstApp::~FirstApp()
	{
	}
	void FirstApp::run()
	{
		std::vector<std::unique_ptr<VulkanBuffer>> uboBuffers(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<VulkanBuffer>(
				vulkanDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			uboBuffers[i]->map();
		}

		auto globalSetLayout =
			VulkanDescriptorSetLayout::Builder(vulkanDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			VulkanDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{
			  vulkanDevice,
			  vulkanRenderer.getSwapChainRenderPass(),
			  globalSetLayout->getDescriptorSetLayout() 
		};
		PlayerSystem playerSystem{
			  vulkanDevice,
			  vulkanRenderer.getSwapChainRenderPass(),
			  globalSetLayout->getDescriptorSetLayout()
		};
		WireframeSystem wireframeSystem{
			  vulkanDevice,
			  vulkanRenderer.getSwapChainRenderPass(),
			  globalSetLayout->getDescriptorSetLayout()
		};
		PointLightSystem pointLightSystem{
			vulkanDevice,
			vulkanRenderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout() 
		};
		KeyCommand keyCommand{};
		VulkanCamera camera{};
		 //camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
		// camera.setViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f, 0.f, 2.5f));

		auto viewerObject = VulkanGameObject::createGameObject(); // has no model, but will store camera's current state.
		KeyboardMovementController gameController{};

		glfwSetWindowUserPointer(vulkanWindow.getGLFWwindow(), &keyCommand);
		glfwSetKeyCallback(vulkanWindow.getGLFWwindow(), key_callback);
		
		auto currentTime = std::chrono::high_resolution_clock::now();
		
		// Initial Camera transformations
		viewerObject.transform.translation = glm::vec3{ 0.f, 0.f, -16.f };
		while (!vulkanWindow.shouldClose()) {
			glfwPollEvents();
			// delta time
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			float aspect = vulkanRenderer.getAspectRatio();

			// < Controller >
			glm::vec3 inputDir = gameController.getInputDirection(vulkanWindow.getGLFWwindow(), frameTime, viewerObject);
			if (keyCommand.free_camera_mode == true) {
				// < Controller.Camera Controller >
				// Camera's Transformation
				gameController.moveInPlaneXZ(inputDir, frameTime, viewerObject);
				gameController.physics(frameTime, viewerObject);
				// update View Matrix
				
				camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation); // Free Camera

				//camera.setViewTarget(viewerObject.transform.translation, glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
				//camera.setOrthographicProjection(-10.f, 10.f, -10.f / aspect, 10.f / aspect, 0.0f, 100.f);
				camera.setPerspectiveProjection(0.67f, aspect, 0.1f, 10000.f);
			}
			else {
				// < Controller.Player Controller >
				TransformComponent gp_trans = gamePlayer.transform;
				TransformComponent vo_trans = viewerObject.transform;
				// Player's Transformation
				gameController.moveInPlaneXZ(inputDir, frameTime, gamePlayer);
				gameController.physics(frameTime, gamePlayer);
				gamePlayer.transform.rotation = vo_trans.rotation;

				gamePlayer.transform.translation.y = Equations::equation_1_y(gp_trans.translation.z, gp_trans.translation.x, .4f, 0.25f, glm::vec3(0,0,0));
				// Camera's Position
				viewerObject.transform.translation = gp_trans.translation +
					glm::vec3(10 * std::cos(vo_trans.rotation.y + M_PI_2), 5.0 * std::sin(vo_trans.rotation.x), -10 * std::sin(vo_trans.rotation.y + M_PI_2));
				// update View Matrix
				camera.setViewTarget(viewerObject.transform.translation, gp_trans.translation, glm::vec3(0.f, -1.f, 0.f));
				
				camera.setPerspectiveProjection(0.67f, aspect, 0.1f, 10000.f);
			}

			if (auto commandBuffer = vulkanRenderer.beginFrame()) {
				int frameIndex = vulkanRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameLightObjects,
					gameMeshObjects,
					gamePlayer
				};
				// update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				ubo.t = (float) glfwGetTime();
				//std::cout << "Time: " << (float) glfwGetTime() << "\n";
				pointLightSystem.update(frameInfo, ubo);
				playerSystem.update(frameInfo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();
				//std::cout << "Size of frameinfo: " << sizeof(FrameInfo) << "\n";
				// render
				vulkanRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo);
				playerSystem.render(frameInfo);
				//wireframeSystem.render(frameInfo);
				pointLightSystem.render(frameInfo);
				vulkanRenderer.endSwapChainRenderPass(commandBuffer);
				vulkanRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(vulkanDevice.device());
	}
	
	void FirstApp::loadGameObjects()
	{
		std::vector<VulkanModel::Vertex> retriever_of_vertices;
		std::shared_ptr<VulkanModel> vulkanModel;
		std::shared_ptr<VulkanModel> normalModel;

		vulkanModel = VulkanModel::createModelFromEquation(vulkanDevice,
			0, glm::vec3(0, 0, 1), -5, 5, -5, 5, 2, glm::vec3(0.6f, 0.25f, 0.25f), retriever_of_vertices);
		auto gameObj = VulkanGameObject::createGameObject();
		gameObj.model = vulkanModel;
		gameObj.transform.translation = { 0, .0f, 0.f };
		gameObj.transform.rotation = glm::vec3(0.f, 0.f, 0.0f);
		gameMeshObjects.emplace(gameObj.getId(), std::move(gameObj));
		
		/*normalModel = VulkanModel::createNormalForModel(vulkanDevice, retriever_of_vertices);
		gameObj = VulkanGameObject::createGameObject();
		gameObj.model = normalModel;
		gameObj.transform.translation = { 0.f, .0f, 0.f };
		gameObj.transform.rotation = glm::vec3(0.f, 0.f, 0.0f);
		gameWireframeObjects.emplace(gameObj.getId(), std::move(gameObj));*/

		//
		vulkanModel = VulkanModel::createModelFromEquation(vulkanDevice,
			0, glm::vec3(0, 0, 1), -5, 5, -5, 5, 2, glm::vec3(0.25f, 0.6f, 0.25f), retriever_of_vertices);
		gameObj = VulkanGameObject::createGameObject();
		gameObj.model = vulkanModel;
		gameObj.transform.translation = { 0.f, .0f, 0.f };
		gameObj.transform.rotation = glm::vec3( 0.f, 0.f, M_PI_2);
		gameMeshObjects.emplace(gameObj.getId(), std::move(gameObj));

		/*normalModel = VulkanModel::createNormalForModel(vulkanDevice, retriever_of_vertices);
		gameObj = VulkanGameObject::createGameObject();
		gameObj.model = normalModel;
		gameObj.transform.translation = { 0.f, .0f, 0.f };
		gameObj.transform.rotation = glm::vec3( M_PI_2, 0.f, 0.0f );
		gameWireframeObjects.emplace(gameObj.getId(), std::move(gameObj));*/

		//
		vulkanModel = VulkanModel::createModelFromEquation(vulkanDevice,
			0, glm::vec3(0, 0, 1.f), -5, 5, -5, 5, 2, glm::vec3(0.25f, 0.25f, 0.6f), retriever_of_vertices);
		gameObj = VulkanGameObject::createGameObject();
		gameObj.model = vulkanModel;
		gameObj.transform.translation = { 0.f, .0f, 0.f };
		gameObj.transform.rotation = glm::vec3(M_PI_2, 0.f, 0.0f);
		gameMeshObjects.emplace(gameObj.getId(), std::move(gameObj));

		/*normalModel = VulkanModel::createNormalForModel(vulkanDevice, retriever_of_vertices);
		gameObj = VulkanGameObject::createGameObject();
		gameObj.model = normalModel;
		gameObj.transform.translation = { 0.f, .0f, 0.f };
		gameObj.transform.rotation = glm::vec3(0.f, 0.f, 0.0f);
		gameObj.transform.scale = glm::vec3(0.5f, 0.5f, 0.5f);
		gameWireframeObjects.emplace(gameObj.getId(), std::move(gameObj));*/
		
		// Player cube
		vulkanModel = VulkanModel::createModelFromFile(vulkanDevice, "assets/colored_cube.obj");
		gamePlayer.model = vulkanModel;
		gamePlayer.transform.translation = { 0.0f, 0.0f, 0.f };
		gamePlayer.transform.scale = glm::vec3(3.f);


		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f},
			{.1f, 1.f, 0.5f},
		};

		for (int i = 0; i < lightColors.size(); i++) {
			auto pointLight = VulkanGameObject::makePointLight(2.9f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.f), // 4x4 identity matrix
				(i * glm::two_pi<float>()) / lightColors.size(), // starting angle
				{ 0.f, -1.f, 0.f } // axis of rotation - up vector
			);
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-3.f, -1.f, -3.f, 1.f));
			gameLightObjects.emplace(pointLight.getId(), std::move(pointLight));
		}
	}
	void FirstApp::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS) {
			switch (key) {
			case GLFW_KEY_O: {
				void *data = glfwGetWindowUserPointer(window);
				KeyCommand* key = static_cast<KeyCommand*>(data);

				if (key->free_camera_mode == false) {
					key->free_camera_mode = true;
				}
				else {
					key->free_camera_mode = false;
				}
				break;
			}
			}
		}
	}
}