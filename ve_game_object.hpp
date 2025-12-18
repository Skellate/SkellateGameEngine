#pragma once

#include "ve_model.hpp"
#include "ve_transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace lve {

struct RigidBody2dComponent {
    glm::vec2 velocity;
    float mass{1.0f};
};

class LveGameObject {
 public:
  using id_t = unsigned int;
  using Map = std::unordered_map<id_t, LveGameObject>;

  static LveGameObject createGameObject() {
    static id_t currentId = 0;
    return LveGameObject{currentId++};
  }

  static LveGameObject makePointLight(
      float intensity = 10.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

  LveGameObject(const LveGameObject &) = delete;
  LveGameObject &operator=(const LveGameObject &) = delete;
  LveGameObject(LveGameObject &&) = default;
  LveGameObject &operator=(LveGameObject &&) = default;

  id_t getId() { return id; }

  glm::vec3 color{};
  TransformComponent transform{};

  // Optional pointer components
  std::shared_ptr<LveModel> model{};
  std::unique_ptr<RigidBody2dComponent> rigidBody{};

 private:
  LveGameObject(id_t objId) : id{objId} {}

  id_t id;
};

}  // namespace lve
