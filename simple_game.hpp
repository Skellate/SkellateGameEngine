#pragma once

#include "ve_camera.hpp"
#include "ve_device.hpp"
#include "ve_game_object.hpp"
#include "ve_pipeline.hpp"
#include "ve_swap_chain.hpp"
#include "ve_window.hpp"
#include "keyboard_movement_controller.hpp"

#include <memory>
#include <vector>
#include <chrono>

namespace lve {

enum class GameState {
  MENU,
  PLAYING,
  PAUSED
};

class SimpleGame {
 public:
  static constexpr int WIDTH = 900;
  static constexpr int HEIGHT = 660;

  SimpleGame();
  ~SimpleGame();

  SimpleGame(const SimpleGame &) = delete;
  SimpleGame &operator=(const SimpleGame &) = delete;

  void run();

 private:
  void loadGameObjects();
  void createPipelineLayout();
  void createPipeline();
  void createCommandBuffers();
  void freeCommandBuffers();
  void drawFrame();
  void recreateSwapChain();
  void recordCommandBuffer(int imageIndex);
  void updateProjectiles(float dt);
  void handleShooting();
  void updateWeapon();
  void handleMenuInput();
  void handleGameInput(float dt);
  void displayMenu();
  void displayPauseMenu();
  void displaySettings();
  void createMenuObjects();
  void updateMenuVisuals();

  ve_window lveWindow{WIDTH, HEIGHT, "Vulkan FPS Game!"};
  LveDevice lveDevice{lveWindow};
  std::unique_ptr<LveSwapChain> lveSwapChain;
  std::unique_ptr<vePipeline> lvePipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;

  // Game objects and systems
  LveGameObject::Map gameObjects;
  LveGameObject::Map projectiles;
  LveGameObject::Map menuObjects; // Menu visual elements
  LveGameObject viewerObject = LveGameObject::createGameObject();
  LveGameObject weaponObject = LveGameObject::createGameObject();
  KeyboardMovementController cameraController{};
  LveCamera camera{};
  
  // Models for reuse
  std::shared_ptr<LveModel> projectileModel;
  std::shared_ptr<LveModel> weaponModel;
  std::shared_ptr<LveModel> menuCubeModel; // For menu options
  
  // Timing
  std::chrono::steady_clock::time_point currentTime;
  
  // Game state and menu navigation
  GameState gameState{GameState::MENU};
  bool showInstructions{true};
  
  // Menu navigation
  enum class MenuOption {
    START_GAME = 0,
    SETTINGS = 1,
    EXIT = 2
  };
  MenuOption selectedMenuOption{MenuOption::START_GAME};
  bool inSettings{false};
};

}  // namespace lve
