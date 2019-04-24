//
// Created by gahwon on 4/17/19.
//

#include <bits/stdc++.h>
#include <glm/glm.hpp>
#include "../Entity.h"
#include "Chunk.h"

#ifndef GLSL_CHUNK_GENERATOR_H
#define GLSL_CHUNK_GENERATOR_H

template<>
struct std::hash<glm::ivec2> {
    std::size_t operator()(const glm::ivec2 &k) const {
        return k.x * 391 + k.y;
    }
};

class World : public Entity {
public:
    static constexpr float gravity = 39.81f;

    explicit World();

    void start() override;

    void update() override;

    void render() override;

    int get_max_height(float x, float z);
    int get_height(int x, int z);

    void setCameraPos(const std::function<glm::vec3()> &cameraPos) {
        camera_pos = cameraPos;
    }
    void setYaw(const std::function<float()> &y) {
        yaw = y;
    }

private:
    static constexpr int render_distance = 24;

    std::function<glm::vec3()> camera_pos;
    std::function<float()> yaw;

    std::unordered_map<glm::ivec2, Chunk> chunks{};
};


#endif //GLSL_CHUNK_GENERATOR_H
