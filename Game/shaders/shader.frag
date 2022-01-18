// FRAGEMENT SHADER
// Copyright (C) 2021, Jesse Springborn
#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push
{
	mat4 modelMatrix;
} push;

void main()
{
	outColor = texture(texSampler, fragTexCoord);
}