#pragma once

#include "ve_window.hpp"
#include "ve_game_object.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve {

class KeyboardMovementController {
 public:
  struct KeyMappings {
    int moveLeft = GLFW_KEY_A;
    int moveRight = GLFW_KEY_D;
    int moveForward = GLFW_KEY_W;
    int moveBackward = GLFW_KEY_S;
    int moveUp = GLFW_KEY_E;
    int moveDown = GLFW_KEY_Q;
    int jump = GLFW_KEY_SPACE;
    int exitGame = GLFW_KEY_ESCAPE;
    int shoot = GLFW_MOUSE_BUTTON_LEFT;
    int pauseGame = GLFW_KEY_P;
    int startGame = GLFW_KEY_ENTER;
    int lookLeft = GLFW_KEY_LEFT;
    int lookRight = GLFW_KEY_RIGHT;
    int lookUp = GLFW_KEY_UP;
    int lookDown = GLFW_KEY_DOWN;
  };

  void moveInPlaneXZ(GLFWwindow* window, float dt, LveGameObject& gameObject);
  bool shouldShoot(GLFWwindow* window);
  glm::vec3 getShootDirection(const LveGameObject& gameObject);

  KeyMappings keys{};
  float moveSpeed{3.0f};
  float lookSpeed{1.5f};
  float jumpSpeed{7.0f};
  
  // Shooting properties
  float projectileSpeed{20.0f};
  float projectileGravity{-15.0f}; // Stronger gravity for projectiles
  float bounceDamping{0.7f}; // Energy loss on bounce (0.7 = loses 30% energy)
  
  // Gravity and physics
  float gravity{-9.8f};
  float groundLevel{0.0f};
  
  // Mouse look settings
  bool enableMouseLook{true};
  float mouseSensitivity{0.002f};
  
 private:
  double lastMouseX{0.0};
  double lastMouseY{0.0};
  bool firstMouse{true};
  
  // Physics state
  float verticalVelocity{0.0f};
  bool isOnGround{false};
  
  // Shooting state
  bool mouseButtonWasPressed{false};
};

}  // namespace lve
