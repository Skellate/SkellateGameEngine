#include "simple_game.hpp"
#include "geometry_builder.hpp"

#include <stdexcept>
#include <cassert>
#include <array>
#include <chrono>
#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve {

struct SimplePushConstantData {
  glm::mat4 transform{1.0f};
  glm::vec3 color;
};

SimpleGame::SimpleGame() {
  currentTime = std::chrono::steady_clock::now();
  loadGameObjects();
  createPipelineLayout();
  recreateSwapChain();
  createCommandBuffers();
  
  // Set up initial FPS camera position
  viewerObject.transform.translation = {0.0f, -1.5f, -3.0f}; // Start above the ground
  viewerObject.transform.rotation = {0.0f, 0.0f, 0.0f}; // Look forward initially
  
  // Set up weapon (initialize after loadGameObjects so weaponModel exists)
  weaponObject.model = weaponModel;
  weaponObject.color = {0.3f, 0.3f, 0.3f}; // Dark gray weapon
  
  // Display the initial menu
  displayMenu();
}

SimpleGame::~SimpleGame() { 
  vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr); 
}

void SimpleGame::run() {
  // Show prominent menu in console
  std::cout << "========================================" << std::endl;
  std::cout << "        VULKAN FPS GAME - MENU         " << std::endl;
  std::cout << "========================================" << std::endl;
  std::cout << " Gray screen = Menu (this is normal!)  " << std::endl;
  std::cout << " Press ENTER to start the game         " << std::endl;
  std::cout << " Press ESC to exit                     " << std::endl;
  std::cout << "========================================" << std::endl;
  
  while (!lveWindow.shouldClose()) {
    glfwPollEvents();

    auto newTime = std::chrono::steady_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    currentTime = newTime;

    // Limit frame time to prevent big jumps (e.g., when debugging)
    frameTime = glm::min(frameTime, 0.25f);

    // Always set up camera projection for rendering
    float aspect = lveSwapChain->extentAspectRatio();
    camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);
    camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

    // Handle input based on game state
    switch (gameState) {
      case GameState::MENU:
        handleMenuInput();
        updateMenuVisuals(); // Update visual menu appearance
        displayMenu();
        break;
        
      case GameState::PLAYING:
        handleGameInput(frameTime);
        break;
        
      case GameState::PAUSED:
        handleMenuInput(); // Reuse menu input for pause menu
        displayPauseMenu();
        break;
    }

    // Always draw the frame
    drawFrame();
  }

  vkDeviceWaitIdle(lveDevice.device());
}

