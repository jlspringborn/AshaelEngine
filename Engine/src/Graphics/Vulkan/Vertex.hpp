/**
* Contains vertex data used by 3D models
*
* Copyright (C) 2022, Jesse Springborn
*/
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <vulkan/vulkan.h>

#include <array>

namespace ash
{
	struct Vertex
	{
		glm::vec3 pos;		// Position
		glm::vec3 normal;
		glm::vec2 uv;		// Texture coordinate
		glm::vec3 color;	// Vertex color
		glm::vec4 jointIndices;
		glm::vec4 jointWeights;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding		= 0;
			bindingDescription.stride		= sizeof(Vertex);
			bindingDescription.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 6> getAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 6> attributeDescriptions{};

			// position
			attributeDescriptions[0].binding	= 0;
			attributeDescriptions[0].location	= 0;
			attributeDescriptions[0].format		= VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset		= offsetof(Vertex, pos);

			// normal
			attributeDescriptions[1].binding	= 0;
			attributeDescriptions[1].location	= 1;
			attributeDescriptions[1].format		= VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset		= offsetof(Vertex, normal);

			// uv
			attributeDescriptions[2].binding	= 0;
			attributeDescriptions[2].location	= 2;
			attributeDescriptions[2].format		= VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset		= offsetof(Vertex, uv);

			// color
			attributeDescriptions[3].binding	= 0;
			attributeDescriptions[3].location	= 3;
			attributeDescriptions[3].format		= VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[3].offset		= offsetof(Vertex, color);

			// joint indices
			attributeDescriptions[4].binding = 0;
			attributeDescriptions[4].location = 4;
			attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[4].offset = offsetof(Vertex, jointIndices);


			// joint weights
			attributeDescriptions[5].binding = 0;
			attributeDescriptions[5].location = 5;
			attributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[5].offset = offsetof(Vertex, jointWeights);

			return attributeDescriptions;
		}

		///**
		// * Override == operator for easy comparing to other vertices
		// */
		//bool operator==(const Vertex& other) const
		//{
		//	return pos == other.pos 
		//		&& normal == other.normal  
		//		&& uv== other.uv 
		//		&& color == other.color
		//		&& jointIndices == other.jointIndices
		//		&& jointWeights == other.jointWeights;
		//}
	};
}

namespace std 
{
	/*template<> struct hash<ash::Vertex>
	{
		size_t operator()(ash::Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.pos)
					^ (hash<glm::vec3>()(vertex.normal)
						^ (hash<glm::vec3>()(vertex.jointWeights)
						^ (hash<glm::vec3>()(vertex.jointIndices)
						^ (hash<glm::vec2>()(vertex.uv) << 1)
						^ (hash<glm::vec3>()(vertex.color) << 1))


				>> 1))));
		}
	};*/
}