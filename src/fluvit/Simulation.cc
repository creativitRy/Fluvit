//
// Created by gahwon on 5/2/19.
//

#include "Simulation.h"
#include "../common_uniforms.h"
#include <GL/glew.h>
#include <iostream>
#include <stdlib.h>
#include "../minecraft/perlin.h"
#include "pngio.h"
#include "../time.h"

namespace {
// @formatter:off
const char *vertex_shader =
#include "shaders/sim.vert"
;

const char *fragment_shader =
#include "shaders/sim.frag"
;
// @formatter:on
}

Simulation::Simulation(const std::string &image) {
    if (image.empty()) {
        // procedurally generate perlin terrain
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                auto h = (uint32_t) glm::clamp(perlin::noise(col * 0.25f, row * 0.25f) * 128.0 + 32.0, 0.0, 255.0);
                starting_terrain_data.emplace_back(h | (h << 8u) | (0u << 16u) | (0u << 24u));
            }
        }
    } else {
        // load image
        if (!pngio::load(image, starting_terrain_data, width, height)) {
            std::cerr << "Illegal png file " << image << std::endl;
            abort();
        }
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                auto data = starting_terrain_data[row * width + height];
                auto h = data & 0xffu;
                starting_terrain_data.emplace_back(h | (h << 8u) | (0u << 16u) | (0u << 24u));
            }
        }
    }
}

void Simulation::start() {
    // simple quad that covers fbo
    {
        vertices.emplace_back(0, 0, 0, 1);
        vertices.emplace_back(0, 1, 0, 1);
        vertices.emplace_back(1, 1, 0, 1);
        vertices.emplace_back(1, 0, 0, 1);
        faces.emplace_back(0, 3, 1);
        faces.emplace_back(3, 2, 1);
    }

    // fbo setup
    {
        // create FBO (off-screen framebuffer)
        glGenFramebuffers(1, &fbo);
        // bind offscreen buffer
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // The texture we're going to render to
        glGenTextures(2, render_texture);
        for (unsigned int texture : render_texture) {
            // "Bind" the newly created texture : all future texture functions will modify this texture
            glBindTexture(GL_TEXTURE_2D, texture);
            // Give an empty image to OpenGL ( the last "0" )
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         starting_terrain_data.data());
            // Poor filtering. Needed !
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture[output_to_second_texture ? 1 : 0], 0);
        // Set the list of draw buffers.
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer error :(" << std::endl;
        }
    }

    // sampler
    {
        CHECK_GL_ERROR(glGenSamplers(1, &sampler2d));
        CHECK_GL_ERROR(glSamplerParameteri(sampler2d, GL_TEXTURE_WRAP_S, GL_CLAMP));
        CHECK_GL_ERROR(glSamplerParameteri(sampler2d, GL_TEXTURE_WRAP_T, GL_CLAMP));
        CHECK_GL_ERROR(glSamplerParameteri(sampler2d, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        CHECK_GL_ERROR(glSamplerParameteri(sampler2d, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    }

    // input texture
    {
        input_texture = make_texture("input_texture", (std::function<uint32_t()>) [this]() {
            return get_sampler();
        }, 0, (std::function<uint32_t()>) [this]() {
            return render_texture[output_to_second_texture ? 0 : 1];
        });
    }

    // render pass
    {
        input.assign(0, "vertex_position", vertices.data(), vertices.size(), 4, GL_FLOAT);
        input.assignIndex(faces.data(), faces.size(), 3);
        pass = new RenderPass(-1,
                              input,
                              {vertex_shader, nullptr, fragment_shader},
                              {common_uniforms::instance.fixed_time, input_texture},
                              {"fragment_color"}
        );
    }
}

void Simulation::update() {
    if (pass->enabled) {
        // Render to our framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        // Set render_texture as our color attachment #0
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture[output_to_second_texture ? 1 : 0], 0);

        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        glClear(GL_COLOR_BUFFER_BIT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        pass->setup();
        pass->render();

        output_to_second_texture = !output_to_second_texture;
    }
}