void SimpleGame::loadGameObjects() {
  std::cout << "Loading game objects..." << std::endl;
  
  // Create projectile model for shooting (smaller)
  std::cout << "Creating projectile model..." << std::endl;
  projectileModel = GeometryBuilder::createSphere(lveDevice, 6); // Smaller sphere
  std::cout << "Projectile model created!" << std::endl;
  
  // Create weapon model
  std::cout << "Creating weapon model..." << std::endl;
  weaponModel = GeometryBuilder::createCube(lveDevice, 0.5f); // Back to cube for testing
  std::cout << "Weapon model created!" << std::endl;
  
  // Create menu cube model for visual menu
  std::cout << "Creating menu cube model..." << std::endl;
  menuCubeModel = GeometryBuilder::createCube(lveDevice);
  std::cout << "Menu cube model created!" << std::endl;
  
  // Create multiple cubes for a more interesting environment
  std::cout << "Creating cube model..." << std::endl;
  auto cubeModel = GeometryBuilder::createCube(lveDevice);
  std::cout << "Cube model created successfully!" << std::endl;
  
  // Main open platform (with a hole in the middle)
  std::cout << "Creating open platform..." << std::endl;
  
  // Platform corners (4 separate platforms forming a square with opening in center)
  auto platform1 = LveGameObject::createGameObject();
  platform1.model = cubeModel;
  platform1.transform.translation = {-2.0f, 2.0f, -2.0f};
  platform1.transform.scale = {1.5f, 0.2f, 1.5f};
  platform1.color = {0.7f, 0.7f, 0.7f};
  gameObjects.emplace(platform1.getId(), std::move(platform1));
  
  auto platform2 = LveGameObject::createGameObject();
  platform2.model = cubeModel;
  platform2.transform.translation = {2.0f, 2.0f, -2.0f};
  platform2.transform.scale = {1.5f, 0.2f, 1.5f};
  platform2.color = {0.7f, 0.7f, 0.7f};
  gameObjects.emplace(platform2.getId(), std::move(platform2));
  
  auto platform3 = LveGameObject::createGameObject();
  platform3.model = cubeModel;
  platform3.transform.translation = {-2.0f, 2.0f, 2.0f};
  platform3.transform.scale = {1.5f, 0.2f, 1.5f};
  platform3.color = {0.7f, 0.7f, 0.7f};
  gameObjects.emplace(platform3.getId(), std::move(platform3));
  
  auto platform4 = LveGameObject::createGameObject();
  platform4.model = cubeModel;
  platform4.transform.translation = {2.0f, 2.0f, 2.0f};
  platform4.transform.scale = {1.5f, 0.2f, 1.5f};
  platform4.color = {0.7f, 0.7f, 0.7f};
  gameObjects.emplace(platform4.getId(), std::move(platform4));
  
  std::cout << "Open platform created!" << std::endl;
  
  // Floating target platforms
  auto floatPlatform1 = LveGameObject::createGameObject();
  floatPlatform1.model = cubeModel;
  floatPlatform1.transform.translation = {4.0f, 0.5f, 4.0f};
  floatPlatform1.transform.scale = {1.0f, 0.2f, 1.0f};
  floatPlatform1.color = {0.6f, 0.8f, 0.6f};
  gameObjects.emplace(floatPlatform1.getId(), std::move(floatPlatform1));
  
  auto floatPlatform2 = LveGameObject::createGameObject();
  floatPlatform2.model = cubeModel;
  floatPlatform2.transform.translation = {-4.0f, 1.0f, -3.0f};
  floatPlatform2.transform.scale = {1.0f, 0.2f, 1.0f};
  floatPlatform2.color = {0.8f, 0.6f, 0.6f};
  gameObjects.emplace(floatPlatform2.getId(), std::move(floatPlatform2));
  
  // Target cubes to shoot at
  auto target1 = LveGameObject::createGameObject();
  target1.model = cubeModel;
  target1.transform.translation = {3.0f, 0.0f, 1.0f};
  target1.transform.scale = {0.3f, 0.3f, 0.3f};
  target1.color = {1.0f, 0.2f, 0.2f};
  gameObjects.emplace(target1.getId(), std::move(target1));
  
  auto target2 = LveGameObject::createGameObject();
  target2.model = cubeModel;
  target2.transform.translation = {-3.0f, 0.0f, -1.0f};
  target2.transform.scale = {0.3f, 0.3f, 0.3f};
  target2.color = {0.2f, 0.2f, 1.0f};
  gameObjects.emplace(target2.getId(), std::move(target2));
  
  // High target for shooting practice
  auto highTarget = LveGameObject::createGameObject();
  highTarget.model = cubeModel;
  highTarget.transform.translation = {0.0f, -2.0f, 5.0f};
  highTarget.transform.scale = {0.4f, 0.4f, 0.4f};
  highTarget.color = {1.0f, 1.0f, 0.2f}; // Yellow target
  gameObjects.emplace(highTarget.getId(), std::move(highTarget));
  
  std::cout << "Creating floor plane..." << std::endl;
  // Ground plane
  auto floorModel = GeometryBuilder::createPlane(lveDevice);
  auto floor = LveGameObject::createGameObject();
  floor.model = floorModel;
  floor.transform.translation = {0.0f, 3.0f, 0.0f};
  floor.transform.scale = {12.0f, 1.0f, 12.0f};
  floor.transform.rotation = {glm::radians(90.0f), 0.0f, 0.0f};
  floor.color = {0.3f, 0.5f, 0.3f};
  gameObjects.emplace(floor.getId(), std::move(floor));
  
  // Create visual menu objects
  createMenuObjects();
  
  std::cout << "All game objects loaded successfully! Total objects: " << gameObjects.size() << std::endl;
}

