//
// Created by gahwon on 4/17/19.
//

#include "Character.h"
#include <glm/gtx/euler_angles.hpp>
#include "../Input.h"
#include "../time.h"

Character::Character(World &world, float aspect) : world(world) {
    projection_matrix = glm::perspective((float) (kFov * (M_PI / 180.0f)), aspect, kNear, kFar);
}

void Character::start() {
    position.y = world.get_height(0, 0);
}

void Character::update() {
    // input update
    {
        yaw -= Input::yaw * yaw_speed * Time::delta_time;
        pitch -= Input::pitch * pitch_speed * Time::delta_time;
        pitch = glm::clamp(pitch, -glm::half_pi<float>(), glm::half_pi<float>());

        if (Input::enable_gravity) {
            auto right = -glm::vec2(-glm::cos(yaw), glm::sin(yaw));
            auto forward = glm::vec2(-right.y, right.x);

            auto horiz_delta = move_speed * (Input::zoom * forward + Input::strafe * right);

            velocity = glm::vec3(horiz_delta.x, velocity.y, horiz_delta.y);
            if (Input::jump && is_grounded)
                velocity.y = jump_height;
        } else {
            auto mat = glm::yawPitchRoll(yaw, pitch, 0.0f);
            auto movement = Time::delta_time * fly_speed * glm::vec4(Input::strafe, Input::updown, Input::zoom, 0);
            position += glm::vec3(mat * movement);
            position.y = glm::max(position.y, (float) world.get_max_height(position.x, position.z));
        }
    }

    // physics update
    if (Input::enable_gravity) {
        velocity -= glm::vec3(0, World::gravity * Time::delta_time, 0);

        if (damping > 0)
            velocity *= 1.0f / (1.0f + Time::delta_time * damping);

        auto displacement = velocity * Time::delta_time;
        auto new_disp = update_collision(displacement);

        if (displacement != new_disp) {
            if (Time::delta_time <= 0)
                velocity = glm::vec3();
            else {
                // if clipping backwards, don't increase velocity
                if ((displacement.x > 0 && new_disp.x < 0) || (displacement.x < 0 && new_disp.x > 0)) {
                    position += glm::vec3(new_disp.x, 0, 0);
                    new_disp.x = 0;
                }

                if ((displacement.y > 0 && new_disp.y < 0) || (displacement.y < 0 && new_disp.y > 0)) {
                    position += glm::vec3(0, new_disp.y, 0);
                    new_disp.y = 0;
                }

                if ((displacement.z > 0 && new_disp.z < 0) || (displacement.z < 0 && new_disp.z > 0)) {
                    position += glm::vec3(0, 0, new_disp.z);
                    new_disp.z = 0;
                }

                velocity = new_disp / Time::delta_time;
            }
        }
        position += velocity * Time::delta_time;
    }

    // view matrix update
    {
        auto p = position + camera_offset;
        view_matrix = glm::inverse(glm::translate(glm::mat4(1), p) * glm::yawPitchRoll(yaw, pitch, 0.0f));
    }
}

glm::vec3 Character::update_collision(glm::vec3 displacement) {
    is_grounded = position.y + displacement.y < 61.875f;
    auto update_vert = displacement.y < 0;
    auto update_horiz = displacement.x != 0 || displacement.z != 0;

    if (update_vert && update_horiz) {
        if (displacement.y * displacement.y > displacement.x * displacement.x + displacement.z * displacement.z) {
            displacement = update_collision_horiz(displacement);
            displacement = update_collision_vert(displacement);
        } else {
            displacement = update_collision_horiz(displacement);
            displacement = update_collision_vert(displacement);
        }
    } else if (update_horiz)
        displacement = update_collision_horiz(displacement);
    else if (update_vert)
        displacement = update_collision_vert(displacement);
    return displacement;
}

bool circle_square_intersect(glm::vec2 circle_center, float radius, glm::vec2 square_lower_left, float square_size) {
    auto delta = circle_center - glm::max(square_lower_left,
                                          glm::min(circle_center, square_lower_left + glm::vec2(square_size)));
    return glm::dot(delta, delta) < radius * radius;
}

glm::vec3 Character::update_collision_vert(glm::vec3 displacement) {
    auto center = glm::vec2(position.x + displacement.x, position.z + displacement.z);
    glm::ivec2 min = glm::ivec2(glm::round(center)) - glm::ivec2(1);

    auto max = 0;

    if (circle_square_intersect(center, cylinder_radius, min, 1))
        max = glm::max(max, world.get_height(min.x, min.y));
    if (circle_square_intersect(center, cylinder_radius, min + glm::ivec2(0, 1), 1))
        max = glm::max(max, world.get_height(min.x, min.y + 1));
    if (circle_square_intersect(center, cylinder_radius, min + glm::ivec2(1, 1), 1))
        max = glm::max(max, world.get_height(min.x + 1, min.y + 1));
    if (circle_square_intersect(center, cylinder_radius, min + glm::ivec2(1, 0), 1))
        max = glm::max(max, world.get_height(min.x + 1, min.y));

    if (max > position.y + displacement.y) {
        displacement.y = max - position.y;
        displacement.y = 0;
        is_grounded = true;
    }

    return displacement;
}

