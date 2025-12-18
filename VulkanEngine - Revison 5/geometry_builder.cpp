#include "geometry_builder.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <vector>

namespace lve {

std::shared_ptr<LveModel> GeometryBuilder::createCube(LveDevice& device, float size) {
    float half = size * 0.5f;
    
    std::vector<LveModel::Vertex> vertices{
        // Front face
        {{-half, -half,  half}, {1.0f, 0.0f, 0.0f}},  // 0
        {{ half, -half,  half}, {0.0f, 1.0f, 0.0f}},  // 1
        {{ half,  half,  half}, {0.0f, 0.0f, 1.0f}},  // 2
        {{-half,  half,  half}, {1.0f, 1.0f, 0.0f}},  // 3
        
        // Back face
        {{-half, -half, -half}, {1.0f, 0.0f, 1.0f}},  // 4
        {{ half, -half, -half}, {0.0f, 1.0f, 1.0f}},  // 5
        {{ half,  half, -half}, {1.0f, 1.0f, 1.0f}},  // 6
        {{-half,  half, -half}, {0.5f, 0.5f, 0.5f}},  // 7
    };

    std::vector<uint32_t> indices{
        // Front face
        0, 1, 2,  2, 3, 0,
        // Back face
        4, 6, 5,  6, 4, 7,
        // Left face
        4, 0, 3,  3, 7, 4,
        // Right face
        1, 5, 6,  6, 2, 1,
        // Top face
        3, 2, 6,  6, 7, 3,
        // Bottom face
        4, 5, 1,  1, 0, 4
    };

    return std::make_shared<LveModel>(device, vertices, indices);
}

std::shared_ptr<LveModel> GeometryBuilder::createSphere(LveDevice& device, float radius, int segments, int rings) {
    std::vector<LveModel::Vertex> vertices;
    std::vector<uint32_t> indices;

    // Generate vertices
    for (int ring = 0; ring <= rings; ++ring) {
        float phi = glm::pi<float>() * static_cast<float>(ring) / static_cast<float>(rings);
        float y = radius * cos(phi);
        float ringRadius = radius * sin(phi);

        for (int segment = 0; segment <= segments; ++segment) {
            float theta = 2.0f * glm::pi<float>() * static_cast<float>(segment) / static_cast<float>(segments);
            float x = ringRadius * cos(theta);
            float z = ringRadius * sin(theta);

            // UV coordinates for color generation
            float u = static_cast<float>(segment) / static_cast<float>(segments);
            float v = static_cast<float>(ring) / static_cast<float>(rings);

            vertices.push_back({{x, y, z}, generateColor(u, v)});
        }
    }

    // Generate indices
    for (int ring = 0; ring < rings; ++ring) {
        for (int segment = 0; segment < segments; ++segment) {
            int current = ring * (segments + 1) + segment;
            int next = current + segments + 1;

            // First triangle
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            // Second triangle
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    return std::make_shared<LveModel>(device, vertices, indices);
}

std::shared_ptr<LveModel> GeometryBuilder::createPlane(LveDevice& device, float width, float height) {
    float halfW = width * 0.5f;
    float halfH = height * 0.5f;
    
    std::vector<LveModel::Vertex> vertices{
        {{-halfW, 0.0f, -halfH}, {0.2f, 0.8f, 0.2f}},  // Bottom-left
        {{ halfW, 0.0f, -halfH}, {0.8f, 0.8f, 0.2f}},  // Bottom-right
        {{ halfW, 0.0f,  halfH}, {0.8f, 0.2f, 0.8f}},  // Top-right
        {{-halfW, 0.0f,  halfH}, {0.2f, 0.2f, 0.8f}},  // Top-left
    };

    std::vector<uint32_t> indices{
        0, 1, 2,  2, 3, 0
    };

    return std::make_shared<LveModel>(device, vertices, indices);
}

glm::vec3 GeometryBuilder::generateColor(float u, float v) {
    // Generate a nice gradient color based on UV coordinates
    float r = 0.5f + 0.5f * sin(u * 2.0f * glm::pi<float>());
    float g = 0.5f + 0.5f * sin(v * 2.0f * glm::pi<float>());
    float b = 0.5f + 0.5f * sin((u + v) * glm::pi<float>());
    return {r, g, b};
}

std::shared_ptr<LveModel> GeometryBuilder::createRifle(LveDevice& device, float scale) {
    std::vector<LveModel::Vertex> vertices;
    std::vector<uint32_t> indices;
    
    // Simple rifle made of connected rectangular parts
    // Colors for different parts
    glm::vec3 barrelColor = {0.2f, 0.2f, 0.2f};      // Dark gray barrel
    glm::vec3 stockColor = {0.4f, 0.2f, 0.1f};       // Brown wooden stock
    glm::vec3 metalColor = {0.3f, 0.3f, 0.3f};       // Metal parts
    
    // === BARREL (long rectangular shape) ===
    float barrelLength = 1.0f * scale;
    float barrelWidth = 0.06f * scale;
    float barrelHeight = 0.06f * scale;
    
    // Barrel vertices (8 vertices for rectangular barrel)
    std::vector<glm::vec3> barrelPositions = {
        {-barrelWidth/2, -barrelHeight/2, -barrelLength/2},  // 0: back-bottom-left
        { barrelWidth/2, -barrelHeight/2, -barrelLength/2},  // 1: back-bottom-right
        { barrelWidth/2,  barrelHeight/2, -barrelLength/2},  // 2: back-top-right
        {-barrelWidth/2,  barrelHeight/2, -barrelLength/2},  // 3: back-top-left
        {-barrelWidth/2, -barrelHeight/2,  barrelLength/2},  // 4: front-bottom-left
        { barrelWidth/2, -barrelHeight/2,  barrelLength/2},  // 5: front-bottom-right
        { barrelWidth/2,  barrelHeight/2,  barrelLength/2},  // 6: front-top-right
        {-barrelWidth/2,  barrelHeight/2,  barrelLength/2},  // 7: front-top-left
    };
    
    for (const auto& pos : barrelPositions) {
        vertices.push_back({pos, barrelColor});
    }
    
    // Barrel faces (12 triangles = 6 faces * 2 triangles each)
    std::vector<uint32_t> cubeIndices = {
        // Front face
        4, 5, 6,  6, 7, 4,
        // Back face
        0, 2, 1,  2, 0, 3,
        // Left face
        0, 4, 7,  7, 3, 0,
        // Right face
        5, 1, 2,  2, 6, 5,
        // Top face
        7, 6, 2,  2, 3, 7,
        // Bottom face
        0, 1, 5,  5, 4, 0
    };
    
    uint32_t vertexOffset = 0;
    for (uint32_t idx : cubeIndices) {
        indices.push_back(vertexOffset + idx);
    }
    vertexOffset += 8;
    
    // === STOCK (rectangular wooden part behind barrel) ===
    float stockLength = 0.6f * scale;
    float stockWidth = 0.08f * scale;
    float stockHeight = 0.25f * scale;
    float stockStartZ = -barrelLength/2 - 0.05f * scale;
    
    std::vector<glm::vec3> stockPositions = {
        {-stockWidth/2, -stockHeight/2, stockStartZ - stockLength},  // 0: back-bottom-left
        { stockWidth/2, -stockHeight/2, stockStartZ - stockLength},  // 1: back-bottom-right
        { stockWidth/2,  stockHeight/2, stockStartZ - stockLength},  // 2: back-top-right
        {-stockWidth/2,  stockHeight/2, stockStartZ - stockLength},  // 3: back-top-left
        {-stockWidth/2, -stockHeight/2, stockStartZ},                // 4: front-bottom-left
        { stockWidth/2, -stockHeight/2, stockStartZ},                // 5: front-bottom-right
        { stockWidth/2,  stockHeight/2, stockStartZ},                // 6: front-top-right
        {-stockWidth/2,  stockHeight/2, stockStartZ},                // 7: front-top-left
    };
    
    for (const auto& pos : stockPositions) {
        vertices.push_back({pos, stockColor});
    }
    
    // Stock faces (same cube structure)
    for (uint32_t idx : cubeIndices) {
        indices.push_back(vertexOffset + idx);
    }
    vertexOffset += 8;
    
    // === GRIP (handle below stock) ===
    float gripWidth = stockWidth * 1.2f;
    float gripHeight = stockHeight * 1.5f;
    float gripLength = 0.12f * scale;
    float gripZ = stockStartZ - stockLength * 0.2f;
    
    std::vector<glm::vec3> gripPositions = {
        {-gripWidth/2, -gripHeight, gripZ - gripLength/2},
        { gripWidth/2, -gripHeight, gripZ - gripLength/2},
        { gripWidth/2, -gripHeight/4, gripZ - gripLength/2},
        {-gripWidth/2, -gripHeight/4, gripZ - gripLength/2},
        {-gripWidth/2, -gripHeight, gripZ + gripLength/2},
        { gripWidth/2, -gripHeight, gripZ + gripLength/2},
        { gripWidth/2, -gripHeight/4, gripZ + gripLength/2},
        {-gripWidth/2, -gripHeight/4, gripZ + gripLength/2},
    };
    
    for (const auto& pos : gripPositions) {
        vertices.push_back({pos, metalColor});
    }
    
    // Grip faces
    for (uint32_t idx : cubeIndices) {
        indices.push_back(vertexOffset + idx);
    }
    
    return std::make_shared<LveModel>(device, vertices, indices);
}

}  // namespace lve
