/**
 * Drawable 3D object
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/LogicalDevice.h"
#include "Vulkan/Buffer.h"
#include "Vulkan/Image.h"
#include "Vulkan/Vertex.hpp"
#include "Vulkan/PushConstantData.hpp"
#include "TransformComponent.hpp"

#include <vulkan/vulkan.h>

#include <chrono>
#include <vector>
#include <array>
#include <string>

namespace ash
{
	struct Node;

	// A Primitive contains the data for a single draw call
	struct Primitive
	{
		uint32_t firstIndex;
		uint32_t indexCount;
		int32_t materialIndex;
	};

	// Contains the node's (optional) geometry and can be made up of an arbitrary number of primitives
	struct Mesh
	{
		std::vector<Primitive> primitives;
	};

	struct Node
	{
		Node* parent;
		std::vector<Node> children;
		Mesh mesh;
		glm::mat4 matrix;
	};

	// A glTF material stores information in e.g. the texture that is attached to it and colors
	struct Material 
	{
		glm::vec4 baseColorFactor = glm::vec4(1.0f);
		uint32_t baseColorTextureIndex;
	};

	// Contains the texture for a single glTF image
	// Images may be reused by texture objects and are as such separated
	struct TextureImage 
	{
		std::unique_ptr<Image> texture;
		// We also store (and create) a descriptor set that's used to access this texture from the fragment shader
		VkDescriptorSet descriptorSet;
	};

	// A glTF texture stores a reference to the image and a sampler
	// In this sample, we are only interested in the image
	struct Texture 
	{
		int32_t imageIndex;
	};

	/**
	 * Drawable 3D object
	 */
	class Model
	{
	public:
		Model(
			const LogicalDevice* logicalDevice, 
			const PhysicalDevice* physicalDevice,
			const int swapChainImageCount, 
			VkDescriptorSetLayout setLayout, 
			VkDescriptorPool pool,
			VkSampler sampler, 
			std::vector<std::unique_ptr<Buffer>>& uniformBuffers,
			std::string modelPath,
			std::string texturePath
		);

		~Model();

		/**
		 * Binds buffers and descriptor set, then calls draw command
		 */
		void draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index, TransformComponent* transform);

		/**
		 * Creates buffer to hold vertices
		 */
		void createVertexBuffer(const PhysicalDevice* physicalDevice);

		/**
		 * Creates buffer to hold indices of vertices
		 */
		void createIndexBuffer(const PhysicalDevice* physicalDevice);

		/**
		 * Creates texture image to display on geometry
		 */
		void createTexture(const PhysicalDevice* physicalDevice, std::string texturePath);

		/**
		 * Creates a descriptor set for each swap chain image
		 */
		void createDescriptorSets(const uint32_t swapChainImageCount, VkDescriptorSetLayout setLayout, VkDescriptorPool pool, VkSampler sampler, std::vector<std::unique_ptr<Buffer>>& uniformBuffers);

		/**
		 * Called during swap chain recreation and class destruction
		 */
		void cleanupDescriptorSets();

		std::vector<Node>& getNodes() { return nodes; }

		std::vector<Vertex>& getVertices() { return m_vertices; }

		std::vector<uint32_t>& getIndices() { return m_indices; }

		std::vector<TextureImage>& getTextureImages() { return m_textureImages; }

		std::vector<Texture>& getTextures() { return m_textures; }
		
		std::vector<Material>& getMaterials() { return m_materials; }

		// Draw a single node including child nodes (if present)
		void drawNode(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, Node node);

	private:

		/**
		 * Vulkan Logical Device, used for resource destruction
		 */
		const LogicalDevice* m_logicalDevice{};

		/**
		 * Vertex data of model
		 */
		std::vector<Vertex> m_vertices;

		/**
		 * Index of each vertex, used for more efficient rendering
		 */
		std::vector<uint32_t> m_indices;

		/**
		 * Buffer to hold vertex data
		 */
		std::unique_ptr<Buffer> m_vertexBuffer;
		
		/**
		 * Buffer to hold index of vertices
		 */
		std::unique_ptr<Buffer> m_indexBuffer;

		/**
		 * Array of Descriptor Sets, one for each Uniform Buffer
		 */
		std::vector<VkDescriptorSet> m_descriptorSets{};

		/**
		 * Texture to be displayed on geometry during fragment stage of pipeline
		 */
		std::unique_ptr<Image> m_texture{};


		std::vector<Node> nodes;

		std::vector<TextureImage> m_textureImages;

		std::vector<Material> m_materials;

		std::vector<Texture> m_textures;


	};
}