glm::vec3 Character::update_collision_horiz(glm::vec3 displacement) {
    if (false)
        return displacement;
    auto pos = position + displacement;
    auto center = glm::vec2(pos.x, pos.z);
    glm::ivec2 min_grid = glm::ivec2(glm::round(center)) - glm::ivec2(1);

    auto frac = glm::fract(pos);

    auto x_min = 0.0f;
    auto z_min = 0.0f;
    auto x_max = 0.0f;
    auto z_max = 0.0f;

    if (circle_square_intersect(center, cylinder_radius, min_grid, 1) &&
        world.get_height(min_grid.x, min_grid.y) > pos.y + slab_height) {
        auto xpos = frac.z >= 0.5f ? 1.0f : (0.5f + 0.5f * glm::sin(frac.z * glm::pi<float>()));
        x_min = glm::max(x_min, xpos);
        auto zpos = frac.x >= 0.5f ? 1.0f : (0.5f + 0.5f * glm::sin(frac.x * glm::pi<float>()));
        z_min = glm::max(z_min, zpos);
    }
    if (circle_square_intersect(center, cylinder_radius, min_grid + glm::ivec2(0, 1), 1) &&
        world.get_height(min_grid.x, min_grid.y + 1) > pos.y + slab_height) {
        auto xpos = frac.z >= 0.5f ? 1.0f : (0.5f + 0.5f * glm::sin(frac.z * glm::pi<float>()));
        x_min = glm::max(x_min, xpos);
        auto fracx = 1 - frac.x;
        auto zpos = fracx >= 0.5f ? 1.0f : (0.5f + 0.5f * glm::sin(fracx * glm::pi<float>()));
        z_max = glm::max(z_max, zpos);

    }
    if (circle_square_intersect(center, cylinder_radius, min_grid + glm::ivec2(1, 1), 1) &&
        world.get_height(min_grid.x + 1, min_grid.y + 1) > pos.y + slab_height) {
        auto fracz = 1 - frac.z;
        auto xpos = fracz >= 0.5f ? 1.0f : (0.5f + 0.5f * glm::sin(fracz * glm::pi<float>()));
        x_max = glm::max(x_max, xpos);
        auto fracx = 1 - frac.x;
        auto zpos = fracx >= 0.5f ? 1.0f : (0.5f + 0.5f * glm::sin(fracx * glm::pi<float>()));
        z_max = glm::max(z_max, zpos);
    }
    if (circle_square_intersect(center, cylinder_radius, min_grid + glm::ivec2(1, 0), 1) &&
        world.get_height(min_grid.x + 1, min_grid.y) > pos.y + slab_height) {
        auto fracz = 1 - frac.z;
        auto xpos = fracz >= 0.5f ? 1.0f : (0.5f + 0.5f * glm::sin(fracz * glm::pi<float>()));
        x_max = glm::max(x_max, xpos);
        auto zpos = frac.x >= 0.5f ? 1.0f : (0.5f + 0.5f * glm::sin(frac.x * glm::pi<float>()));
        z_min = glm::max(z_min, zpos);
    }

    if (displacement.x < 0 && x_min > 0) {
        auto boundary = min_grid.x + (x_min - 0.5f) + skin_width;
        if (boundary > pos.x) {
            displacement.x = boundary - position.x;
            displacement.x = 0;
//            std::cout << "x min" << std::endl;
        }
    } else if (displacement.x > 0 && x_max > 0) {
        auto boundary = min_grid.x + 1 - (x_min - 0.5f) - skin_width;
        if (boundary < pos.x) {
            displacement.x = boundary - position.x;
            displacement.x = 0;
//            std::cout << "x max" << std::endl;
        }
    }

    if (displacement.z < 0 && z_min > 0) {
        auto boundary = min_grid.y + (z_min - 0.5f) + skin_width;
        if (boundary > pos.z) {
            displacement.z = boundary - position.z;
            displacement.z = 0;
//            std::cout << "z min" << std::endl;

        }
    } else if (displacement.z > 0 && z_max > 0) {
        auto boundary = min_grid.y + 1 - (z_min - 0.5f) - skin_width;
        if (boundary < pos.z) {
            displacement.z = boundary - position.z;
            displacement.z = 0;
//            std::cout << "z max" << std::endl;

        }
    }

    return displacement;
}

MatrixPointers Character::getMatrixPointers() const {
    MatrixPointers ret{};
    ret.projection = &projection_matrix;
    ret.view = &view_matrix;
    return ret;
}
