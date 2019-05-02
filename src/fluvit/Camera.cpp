//
// Created by gahwon on 5/2/19.
//

#include <glm/gtx/euler_angles.hpp>
#include "Camera.h"
#include "../Input.h"
#include "../time.h"

Camera::Camera(float aspect) {
    projection_matrix = glm::perspective((float) (kFov * (M_PI / 180.0f)), aspect, kNear, kFar);
}

void Camera::start() {
    Entity::start();
}

void Camera::update() {
    if (Input::left_click) {
        // todo: bring down rain
    }

    if (Input::right_click) {
        yaw -= Input::mouse_x_delta * yaw_speed * Time::delta_time;
        pitch -= Input::mouse_y_delta * pitch_speed * Time::delta_time;
        pitch = glm::clamp(pitch, -glm::half_pi<float>(), glm::half_pi<float>());
    }

    auto mat = glm::yawPitchRoll(yaw, pitch, 0.0f);
    auto movement = Time::delta_time * move_speed * glm::vec4(Input::strafe, Input::updown, Input::zoom, 0);
    position += glm::vec3(mat * movement);

    view_matrix = glm::inverse(glm::translate(glm::mat4(1), position) * glm::yawPitchRoll(yaw, pitch, 0.0f));
}

MatrixPointers Camera::getMatrixPointers() const {
    MatrixPointers ret{};
    ret.projection = &projection_matrix;
    ret.view = &view_matrix;
    return ret;
}
