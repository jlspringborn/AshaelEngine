/**
 * Helper functions for loading in 3D models
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/LogicalDevice.h"
#include "Vulkan/Vertex.hpp"
#include "GLTFModel.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

//#define TINYGLTF_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#define TINYGLTF_NO_STB_IMAGE_WRITE
//#include "tiny_gltf.h"

#include <vector>
#include <string>
#include <stdexcept>
#include <unordered_map>

namespace ash
{
	//void loadImages(tinygltf::Model& input, 
	//	std::vector<TextureImage> images, 
	//	const LogicalDevice* logicalDevice, 
	//	const PhysicalDevice* physicalDevice)
	//{
	//	// Images can be stored inside the glTF (which is the case for the sample model), so instead of directly
	//	// loading them from disk, we fetch them from the glTF loader and upload the buffers
	//	images.resize(input.images.size());
	//	for (size_t i = 0; i < input.images.size(); i++) {
	//		tinygltf::Image& glTFImage = input.images[i];
	//		// Get the image data from the glTF loader
	//		unsigned char* buffer = nullptr;
	//		VkDeviceSize bufferSize = 0;
	//		bool deleteBuffer = false;
	//		// We convert RGB-only images to RGBA, as most devices don't support RGB-formats in Vulkan
	//		if (glTFImage.component == 3) {
	//			bufferSize = glTFImage.width * glTFImage.height * 4;
	//			buffer = new unsigned char[bufferSize];
	//			unsigned char* rgba = buffer;
	//			unsigned char* rgb = &glTFImage.image[0];
	//			for (size_t i = 0; i < glTFImage.width * glTFImage.height; ++i) {
	//				memcpy(rgba, rgb, sizeof(unsigned char) * 3);
	//				rgba += 4;
	//				rgb += 3;
	//			}
	//			deleteBuffer = true;
	//		}
	//		else {
	//			buffer = &glTFImage.image[0];
	//			bufferSize = glTFImage.image.size();
	//		}
	//		// Load texture from image buffer
	//		images[i].texture = std::make_unique<Image>(
	//			logicalDevice, 
	//			physicalDevice, 
	//			glTFImage.width, 
	//			glTFImage.height, 
	//			VK_FORMAT_R8G8B8A8_SRGB, 
	//			VK_IMAGE_TILING_OPTIMAL,
	//			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
	//			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	//			VK_IMAGE_ASPECT_COLOR_BIT);
	//		if (deleteBuffer) {
	//			delete[] buffer;
	//		}
	//	}
	//}


	/**
	 * Load model from obj file
	 */
	void loadModel(std::string modelPath, std::vector<Vertex>& outVertices, std::vector<uint32_t>& outIndices)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn;
		std::string err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str()))
		{
			throw std::runtime_error(warn + err);
		}

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				vertex.pos =
				{
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.texCoord =
				{
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1] // flip for correct color
				};

				vertex.color = { 1.0f, 1.0f, 1.0f };


				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(outVertices.size());
					outVertices.push_back(vertex);
				}
				outIndices.push_back(uniqueVertices[vertex]);
			}
		}
	}
}