void SimpleGame::createPipelineLayout() {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(SimplePushConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void SimpleGame::createPipeline() {
  assert(lveSwapChain != nullptr && "Cannot create pipeline before swap chain");
  assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  vePipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = lveSwapChain->getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;
  lvePipeline = std::make_unique<vePipeline>(
      lveDevice,
      "shaders/simpleShader.vert.spv",
      "shaders/simpleShader.frag.spv",
      pipelineConfig);
}

void SimpleGame::recreateSwapChain() {
  auto extent = lveWindow.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = lveWindow.getExtent();
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(lveDevice.device());

  if (lveSwapChain == nullptr) {
    lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);
  } else {
    lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent, std::move(lveSwapChain));

    if (lveSwapChain->imageCount() != commandBuffers.size()) {
      freeCommandBuffers();
      createCommandBuffers();
    }
  }

  createPipeline();
}

void SimpleGame::createCommandBuffers() {
  commandBuffers.resize(lveSwapChain->imageCount());

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = lveDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void SimpleGame::freeCommandBuffers() {
  vkFreeCommandBuffers(
      lveDevice.device(),
      lveDevice.getCommandPool(),
      static_cast<uint32_t>(commandBuffers.size()),
      commandBuffers.data());
  commandBuffers.clear();
}

void SimpleGame::drawFrame() {
  uint32_t imageIndex;
  auto result = lveSwapChain->acquireNextImage(&imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  recordCommandBuffer(imageIndex);
  result = lveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      lveWindow.wasWindowResized()) {
    lveWindow.resetWindowResizedFlag();
    recreateSwapChain();
    return;
  }

  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }
}

void SimpleGame::recordCommandBuffer(int imageIndex) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = lveSwapChain->getRenderPass();
  renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(imageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(lveSwapChain->getSwapChainExtent().width);
  viewport.height = static_cast<float>(lveSwapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{{0, 0}, lveSwapChain->getSwapChainExtent()};
  vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
  vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

  lvePipeline->bind(commandBuffers[imageIndex]);

  if (gameState == GameState::MENU) {
    // Render menu objects when in menu state
    for (auto& kv : menuObjects) {
      auto& obj = kv.second;
      if (obj.model == nullptr) continue;

      SimplePushConstantData push{};
      push.transform = camera.getProjection() * camera.getView() * obj.transform.mat4();
      push.color = obj.color;

      vkCmdPushConstants(
        commandBuffers[imageIndex],
        pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(SimplePushConstantData),
        &push);
      obj.model->bind(commandBuffers[imageIndex]);
      obj.model->draw(commandBuffers[imageIndex]);
    }
  } else if (gameState == GameState::PLAYING || gameState == GameState::PAUSED) {
    // Render game objects
    for (auto& kv : gameObjects) {
      auto& obj = kv.second;
      if (obj.model == nullptr) continue;

      SimplePushConstantData push{};
      push.transform = camera.getProjection() * camera.getView() * obj.transform.mat4();
      push.color = obj.color;

      vkCmdPushConstants(
          commandBuffers[imageIndex],
          pipelineLayout,
          VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
          0,
          sizeof(SimplePushConstantData),
          &push);
      obj.model->bind(commandBuffers[imageIndex]);
      obj.model->draw(commandBuffers[imageIndex]);
    }
    
    // Render projectiles
    for (auto& kv : projectiles) {
      auto& projectile = kv.second;
      if (projectile.model == nullptr) continue;

      SimplePushConstantData push{};
      push.transform = camera.getProjection() * camera.getView() * projectile.transform.mat4();
      push.color = projectile.color;

      vkCmdPushConstants(
          commandBuffers[imageIndex],
          pipelineLayout,
          VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
          0,
          sizeof(SimplePushConstantData),
          &push);
      projectile.model->bind(commandBuffers[imageIndex]);
      projectile.model->draw(commandBuffers[imageIndex]);
    }
    
    // Render weapon (only when playing, not when paused)
    if (gameState == GameState::PLAYING && weaponObject.model != nullptr) {
      SimplePushConstantData push{};
      push.transform = camera.getProjection() * camera.getView() * weaponObject.transform.mat4();
      push.color = weaponObject.color;

      vkCmdPushConstants(
          commandBuffers[imageIndex],
          pipelineLayout,
          VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
          0,
          sizeof(SimplePushConstantData),
          &push);
      weaponObject.model->bind(commandBuffers[imageIndex]);
      weaponObject.model->draw(commandBuffers[imageIndex]);
    }
  }

  vkCmdEndRenderPass(commandBuffers[imageIndex]);
  if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}

void SimpleGame::updateWeapon() {
  // Position weapon in front of camera
  float yaw = viewerObject.transform.rotation.y;
  float pitch = viewerObject.transform.rotation.x;
  
  // Calculate forward vector (same as camera direction)
  glm::vec3 forward;
  forward.x = cos(pitch) * sin(yaw);
  forward.y = -sin(pitch);
  forward.z = cos(pitch) * cos(yaw);
  
  // Calculate right vector from yaw
  glm::vec3 right;
  right.x = sin(yaw + glm::half_pi<float>());
  right.y = 0.0f;
  right.z = cos(yaw + glm::half_pi<float>());
  
  glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f); // World up
  
  // Position weapon slightly forward, down, and to the right of camera
  weaponObject.transform.translation = viewerObject.transform.translation + 
                                       forward * 0.8f +        // Forward from camera
                                       right * 0.25f +         // Right from camera
                                       up * 0.15f;             // Down from camera
  
  // Scale weapon appropriately for rifle model (the rifle is already well-proportioned)
  weaponObject.transform.scale = {1.0f, 1.0f, 1.0f}; // Use rifle at its designed scale
  weaponObject.transform.rotation = viewerObject.transform.rotation;
}

