// VERTEX SHADER
// Copyright (C) 2021, Jesse Springborn
#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject
{
	mat4 model;
	mat4 view;
	mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;
layout(location = 4) in vec3 inJoints;
layout(location = 5) in vec3 inWeights;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(push_constant) uniform Push
{
	mat4 modelMatrix;
} push;

void main()
{
	gl_Position = ubo.proj * ubo.view * push.modelMatrix * vec4(inPosition, 1.0); 
	fragColor = inColor;
	fragTexCoord = inUV;
}