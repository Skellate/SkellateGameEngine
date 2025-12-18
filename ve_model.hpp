#pragma once
 
 #include "ve_device.hpp"
 
 // libs
 #define GLM_FORCE_RADIANS
 #define GLM_FORCE_DEPTH_ZERO_TO_ONE
 #include <glm/glm.hpp>
 
 // std
 #include <vector>
 
 namespace lve {
 class LveModel {
  public:
   struct Vertex {
     glm::vec3 position;  // Changed from vec2 to vec3
     glm::vec3 color;

     static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
     static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
   };

   LveModel(LveDevice &device, const std::vector<Vertex> &vertices);
   LveModel(LveDevice &device, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
   ~LveModel();   LveModel(const LveModel &) = delete;
   LveModel &operator=(const LveModel &) = delete;
 
   void bind(VkCommandBuffer commandBuffer);
   void draw(VkCommandBuffer commandBuffer);
 
  private:
   void createVertexBuffers(const std::vector<Vertex> &vertices);
   void createIndexBuffers(const std::vector<uint32_t> &indices);

   LveDevice &lveDevice;
   
   VkBuffer vertexBuffer;
   VkDeviceMemory vertexBufferMemory;
   uint32_t vertexCount;
   
   bool hasIndexBuffer = false;
   VkBuffer indexBuffer;
   VkDeviceMemory indexBufferMemory;
   uint32_t indexCount;
 };
 }  // namespace lve