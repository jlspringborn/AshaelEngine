/**
 * Helper functions for loading in 3D models
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "Model/Model.h"
#include "Graphics/Vulkan/Buffer.h"
#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/LogicalDevice.h"
#include "Vulkan/Vertex.hpp"

// TODO: This file is throwing a API REDEFINITION warning. It's related to glfw.h and windows.h
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tiny_gltf.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <vector>
#include <string>
#include <stdexcept>
#include <unordered_map>

namespace ash
{
	void loadTextures(tinygltf::Model& input, Model& model)
	{
		model.getTextures().resize(input.textures.size());
		for (size_t i = 0; i < input.textures.size(); i++) {
			model.getTextures()[i].imageIndex = input.textures[i].source;
		}
	}

	void loadMaterials(tinygltf::Model& input, Model& model)
	{
		model.getMaterials().resize(input.materials.size());
		for (size_t i = 0; i < input.materials.size(); i++) {
			// We only read the most basic properties required for our sample
			tinygltf::Material glTFMaterial = input.materials[i];
			// Get the base color factor
			if (glTFMaterial.values.find("baseColorFactor") != glTFMaterial.values.end()) {
				model.getMaterials()[i].baseColorFactor = glm::make_vec4(glTFMaterial.values["baseColorFactor"].ColorFactor().data());
			}
			// Get base color texture index
			if (glTFMaterial.values.find("baseColorTexture") != glTFMaterial.values.end()) {
				model.getMaterials()[i].baseColorTextureIndex = glTFMaterial.values["baseColorTexture"].TextureIndex();
			}
		}
	}

	void loadImages(tinygltf::Model& input, Model& model, const LogicalDevice* logicalDevice, const PhysicalDevice* physicalDevice)
	{
		// Images can be stored inside the glTF (which is the case for the sample model), so instead of directly
		// loading them from disk, we fetch them from the glTF loader and upload the buffers
		model.getTextureImages().resize(input.images.size());
		for (size_t i = 0; i < input.images.size(); i++) {
			tinygltf::Image& glTFImage = input.images[i];
			// Get the image data from the glTF loader
			unsigned char* buffer = nullptr;
			VkDeviceSize bufferSize = 0;
			bool deleteBuffer = false;
			// We convert RGB-only images to RGBA, as most devices don't support RGB-formats in Vulkan
			if (glTFImage.component == 3) {
				bufferSize = glTFImage.width * glTFImage.height * 4;
				buffer = new unsigned char[bufferSize];
				unsigned char* rgba = buffer;
				unsigned char* rgb = &glTFImage.image[0];
				for (size_t i = 0; i < glTFImage.width * glTFImage.height; ++i) {
					memcpy(rgba, rgb, sizeof(unsigned char) * 3);
					rgba += 4;
					rgb += 3;
				}
				deleteBuffer = true;
			}
			else {
				buffer = &glTFImage.image[0];
				bufferSize = glTFImage.image.size();
			}

			std::unique_ptr<Buffer> stagingBuffer{ std::make_unique<Buffer>(
			logicalDevice,
			physicalDevice,
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) };

			void* data;
			vkMapMemory(*logicalDevice, stagingBuffer->getBufferMemory(), 0, bufferSize, 0, &data);
			memcpy(data, buffer, static_cast<size_t>(bufferSize));
			vkUnmapMemory(*logicalDevice, stagingBuffer->getBufferMemory());

			model.getTextureImages()[i].texture = std::make_unique<Image>(
				logicalDevice,
				physicalDevice,
				glTFImage.width,
				glTFImage.height,
				VK_FORMAT_R8G8B8A8_SRGB, 
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				VK_IMAGE_ASPECT_COLOR_BIT);

			model.getTextureImages()[i].texture->transitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			model.getTextureImages()[i].texture->copyFromBuffer(*stagingBuffer, static_cast<uint32_t>(glTFImage.width), static_cast<uint32_t>(glTFImage.height));
			model.getTextureImages()[i].texture->transitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			
			if (deleteBuffer) 
			{
				delete[] buffer;
			}
		}
	}

	void loadNode(const tinygltf::Node& inputNode, 
		const tinygltf::Model& input, 
		Node* parent, 
		std::vector<uint32_t>& outIndices, 
		std::vector<Vertex>& outVertices, 
		std::vector<Node>& nodes)
	{
		Node node{};
		node.matrix = glm::mat4(1.0f);

		// Get the local node matrix
		// It's either made up from translation, rotation, scale or a 4x4 matrix
		if (inputNode.translation.size() == 3) {
			node.matrix = glm::translate(node.matrix, glm::vec3(glm::make_vec3(inputNode.translation.data())));
		}
		if (inputNode.rotation.size() == 4) {
			glm::quat q = glm::make_quat(inputNode.rotation.data());
			node.matrix *= glm::mat4(q);
		}
		if (inputNode.scale.size() == 3) {
			node.matrix = glm::scale(node.matrix, glm::vec3(glm::make_vec3(inputNode.scale.data())));
		}
		if (inputNode.matrix.size() == 16) {
			node.matrix = glm::make_mat4x4(inputNode.matrix.data());
		};

		// Load node's children
		if (inputNode.children.size() > 0) {
			for (size_t i = 0; i < inputNode.children.size(); i++) {
				loadNode(input.nodes[inputNode.children[i]], input, &node, outIndices, outVertices, nodes);
			}
		}

		// If the node contains mesh data, we load vertices and indices from the buffers
		// In glTF this is done via accessors and buffer views
		if (inputNode.mesh > -1) {
			const tinygltf::Mesh mesh = input.meshes[inputNode.mesh];
			// Iterate through all primitives of this node's mesh
			for (size_t i = 0; i < mesh.primitives.size(); i++) {
				const tinygltf::Primitive& glTFPrimitive = mesh.primitives[i];
				uint32_t firstIndex = static_cast<uint32_t>(outIndices.size());
				uint32_t vertexStart = static_cast<uint32_t>(outVertices.size());
				uint32_t indexCount = 0;
				// Vertices
				{
					const float* positionBuffer = nullptr;
					const float* normalsBuffer = nullptr;
					const float* texCoordsBuffer = nullptr;
					size_t vertexCount = 0;

					// Get buffer data for vertex normals
					if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end()) {
						const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
						const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
						positionBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
						vertexCount = accessor.count;
					}
					// Get buffer data for vertex normals
					if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end()) {
						const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
						const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
						normalsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
					}
					// Get buffer data for vertex texture coordinates
					// glTF supports multiple sets, we only load the first one
					if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end()) {
						const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
						const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
						texCoordsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
					}

					// Append data to model's vertex buffer
					for (size_t v = 0; v < vertexCount; v++) {
						Vertex vert{};
						vert.pos = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
						vert.normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
						vert.uv = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.0f);
						vert.color = glm::vec3(1.0f);
						outVertices.push_back(vert);
					}
				}
				// Indices
				{
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.indices];
					const tinygltf::BufferView& bufferView = input.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = input.buffers[bufferView.buffer];

					indexCount += static_cast<uint32_t>(accessor.count);

					// glTF supports different component types of indices
					switch (accessor.componentType) {
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
						const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++) {
							outIndices.push_back(buf[index] + vertexStart);
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
						const uint16_t* buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++) {
							outIndices.push_back(buf[index] + vertexStart);
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
						const uint8_t* buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++) {
							outIndices.push_back(buf[index] + vertexStart);
						}
						break;
					}
					default:
						std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
						return;
					}
				}
				Primitive primitive{};
				primitive.firstIndex = firstIndex;
				primitive.indexCount = indexCount;
				primitive.materialIndex = glTFPrimitive.material;
				node.mesh.primitives.push_back(primitive);
			}
		}

		if (parent) {
			parent->children.push_back(node);
		}
		else {
			nodes.push_back(node);
		}
	}

	void loadglTFFile(std::string filename, Model& model, const LogicalDevice* logicalDevice, const PhysicalDevice* physicalDevice)
	{
		tinygltf::Model glTFInput;
		tinygltf::TinyGLTF gltfContext;
		std::string error, warning;

		bool fileLoaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, filename);

		if (fileLoaded) {
			loadImages(glTFInput, model, logicalDevice, physicalDevice);
			loadMaterials(glTFInput, model);
			loadTextures(glTFInput, model);
			const tinygltf::Scene& scene = glTFInput.scenes[0];
			for (size_t i = 0; i < scene.nodes.size(); i++) {
				const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
				loadNode(node, glTFInput, nullptr, model.getIndices(), model.getVertices(), model.getNodes());
			}
		}
		else {
			throw std::runtime_error("Could not open the glTF file.\n\nThe file is part of the additional asset pack.\n\nRun \"download_assets.py\" in the repository root to download the latest version.");
			return;
		}
	}


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

				vertex.uv =
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