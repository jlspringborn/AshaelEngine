/**
 * Drawable 3D object
 *
 * Copyright (C) 2021, Jesse Springborn
 */
#pragma once

#include "Vulkan/LogicalDevice.h"
#include "Vulkan/Vertex.hpp"
#include "Vulkan/Buffer.h"
#include "Vulkan/Image.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace ash
{

	struct Node;

	/**
	 * A Primitive contains data for a single draw call
	 */
	struct Primitive
	{
		uint32_t firstIndex;
		uint32_t indexCount;
		int32_t materialIndex;
	};

	/**
	 * Contains the node's (optional) geometry and can be made up of an arbitrary number of primitives
	 */
	struct Mesh
	{
		std::vector<Primitive> primitives;
	};

	/**
	 * A node represents an object in the gltf scene graph
	 */
	struct Node
	{
		Node* parent;
		std::vector<Node> children;
		Mesh mesh;
		glm::mat4 matrix;
	};

	/**
	 * A gltf material stores information in e.g. the texture that is attached to it and colors
	 */
	struct Material
	{
		glm::vec4 baseColorFactor = glm::vec4(1.0f);
		uint32_t baseColorTextureIndex;
	};

	/**
	 * Contains the texture for a single gltf image
	 * TextureImages may be reused by texture objects and are as such separated
	 */
	struct TextureImage
	{
		std::unique_ptr<Image> texture;
		// We also store (and create) a descriptor set that's used to access this texture from the fragment shader
		VkDescriptorSet descriptorSet;
	};

	/**
	 * A gltf texture stores a reference to the image and a sampler
	 * In this sample, we are only interested in the image
	 */
	struct Texture
	{
		int32_t imageIndex;
	};
	class GLTFModel
	{
	public:
		GLTFModel();
		~GLTFModel();

	private:

		/**
		 * Vulkan Logical Device, used for resource destruction
		 */
		const LogicalDevice* m_logicalDevice{};

		/**
		 * single vertex buffer for all primitives
		 */
		std::unique_ptr<Buffer> vertices{};

		/**
		 * Single index buffer for all primitives
		 */
		std::unique_ptr<Buffer> indices{};

		int indexCount{};

		std::vector<TextureImage> m_images;
		std::vector<Texture> m_textures;
		std::vector<Material> m_materials;
		std::vector<Node> m_nodes;

	};
}