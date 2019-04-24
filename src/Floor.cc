//
// Created by gahwon on 4/16/19.
//

#include "Floor.h"
#include "common_uniforms.h"

namespace {
// @formatter:off
const char* vertex_shader =
#include "shaders/floor.vert"
;

const char* geometry_shader =
#include "shaders/floor.geom"
;

const char* fragment_shader =
#include "shaders/floor.frag"
;
// @formatter:on
}


void Floor::start() {
    vertices.emplace_back(kFloorXMin, kFloorY, kFloorZMax, 1.0f);
    vertices.emplace_back(kFloorXMax, kFloorY, kFloorZMax, 1.0f);
    vertices.emplace_back(kFloorXMax, kFloorY, kFloorZMin, 1.0f);
    vertices.emplace_back(kFloorXMin, kFloorY, kFloorZMin, 1.0f);
    faces.emplace_back(0, 1, 2);
    faces.emplace_back(2, 3, 0);

    model = make_uniform("model", (std::function<glm::mat4()>) []() { return glm::mat4(1.0f); });

    input.assign(0, "vertex_position", vertices.data(), vertices.size(), 4, GL_FLOAT);
    input.assignIndex(faces.data(), faces.size(), 3);
    pass = new RenderPass(-1,
                          input,
                          {vertex_shader, geometry_shader, fragment_shader},
                          {model, common_uniforms::instance.view, common_uniforms::instance.projection,
                           common_uniforms::instance.camera_position, common_uniforms::instance.light_position},
                          {"fragment_color"}
    );
}

void Floor::render() {
    if (pass->enabled) {
        pass->setup();
        pass->render();
    }
}

Floor::~Floor() {
    delete pass;
}