void SimpleGame::handleShooting() {
  if (cameraController.shouldShoot(lveWindow.getGLFWwindow())) {
    // Create a new projectile
    auto projectile = LveGameObject::createGameObject();
    projectile.model = projectileModel;
    
    // Get shooting direction from camera
    glm::vec3 shootDirection = cameraController.getShootDirection(viewerObject);
    
    // Position projectile at weapon tip (end of weapon in forward direction)
    projectile.transform.translation = weaponObject.transform.translation + shootDirection * 0.5f;
    projectile.transform.scale = {0.05f, 0.05f, 0.05f}; // Much smaller projectile
    projectile.color = {1.0f, 1.0f, 1.0f}; // White projectile
    
    // Store velocity in rotation components (velocity hack)
    projectile.transform.rotation.x = shootDirection.x * cameraController.projectileSpeed; // X velocity
    projectile.transform.rotation.y = shootDirection.y * cameraController.projectileSpeed; // Y velocity  
    projectile.transform.rotation.z = shootDirection.z * cameraController.projectileSpeed; // Z velocity
    
    projectiles.emplace(projectile.getId(), std::move(projectile));
    std::cout << "White projectile fired! Total projectiles: " << projectiles.size() << std::endl;
  }
}

void SimpleGame::updateProjectiles(float dt) {
  for (auto& kv : projectiles) {
    auto& projectile = kv.second;
    
    // Get velocity from rotation components
    glm::vec3 velocity = {projectile.transform.rotation.x, 
                         projectile.transform.rotation.y, 
                         projectile.transform.rotation.z};
    
    // Apply gravity to Y velocity
    velocity.y += cameraController.projectileGravity * dt;
    
    // Update position
    projectile.transform.translation += velocity * dt;
    
    // Ground collision detection (Y coordinate increases going down)
    float groundY = cameraController.groundLevel;
    if (projectile.transform.translation.y <= groundY) {
      projectile.transform.translation.y = groundY;
      
      // Bounce: reverse Y velocity and apply damping
      velocity.y = -velocity.y * cameraController.bounceDamping;
      
      // Also apply some damping to horizontal velocity for more realistic bounce
      velocity.x *= 0.9f;
      velocity.z *= 0.9f;
      
      std::cout << "Projectile bounced!" << std::endl;
    }
    
    // Simple collision with platforms/objects (basic Y-level collision)
    // Check collision with platform height levels (around Y = -0.5f to -1.0f)
    bool hitPlatform = false;
    for (auto& objKv : gameObjects) {
      auto& obj = objKv.second;
      if (obj.model != nullptr) {
        // Basic bounding box collision check
        glm::vec3 objPos = obj.transform.translation;
        glm::vec3 objScale = obj.transform.scale;
        glm::vec3 projPos = projectile.transform.translation;
        
        // Check if projectile is within object bounds
        if (projPos.x >= objPos.x - objScale.x && projPos.x <= objPos.x + objScale.x &&
            projPos.z >= objPos.z - objScale.z && projPos.z <= objPos.z + objScale.z &&
            projPos.y >= objPos.y - objScale.y && projPos.y <= objPos.y + objScale.y) {
          
          // Bounce off object surface
          velocity.y = abs(velocity.y) * cameraController.bounceDamping; // Bounce up
          velocity.x *= 0.8f; // Reduce horizontal velocity
          velocity.z *= 0.8f;
          hitPlatform = true;
          std::cout << "Projectile hit platform!" << std::endl;
          break;
        }
      }
    }
    
    // Store updated velocity back
    projectile.transform.rotation.x = velocity.x;
    projectile.transform.rotation.y = velocity.y;
    projectile.transform.rotation.z = velocity.z;
  }
  
  // Note: Projectiles persist in the scene and are never deleted
}

