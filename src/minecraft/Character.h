//
// Created by gahwon on 4/17/19.
//

#ifndef GLSL_CHARACTER_H
#define GLSL_CHARACTER_H


#include "../Entity.h"
#include "World.h"
#include "../gui.h"

class Character : public Entity {
public:
    Character(World &world, float aspect);

    void start() override;

    void update() override;

    const glm::vec3 &getPos() const {
        return position;
    }

    MatrixPointers getMatrixPointers() const;

private:
    static constexpr float kNear = 0.1f;
    static constexpr float kFar = 1000.0f;
    static constexpr float kFov = 45.0f;

    static constexpr float yaw_speed = 0.3f;
    static constexpr float pitch_speed = 0.3f;
    static constexpr float move_speed = 8.0f;
    static constexpr float jump_height = 10.0f;
    static constexpr float fly_speed = 8.0f;

    /// needs to be in range (0, 0.5]
    static constexpr float cylinder_radius = 0.5f;
    static constexpr float cylinder_height = 1.75f;
    static constexpr float slab_height = 0.1f;
    static constexpr float skin_width = 1.1f;

    World &world;

public:
    glm::vec3 position{0.5f, 0.5f, 0.5f};
    glm::vec3 camera_offset{0, cylinder_height, 0};

    /// up down [-pi, pi]
    float pitch;
    /// left right [0, 2pi)
    float yaw;

private:
    glm::vec3 velocity;
    float damping = 0;//0.1f;

    bool is_grounded;

    glm::mat4 view_matrix{1};
    glm::mat4 projection_matrix;

    glm::vec3 update_collision(glm::vec3 displacement);
    glm::vec3 update_collision_horiz(glm::vec3 displacement);
    glm::vec3 update_collision_vert(glm::vec3 displacement);
};


#endif //GLSL_CHARACTER_H
