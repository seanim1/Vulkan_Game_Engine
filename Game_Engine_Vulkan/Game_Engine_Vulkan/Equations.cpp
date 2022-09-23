#include "Equations.h"
#include "print_utility.h"

#include <iostream>

namespace VulkanEngine {
	namespace Equations {
		void plane(VulkanModel::Vertex& vertex, glm::vec3 coefficients, int x_param, int y_param, float interval_density)
		{
			float x = x_param / interval_density;
			float y = y_param / interval_density;
			glm::vec3 temp = { x, y, (- coefficients.x * x - coefficients.y * y) / coefficients.z };
			vertex.position = { temp.y, -temp.z, -temp.x };

			temp = { coefficients.x / coefficients.z, coefficients.y / coefficients.z, 1.0f };
			vertex.normal = { temp.y, -temp.z, -temp.x };
		}
		float plane_y(glm::vec3 coefficients, float x_param, float y_param, float interval_size, glm::vec3 displacement)
		{
			return 0.0f;
		}
		void equation_1(
			VulkanModel::Vertex& vertex, int x_param, int y_param, float interval_density, float frequency, float amplitude)
		{
			float x = x_param / interval_density;
			float y = y_param / interval_density;
			/*  Equation : f(x,y) = amp * (freq * cos(x) + freq * cos(y)) = 0
					Parametric Equation : { x, y, amp * ( freq * cos(x) + freq * cos(y) ) } */
			glm::vec3 temp = { x, y, amplitude * (std::cos(frequency * x) + std::cos(frequency * y)) };
			/* in Vulkan, X -> -Z, Y -> X, Z -> -Y. */
			vertex.position = { temp.y, -temp.z, -temp.x };
			/* Direction vector perpendicular to the surface (Calculus 3) ->
				* First, convert our f(x,y) into 4D function of f(x,y,z), which is,
				* In a such case: f(x,y) = (cos(x) + cos(y)) = 0
				* f(x,y) = z -> f(x,y) - z = 0, Our level surface is 0. So, Gradient[f(x,y) - z] is perpendicular to our graph.
					Gradient[f(x,y) - z] = Gradient[(cos(x) + cos(y) - z] = < -sin(x), -sin(y), -1 > */
			temp = { amplitude * frequency * std::sin(frequency * x),
						amplitude * frequency * std::sin(frequency * y), 1.0f };
			vertex.normal = { temp.y, -temp.z, -temp.x };
		}
		float equation_1_y(
			float x_param, float y_param, float frequency, float amplitude, glm::vec3 displacement) {
			float x = x_param;
			float y = y_param;
			return -( amplitude * (std::cos(frequency * x) + std::cos(frequency * y)) );
		}
	}
}