void SimpleGame::handleMenuInput() {
  GLFWwindow* window = lveWindow.getGLFWwindow();
  
  // Static variables to track key states
  static bool enterKeyWasPressed = false;
  static bool escapeKeyWasPressed = false;
  static bool upKeyWasPressed = false;
  static bool downKeyWasPressed = false;
  
  if (inSettings) {
    // Handle settings menu
    bool escapeKeyPressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    if (escapeKeyPressed && !escapeKeyWasPressed) {
      inSettings = false;
      std::cout << "Returned to Main Menu" << std::endl;
    }
    escapeKeyWasPressed = escapeKeyPressed;
    return;
  }
  
  if (gameState == GameState::MENU) {
    // Handle menu navigation
    bool upKeyPressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
    bool downKeyPressed = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
    
    if (upKeyPressed && !upKeyWasPressed) {
      selectedMenuOption = static_cast<MenuOption>((static_cast<int>(selectedMenuOption) - 1 + 3) % 3);
      displayMenu(); // Refresh menu display
    }
    upKeyWasPressed = upKeyPressed;
    
    if (downKeyPressed && !downKeyWasPressed) {
      selectedMenuOption = static_cast<MenuOption>((static_cast<int>(selectedMenuOption) + 1) % 3);
      displayMenu(); // Refresh menu display
    }
    downKeyWasPressed = downKeyPressed;
    
    // Handle menu selection
    bool enterKeyPressed = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;
    if (enterKeyPressed && !enterKeyWasPressed) {
      switch (selectedMenuOption) {
        case MenuOption::START_GAME:
          gameState = GameState::PLAYING;
          std::cout << "\n========================================" << std::endl;
          std::cout << "           GAME STARTED!                " << std::endl;
          std::cout << "========================================" << std::endl;
          std::cout << " You should now see platforms and      " << std::endl;
          std::cout << " objects in the game window!           " << std::endl;
          std::cout << " Use P to pause, ESC for menu          " << std::endl;
          std::cout << "========================================" << std::endl;
          // Reset camera position for game start
          viewerObject.transform.translation = {0.0f, -2.5f, -5.0f};
          viewerObject.transform.rotation = {0.0f, 0.0f, 0.0f};
          break;
        case MenuOption::SETTINGS:
          inSettings = true;
          displaySettings();
          break;
        case MenuOption::EXIT:
          std::cout << "Exiting game..." << std::endl;
          glfwSetWindowShouldClose(window, GLFW_TRUE);
          break;
      }
    }
    enterKeyWasPressed = enterKeyPressed;
  } else if (gameState == GameState::PAUSED) {
    // Handle pause menu (simplified - just resume or return to menu)
    bool enterKeyPressed = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;
    if (enterKeyPressed && !enterKeyWasPressed) {
      gameState = GameState::PLAYING;
      std::cout << "\n*** GAME RESUMED! ***" << std::endl;
    }
    enterKeyWasPressed = enterKeyPressed;
    
    bool escapeKeyPressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    if (escapeKeyPressed && !escapeKeyWasPressed) {
      gameState = GameState::MENU;
      std::cout << "Returned to Main Menu" << std::endl;
    }
    escapeKeyWasPressed = escapeKeyPressed;
  }
}

