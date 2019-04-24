//
// Created by gahwon on 4/16/19.
//

#include <glm/gtx/transform.hpp>
#include "Chunk.h"
#include "perlin.h"
#include "../common_uniforms.h"

namespace {
// @formatter:off
const char* vertex_shader =
#include "chunk.vert"
;

const char* geometry_shader =
#include "chunk.geom"
;

const char* fragment_shader =
#include "chunk.frag"
;
// @formatter:on
}

Chunk::Chunk(glm::ivec2 position) : position(position), input() {
}

Chunk::~Chunk() {
    if (is_init) {
        // todo
        //delete pass;
    }
}

void Chunk::start() {
    auto start = (int) chunk_size * position;

    for (int y = 0; y < (int) chunk_size; ++y) {
        for (int x = 0; x < (int) chunk_size; ++x) {
            auto noise = perlin::noise(start.x + x, start.y + y);
            data[y][x] = (uint8_t) (noise * 64);
        }
    }

    setup_mesh();

    model = make_uniform("model", (std::function<glm::mat4()>) [this]() {
        return glm::translate((float) chunk_size * glm::vec3(position.x, 0, position.y));
    });

    input.assign(0, "vertex_position", vertices.data(), vertices.size(), 4, GL_FLOAT);
    input.assignIndex(faces.data(), faces.size(), 3);
    pass = new RenderPass(-1,
                          input,
                          {vertex_shader, geometry_shader, fragment_shader},
                          {model, common_uniforms::instance.view, common_uniforms::instance.projection,
                           common_uniforms::instance.light_position},
                          {"fragment_color"}
    );

    is_init = true;
}

void Chunk::render() {
    if (pass->enabled) {
        pass->setup();
        pass->render();
    }
}

uint8_t Chunk::get_height(int x, int z) {
    if (x < 0 || z < 0)
        return 0;
    if (x >= (int) chunk_size || z >= (int) chunk_size)
        return 0;
    return data[z][x];
}

void Chunk::setup_mesh() {
    vertices.reserve(2 * chunk_size * chunk_size);
    faces.reserve(chunk_size * chunk_size * 6 + chunk_size * 4);

    for (int z = 0; z < (int) chunk_size; ++z) {
        for (int x = 0; x < (int) chunk_size; ++x) {
            auto height = get_height(x, z);

            auto vertex_index = vertices.size();
            vertices.emplace_back(x, height, z, 1);
            vertices.emplace_back(x, height, z + 1, 1);
            vertices.emplace_back(x + 1, height, z + 1, 1);
            vertices.emplace_back(x + 1, height, z, 1);

            faces.emplace_back(vertex_index + 0, vertex_index + 1, vertex_index + 3);
            faces.emplace_back(vertex_index + 1, vertex_index + 2, vertex_index + 3);

            if (x == 0) {
                auto heightLeft = get_height(x - 1, z);
                if (height != heightLeft) {
                    auto vi = vertices.size();
                    vertices.emplace_back(x, heightLeft, z, 1);
                    vertices.emplace_back(x, heightLeft, z + 1, 1);

                    if (heightLeft > height) {
                        faces.emplace_back(vertex_index + 0, vi + 0, vertex_index + 1);
                        faces.emplace_back(vi + 0, vi + 1, vertex_index + 1);
                    } else {
                        faces.emplace_back(vi + 1, vertex_index + 1, vi + 0);
                        faces.emplace_back(vertex_index + 1, vertex_index + 0, vi + 0);
                    }
                }
            }

            if (z == 0) {
                auto heightDown = get_height(x, z - 1);
                if (height != heightDown) {
                    auto vi = vertices.size();
                    vertices.emplace_back(x, heightDown, z, 1);
                    vertices.emplace_back(x + 1, heightDown, z, 1);

                    if (heightDown > height) {
                        faces.emplace_back(vertex_index + 3, vi + 1, vertex_index + 0);
                        faces.emplace_back(vi + 1, vi + 0, vertex_index + 0);
                    } else {
                        faces.emplace_back(vi + 0, vertex_index + 0, vi + 1);
                        faces.emplace_back(vertex_index + 0, vertex_index + 3, vi + 1);
                    }
                }
            }

            auto heightRight = get_height(x + 1, z);
            if (height != heightRight) {
                auto vi = vertices.size();
                vertices.emplace_back(x + 1, heightRight, z, 1);
                vertices.emplace_back(x + 1, heightRight, z + 1, 1);

                if (heightRight > height) {
                    faces.emplace_back(vertex_index + 2, vi + 1, vertex_index + 3);
                    faces.emplace_back(vi + 1, vi + 0, vertex_index + 3);
                } else {
                    faces.emplace_back(vi + 0, vertex_index + 3, vi + 1);
                    faces.emplace_back(vertex_index + 3, vertex_index + 2, vi + 1);
                }
            }

            auto heightUp = get_height(x, z + 1);
            if (height != heightUp) {
                auto vi = vertices.size();
                vertices.emplace_back(x, heightUp, z + 1, 1);
                vertices.emplace_back(x + 1, heightUp, z + 1, 1);

                if (heightUp > height) {
                    faces.emplace_back(vertex_index + 1, vi + 0, vertex_index + 2);
                    faces.emplace_back(vi + 0, vi + 1, vertex_index + 2);
                } else {
                    faces.emplace_back(vi + 1, vertex_index + 2, vi + 0);
                    faces.emplace_back(vertex_index + 2, vertex_index + 1, vi + 0);
                }
            }
        }
    }
}
