#pragma once

#include "ve_model.hpp"
#include "ve_device.hpp"

#include <memory>
#include <vector>

namespace lve {

class GeometryBuilder {
 public:
  // Create a cube mesh
  static std::shared_ptr<LveModel> createCube(LveDevice& device, float size = 1.0f);
  
  // Create a sphere mesh
  static std::shared_ptr<LveModel> createSphere(LveDevice& device, float radius = 1.0f, int segments = 16, int rings = 12);
  
  // Create a plane mesh
  static std::shared_ptr<LveModel> createPlane(LveDevice& device, float width = 1.0f, float height = 1.0f);
  
  // Create a realistic rifle/gun model
  static std::shared_ptr<LveModel> createRifle(LveDevice& device, float scale = 1.0f);

 private:
  static glm::vec3 generateColor(float u, float v);
};

}  // namespace lve
