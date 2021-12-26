#pragma once

#include "Device.hpp"
#include "Buffer.hpp"

#include <tiny_gltf.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace ash
{
	/**
	 * @brief Take vertex data read on the CPU
	 *		and allocate the memory and copy the data
	 *		over to the device GPU so that it can be rendered
	 */
	class Model
	{
	public:

		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const
			{
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		// A primitive contains the data for a single draw call
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

		// A node represents an object in the glTF scene graph
		struct Node 
		{
			Node* parent;
			std::vector<Node> children;
			Mesh mesh;
			glm::mat4 matrix;
		};

		struct Data
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

		void loadGLTFModel(std::string filename);
		void loadNode(
			const tinygltf::Node& inputNode,
			const tinygltf::Model& input,
			Node* parent,
			std::vector<uint32_t>& indices,
			std::vector<Vertex>& vertices);

		Model(Device& device, const Model::Data& data);
		~Model();

		Model(const Model&) = delete;
		Model operator=(const Model&) = delete;

		static std::unique_ptr<Model> createModelFromFile(Device& device, const std::string& filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
		void drawNode(VkCommandBuffer commandBuffer, Node node);

	private:

		void createVertexBuffers(const std::vector<Vertex> &vertices);

		void createIndexBuffer(const std::vector<uint32_t> &indices);

		Device& device;

		std::unique_ptr<Buffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;

		std::unique_ptr<Buffer> indexBuffer;
		uint32_t indexCount;

		std::vector<Node> nodes;

	};
}