void SimpleGame::handleGameInput(float frameTime) {
  GLFWwindow* window = lveWindow.getGLFWwindow();
  
  // Check for pause (P)
  static bool pauseKeyWasPressed = false;
  bool pauseKeyPressed = glfwGetKey(window, cameraController.keys.pauseGame) == GLFW_PRESS;
  if (pauseKeyPressed && !pauseKeyWasPressed) {
    gameState = GameState::PAUSED;
    std::cout << "Game Paused! Press ENTER to resume, ESC for main menu" << std::endl;
  }
  pauseKeyWasPressed = pauseKeyPressed;
  
  // Check for exit to menu (ESC)
  static bool escapeKeyWasPressed = false;
  bool escapeKeyPressed = glfwGetKey(window, cameraController.keys.exitGame) == GLFW_PRESS;
  if (escapeKeyPressed && !escapeKeyWasPressed) {
    gameState = GameState::MENU;
    std::cout << "Returned to Main Menu" << std::endl;
  }
  escapeKeyWasPressed = escapeKeyPressed;
  
  // Normal game input when playing
  if (gameState == GameState::PLAYING) {
    // Update camera
    cameraController.moveInPlaneXZ(window, frameTime, viewerObject);
    
    // Update weapon position
    updateWeapon();
    
    // Handle shooting
    handleShooting();
    
    // Update projectiles
    updateProjectiles(frameTime);
  }
}

void SimpleGame::displayMenu() {
  if (inSettings) {
    displaySettings();
    return;
  }
  
  // Clear screen for menu display
  system("cls");
  
  std::cout << "\n" << std::endl;
  std::cout << "  ██╗   ██╗██╗   ██╗██╗     ██╗  ██╗ █████╗ ███╗   ██╗" << std::endl;
  std::cout << "  ██║   ██║██║   ██║██║     ██║ ██╔╝██╔══██╗████╗  ██║" << std::endl;
  std::cout << "  ██║   ██║██║   ██║██║     █████╔╝ ███████║██╔██╗ ██║" << std::endl;
  std::cout << "  ╚██╗ ██╔╝██║   ██║██║     ██╔═██╗ ██╔══██║██║╚██╗██║" << std::endl;
  std::cout << "   ╚████╔╝ ╚██████╔╝███████╗██║  ██╗██║  ██║██║ ╚████║" << std::endl;
  std::cout << "    ╚═══╝   ╚═════╝ ╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝" << std::endl;
  std::cout << "\n" << std::endl;
  std::cout << "               ███████╗██████╗ ███████╗                " << std::endl;
  std::cout << "               ██╔════╝██╔══██╗██╔════╝                " << std::endl;
  std::cout << "               █████╗  ██████╔╝███████╗                " << std::endl;
  std::cout << "               ██╔══╝  ██╔═══╝ ╚════██║                " << std::endl;
  std::cout << "               ██║     ██║     ███████║                " << std::endl;
  std::cout << "               ╚═╝     ╚═╝     ╚══════╝                " << std::endl;
  std::cout << "\n" << std::endl;
  std::cout << "         ═══════════════════════════════════════════" << std::endl;
  std::cout << "                      MAIN MENU                      " << std::endl;
  std::cout << "         ═══════════════════════════════════════════" << std::endl;
  std::cout << "\n" << std::endl;
  
  // Display menu options with selection indicator
  std::cout << "         " << (selectedMenuOption == MenuOption::START_GAME ? "► " : "  ") 
            << "START GAME" << std::endl;
  std::cout << "\n" << std::endl;
  std::cout << "         " << (selectedMenuOption == MenuOption::SETTINGS ? "► " : "  ") 
            << "SETTINGS" << std::endl;
  std::cout << "\n" << std::endl;
  std::cout << "         " << (selectedMenuOption == MenuOption::EXIT ? "► " : "  ") 
            << "EXIT GAME" << std::endl;
  std::cout << "\n" << std::endl;
  std::cout << "         ═══════════════════════════════════════════" << std::endl;
  std::cout << "\n" << std::endl;
  std::cout << "              Use W/S or ↑/↓ to navigate            " << std::endl;
  std::cout << "                 Press ENTER to select              " << std::endl;
  std::cout << "\n" << std::endl;
  std::cout << "         ═══════════════════════════════════════════" << std::endl;
}

