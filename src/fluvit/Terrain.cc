//
// Created by gahwon on 4/24/19.
//

#include <glm/gtx/transform.hpp>
#include "Terrain.h"
#include "../common_uniforms.h"

namespace {
// @formatter:off
const char* vertex_shader =
#include "shaders/terrain.vert"
;

const char* geometry_shader =
#include "shaders/terrain.geom"
;

const char* fragment_shader =
#include "shaders/terrain.frag"
;

const char* water_geometry_shader =
#include "shaders/water.geom"
;

const char* water_fragment_shader =
#include "shaders/water.frag"
;
// @formatter:on
}

Terrain::Terrain(Simulation *simulation) : simulation(simulation) {}

void Terrain::start() {
    init_terrain(num_rows, num_cols);

    model = make_uniform("model", (std::function<glm::mat4()>) []() {
        return glm::translate(glm::vec3(-0.5f, 0, -0.5f));
    });
    sim_texture1 = make_texture("simulation1", (std::function<uint32_t()>) [this]() {
        return simulation->get_sampler();
    }, 0, (std::function<uint32_t()>) [this]() {
        return simulation->get_texture1();
    });
    sim_texture3 = make_texture("simulation3", (std::function<uint32_t()>) [this]() {
        return simulation->get_sampler();
    }, 1, (std::function<uint32_t()>) [this]() {
        return simulation->get_texture3();
    });

    input.assign(0, "vertex_position", vertices.data(), vertices.size(), 4, GL_FLOAT);
    input.assignIndex(faces.data(), faces.size(), 3);
    terrain_pass = new RenderPass(-1,
                          input,
                          {vertex_shader, geometry_shader, fragment_shader},
                          {model, common_uniforms::instance.view, common_uniforms::instance.projection,
                           common_uniforms::instance.light_position, sim_texture1, sim_texture3},
                          {"fragment_color"}
    );
    water_pass = new RenderPass(-1,
                          input,
                          {vertex_shader, water_geometry_shader, water_fragment_shader},
                          {model, common_uniforms::instance.view, common_uniforms::instance.projection,
                           common_uniforms::instance.light_position, sim_texture1},
                          {"fragment_color"}
    );
}

void Terrain::render() {
    if (terrain_pass->enabled) {
        terrain_pass->setup();
        terrain_pass->render();
    }
    if (water_pass->enabled) {
        water_pass->setup();
        water_pass->render();
    }
}

void Terrain::init_terrain(int rows, int cols) {
    auto dx = 1.0f / rows;
    auto dz = 1.0f / cols;

    auto flipped = false;
    for (auto z = 0; z <= cols; ++z) {
        auto startz = glm::min(z * dz, 1.0f);

        if (flipped) {
            vertices.emplace_back(0, 0, startz, 1);
            for (auto x = 0; x < rows; ++x) {
                vertices.emplace_back(glm::min(x * dx + dx * 0.5f, 1.0f), 0, startz, 1);
            }
            vertices.emplace_back(1.0f, 0, startz, 1);
        } else {
            for (auto x = 0; x <= rows; ++x) {
                vertices.emplace_back(glm::min(x * dx, 1.0f), 0, startz, 1);
            }
        }

        flipped = !flipped;
    }

    flipped = false;
    auto index = 0;
    for (auto z = 0; z < cols; ++z) {
        if (flipped) {
            auto top_index = index + rows + 2;
            faces.emplace_back(index, top_index, index + 1);
            ++index;
            for (auto x = 0; x < rows; ++x) {
                faces.emplace_back(index + 0, top_index + 0, top_index + 1);
                faces.emplace_back(index + 0, top_index + 1, index + 1);
                ++index;
                ++top_index;
            }
        } else {
            auto top_index = index + rows + 1;
            faces.emplace_back(index + 0, top_index, top_index + 1);
            ++top_index;
            for (auto x = 0; x < rows; ++x) {
                faces.emplace_back(index + 0, top_index + 0, index + 1);
                faces.emplace_back(index + 1, top_index + 0, top_index + 1);
                ++index;
                ++top_index;
            }
        }

        ++index;
        flipped = !flipped;
    }

//    for (auto face : faces)
//        std::cout << vertices[face.x].x << ", " << vertices[face.x].z << ") == (" << vertices[face.y].x << ", "
//                  << vertices[face.y].z << ") == (" << vertices[face.z].x << ", " << vertices[face.z].z << std::endl;
}
