#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;

struct PointLight {
  vec3 position; // ignore w
  vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 invView;
  vec4 ambientLightColor; // w is intensity
  PointLight pointLights[10];
  int numLights;
  float t;
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
	vec3 color;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));
const float AMBIENT = 0.1;

void main() {
	mat4 shear = mat4(
					1, 0, 0, 0,
					0.2, 1, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);
	vec4 positionWorld = shear * push.modelMatrix * vec4(position, 1.0);
	gl_Position = ubo.projection * ubo.view * positionWorld;

	fragNormalWorld = mat3(push.normalMatrix) * normal;
	fragPosWorld = positionWorld.xyz;

	//gl_PointSize = 5.0;

	fragColor = color;
}