void SimpleGame::displayPauseMenu() {
  static bool pauseMessageShown = false;
  if (!pauseMessageShown) {
    std::cout << "\n=======================================" << std::endl;
    std::cout << "            GAME PAUSED                 " << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "  ENTER - Resume game                  " << std::endl;
    std::cout << "  ESC - Return to main menu            " << std::endl;
    std::cout << "=======================================" << std::endl;
    pauseMessageShown = true;
  }
  
  // Reset the flag when leaving pause
  if (gameState != GameState::PAUSED) {
    pauseMessageShown = false;
  }
}

void SimpleGame::displaySettings() {
  // Clear screen for settings display
  system("cls");
  
  std::cout << "\n" << std::endl;
  std::cout << "         ═══════════════════════════════════════════" << std::endl;
  std::cout << "                      SETTINGS                      " << std::endl;
  std::cout << "         ═══════════════════════════════════════════" << std::endl;
  std::cout << "\n" << std::endl;
  std::cout << "                   GAME CONTROLS:                   " << std::endl;
  std::cout << "\n" << std::endl;
  std::cout << "              WASD - Move around                    " << std::endl;
  std::cout << "              Mouse - Look around (FPS style)       " << std::endl;
  std::cout << "              Spacebar - Jump                       " << std::endl;
  std::cout << "              Left Click - Shoot projectiles        " << std::endl;
  std::cout << "              P - Pause/Resume game                 " << std::endl;
  std::cout << "              ESC - Return to menu                  " << std::endl;
  std::cout << "\n" << std::endl;
  std::cout << "                    GAME INFO:                      " << std::endl;
  std::cout << "\n" << std::endl;
  std::cout << "              • Physics enabled with gravity        " << std::endl;
  std::cout << "              • Projectiles bounce off surfaces     " << std::endl;
  std::cout << "              • FPS-style camera movement           " << std::endl;
  std::cout << "              • Real-time 3D rendering              " << std::endl;
  std::cout << "\n" << std::endl;
  std::cout << "         ═══════════════════════════════════════════" << std::endl;
  std::cout << "\n" << std::endl;
  std::cout << "               Press ESC to return to menu          " << std::endl;
  std::cout << "\n" << std::endl;
  std::cout << "         ═══════════════════════════════════════════" << std::endl;
}

