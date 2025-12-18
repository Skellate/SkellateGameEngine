#include "keyboard_movement_controller.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <limits>

namespace lve {

void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, LveGameObject& gameObject) {
    // Handle escape key to exit game
    if (glfwGetKey(window, keys.exitGame) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }
    
    // Mouse look
    if (enableMouseLook) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        
        if (firstMouse) {
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            firstMouse = false;
        }
        
        double deltaX = mouseX - lastMouseX;
        double deltaY = mouseY - lastMouseY;
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        
        gameObject.transform.rotation.y += static_cast<float>(deltaX) * mouseSensitivity;
        gameObject.transform.rotation.x -= static_cast<float>(deltaY) * mouseSensitivity;
        
        // Hide cursor and keep it centered (for FPS-style controls)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    // Limit pitch values between about +/- 85ish degrees
    gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
    gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

    float yaw = gameObject.transform.rotation.y;
    const glm::vec3 forwardDir{sin(yaw), 0.0f, cos(yaw)};
    const glm::vec3 rightDir{forwardDir.z, 0.0f, -forwardDir.x};
    const glm::vec3 upDir{0.0f, -1.0f, 0.0f};

    // Horizontal movement
    glm::vec3 moveDir{0.0f};
    if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
    if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;

    // Apply horizontal movement
    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
        gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
    }
    
    // Gravity and jumping physics
    bool wasOnGround = isOnGround;
    
    // Simple ground collision detection
    if (gameObject.transform.translation.y >= groundLevel) {
        isOnGround = true;
        gameObject.transform.translation.y = groundLevel;
        verticalVelocity = 0.0f;
    } else {
        isOnGround = false;
    }
    
    // Jump if on ground and space is pressed
    static bool jumpKeyWasPressed = false;
    bool jumpKeyPressed = glfwGetKey(window, keys.jump) == GLFW_PRESS;
    
    if (jumpKeyPressed && !jumpKeyWasPressed && isOnGround) {
        verticalVelocity = jumpSpeed;
        isOnGround = false;
    }
    jumpKeyWasPressed = jumpKeyPressed;
    
    // Apply gravity
    if (!isOnGround) {
        verticalVelocity += gravity * dt;
    }
    
    // Apply vertical movement
    gameObject.transform.translation.y -= verticalVelocity * dt;
    
    // Manual up/down movement (for debugging/flying)
    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) {
        gameObject.transform.translation += upDir * moveSpeed * dt;
        verticalVelocity = 0.0f; // Cancel gravity when manually moving up
    }
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) {
        gameObject.transform.translation -= upDir * moveSpeed * dt;
        verticalVelocity = 0.0f; // Cancel gravity when manually moving down
    }
}

bool KeyboardMovementController::shouldShoot(GLFWwindow* window) {
    bool mouseButtonPressed = glfwGetMouseButton(window, keys.shoot) == GLFW_PRESS;
    bool shouldShootNow = mouseButtonPressed && !mouseButtonWasPressed;
    mouseButtonWasPressed = mouseButtonPressed;
    return shouldShootNow;
}

glm::vec3 KeyboardMovementController::getShootDirection(const LveGameObject& gameObject) {
    float yaw = gameObject.transform.rotation.y;
    float pitch = gameObject.transform.rotation.x;
    
    // Calculate direction vector from camera rotation
    glm::vec3 direction;
    direction.x = cos(pitch) * sin(yaw);
    direction.y = -sin(pitch);
    direction.z = cos(pitch) * cos(yaw);
    
    return glm::normalize(direction);
}

}  // namespace lve
