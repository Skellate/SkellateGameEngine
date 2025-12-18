#include "ve_game_object.hpp"

namespace lve {

LveGameObject LveGameObject::makePointLight(float intensity, float radius, glm::vec3 color) {
    auto gameObj = LveGameObject::createGameObject();
    gameObj.color = color;
    gameObj.transform.scale.x = radius;
    return gameObj;
}

}  // namespace lve