void SimpleGame::createMenuObjects() {
  std::cout << "Creating visual menu objects..." << std::endl;
  
  // Create 3D menu option cubes that will represent our menu
  // START GAME option
  auto startGameCube = LveGameObject::createGameObject();
  startGameCube.model = menuCubeModel;
  startGameCube.transform.translation = {0.0f, 0.5f, 0.0f};
  startGameCube.transform.scale = {2.0f, 0.3f, 0.5f};
  startGameCube.color = {0.2f, 0.8f, 0.2f}; // Green for START GAME
  menuObjects.emplace(0, std::move(startGameCube)); // ID 0 = START GAME
  
  // SETTINGS option
  auto settingsCube = LveGameObject::createGameObject();
  settingsCube.model = menuCubeModel;
  settingsCube.transform.translation = {0.0f, 0.0f, 0.0f};
  settingsCube.transform.scale = {2.0f, 0.3f, 0.5f};
  settingsCube.color = {0.2f, 0.2f, 0.8f}; // Blue for SETTINGS
  menuObjects.emplace(1, std::move(settingsCube)); // ID 1 = SETTINGS
  
  // EXIT GAME option
  auto exitCube = LveGameObject::createGameObject();
  exitCube.model = menuCubeModel;
  exitCube.transform.translation = {0.0f, -0.5f, 0.0f};
  exitCube.transform.scale = {2.0f, 0.3f, 0.5f};
  exitCube.color = {0.8f, 0.2f, 0.2f}; // Red for EXIT GAME
  menuObjects.emplace(2, std::move(exitCube)); // ID 2 = EXIT GAME
  
  // Menu title cubes (VULKAN FPS)
  auto titleCube1 = LveGameObject::createGameObject();
  titleCube1.model = menuCubeModel;
  titleCube1.transform.translation = {-2.0f, 1.5f, 0.0f};
  titleCube1.transform.scale = {0.3f, 0.3f, 0.3f};
  titleCube1.color = {0.9f, 0.9f, 0.1f}; // Yellow for title
  menuObjects.emplace(10, std::move(titleCube1));
  
  auto titleCube2 = LveGameObject::createGameObject();
  titleCube2.model = menuCubeModel;
  titleCube2.transform.translation = {-1.0f, 1.5f, 0.0f};
  titleCube2.transform.scale = {0.3f, 0.3f, 0.3f};
  titleCube2.color = {0.9f, 0.9f, 0.1f};
  menuObjects.emplace(11, std::move(titleCube2));
  
  auto titleCube3 = LveGameObject::createGameObject();
  titleCube3.model = menuCubeModel;
  titleCube3.transform.translation = {0.0f, 1.5f, 0.0f};
  titleCube3.transform.scale = {0.3f, 0.3f, 0.3f};
  titleCube3.color = {0.9f, 0.9f, 0.1f};
  menuObjects.emplace(12, std::move(titleCube3));
  
  auto titleCube4 = LveGameObject::createGameObject();
  titleCube4.model = menuCubeModel;
  titleCube4.transform.translation = {1.0f, 1.5f, 0.0f};
  titleCube4.transform.scale = {0.3f, 0.3f, 0.3f};
  titleCube4.color = {0.9f, 0.9f, 0.1f};
  menuObjects.emplace(13, std::move(titleCube4));
  
  auto titleCube5 = LveGameObject::createGameObject();
  titleCube5.model = menuCubeModel;
  titleCube5.transform.translation = {2.0f, 1.5f, 0.0f};
  titleCube5.transform.scale = {0.3f, 0.3f, 0.3f};
  titleCube5.color = {0.9f, 0.9f, 0.1f};
  menuObjects.emplace(14, std::move(titleCube5));
  
  std::cout << "Visual menu objects created!" << std::endl;
}

void SimpleGame::updateMenuVisuals() {
  // Update the visual appearance of menu items based on selection
  // Make selected item larger and brighter
  
  for (auto& kv : menuObjects) {
    auto& obj = kv.second;
    int menuId = kv.first;
    
    // Only update menu option cubes (IDs 0, 1, 2)
    if (menuId < 3) {
      if (menuId == static_cast<int>(selectedMenuOption)) {
        // Selected item - larger and brighter
        obj.transform.scale = {2.5f, 0.4f, 0.6f};
        // Make it pulse slightly by adding some brightness
        float pulse = 0.5f + 0.3f * sin(glfwGetTime() * 3.0f);
        switch (menuId) {
          case 0: obj.color = {0.2f * pulse, 1.0f * pulse, 0.2f * pulse}; break; // Green
          case 1: obj.color = {0.2f * pulse, 0.2f * pulse, 1.0f * pulse}; break; // Blue  
          case 2: obj.color = {1.0f * pulse, 0.2f * pulse, 0.2f * pulse}; break; // Red
        }
      } else {
        // Unselected item - normal size and dimmer
        obj.transform.scale = {2.0f, 0.3f, 0.5f};
        switch (menuId) {
          case 0: obj.color = {0.1f, 0.4f, 0.1f}; break; // Dim green
          case 1: obj.color = {0.1f, 0.1f, 0.4f}; break; // Dim blue
          case 2: obj.color = {0.4f, 0.1f, 0.1f}; break; // Dim red
        }
      }
    }
  }
}

}  // namespace lve
