//
// Created by gahwon on 5/2/19.
//

#ifndef FLUVIT_CAMERA_H
#define FLUVIT_CAMERA_H

#include <glm/glm.hpp>
#include "../Entity.h"
#include "../gui.h"

class Camera : public Entity {
    static constexpr float kNear = 0.1f;
    static constexpr float kFar = 1000.0f;
    static constexpr float kFov = 45.0f;

    static constexpr float yaw_speed = 0.3f;
    static constexpr float pitch_speed = 0.3f;
    static constexpr float move_speed = 1.0f;

    glm::vec3 position{0.0f, 1.0f, 0.0f};

    /// up down [-pi, pi]
    float pitch = 0;
    /// left right [0, 2pi)
    float yaw = 0;

    glm::mat4 view_matrix{1};
    glm::mat4 projection_matrix;

public:
    explicit Camera(float aspect);

    void start() override;

    void update() override;

    const glm::vec3 &getPos() const {
        return position;
    }

    MatrixPointers getMatrixPointers() const;
};


#endif //FLUVIT_CAMERA_H
