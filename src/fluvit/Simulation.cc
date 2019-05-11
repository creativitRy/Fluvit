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

const char *sim1_shader =
#include "shaders/sim1.frag"
;

const char *sim2_shader =
#include "shaders/sim2.frag"
;

const char *sim3_shader =
#include "shaders/sim3.frag"
;

const char *sim4_shader =
#include "shaders/sim4.frag"
;
// @formatter:on

inline uint32_t convert_rgba(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
    return r | (g << 8u) | (b << 16u) | (a << 24u);
}

}

Simulation::Simulation(const std::string &image) : starting_terrain_height{} {
    if (image.empty()) {
        // procedurally generate perlin terrain
        for (int col = 0; col < height; col++) {
            for (int row = 0; row < width; row++) {
                auto h = (uint32_t) glm::clamp(perlin::noise(row * 0.25f, col * 0.25f) * 128.0 + 32.0, 0.0, 255.0);
                starting_terrain_height.emplace_back(h);
            }
        }
    } else {
        // load image
        if (!pngio::load(image, starting_terrain_height, width, height)) {
            std::cerr << "Illegal png file " << image << std::endl;
            abort();
        }
        for (int col = 0; col < height; col++) {
            for (int row = 0; row < width; row++) {
                auto index = col * width + row;

                auto data = starting_terrain_height[index];
                starting_terrain_height[index] = data & 0xffu;
            }
        }

        std::cout << "Successfully read in heightmap " << image << std::endl;
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

    // output textures
    {
        {
            std::vector<uint32_t> data;
            data.reserve(width * height);
            for (int col = 0; col < height; col++) {
                for (int row = 0; row < width; row++) {
                    auto index = col * width + row;
                    auto h = starting_terrain_height[index] & 0xffu;
                    data.emplace_back(convert_rgba(h, 0, 0, h));
                }
            }

            glGenTextures(1, &texture1);
            glBindTexture(GL_TEXTURE_2D, texture1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glGenTextures(1, &texture1_swap);
            glBindTexture(GL_TEXTURE_2D, texture1_swap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        {
            glGenTextures(1, &texture2);
            glBindTexture(GL_TEXTURE_2D, texture2);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glGenTextures(1, &texture2_swap);
            glBindTexture(GL_TEXTURE_2D, texture2_swap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        {
            glGenTextures(1, &texture3);
            glBindTexture(GL_TEXTURE_2D, texture3);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        std::vector<uint32_t> old_data;
        old_data.reserve(width * height);
        for (int col = 0; col < height; col++) {
            for (int row = 0; row < width; row++) {
                auto index = col * width + row;
                auto data = starting_terrain_height[index];
                auto h = data & 0xffu;
                old_data.emplace_back(convert_rgba(h, h, 0, 0));
            }
        }

        // The texture we're going to render to
        glGenTextures(2, render_texture);
        for (unsigned int texture : render_texture) {
            // "Bind" the newly created texture : all future texture functions will modify this texture
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, old_data.data());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
    }

    // fbos
    {
        {
            glGenFramebuffers(4, fbos);

            glBindFramebuffer(GL_FRAMEBUFFER, fbos[0]);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture1_swap, 0);
            GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
            glDrawBuffers(1, DrawBuffers);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "Framebuffer sim1 error :(" << std::endl;
                abort();
            }

            glBindFramebuffer(GL_FRAMEBUFFER, fbos[1]);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, texture2_swap, 0);
            DrawBuffers[0] = GL_COLOR_ATTACHMENT1;
            glDrawBuffers(1, DrawBuffers);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "Framebuffer sim2 error :(" << std::endl;
                abort();
            }

            glBindFramebuffer(GL_FRAMEBUFFER, fbos[2]);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, texture1, 0);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, texture3, 0);
            GLenum DrawBuffers2[2] = {GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
            glDrawBuffers(2, DrawBuffers2);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "Framebuffer sim3 error :(" << std::endl;
                abort();
            }

            glBindFramebuffer(GL_FRAMEBUFFER, fbos[1]);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture1_swap, 0);
            DrawBuffers[0] = GL_COLOR_ATTACHMENT0;
            glDrawBuffers(1, DrawBuffers);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "Framebuffer sim4 error :(" << std::endl;
                abort();
            }
        }

        // create FBO (off-screen framebuffer)
        glGenFramebuffers(1, &fbo);
        // bind offscreen buffer
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture[swap_texture2 ? 1 : 0], 0);
        // Set the list of draw buffers.
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer error :(" << std::endl;
            abort();
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

    // input textures
    {
        input_texture1 = make_texture("input_texture1", (std::function<uint32_t()>) [this]() {
            return sampler2d;
        }, 0, (std::function<uint32_t()>) [this]() {
            return swap_texture1 ? texture1_swap : texture1;
        });
        input_texture1_swap = make_texture("input_texture1", (std::function<uint32_t()>) [this]() {
            return sampler2d;
        }, 0, (std::function<uint32_t()>) [this]() {
            return swap_texture1 ? texture1 : texture1_swap;
        });
        input_texture2 = make_texture("input_texture2", (std::function<uint32_t()>) [this]() {
            return sampler2d;
        }, 0, (std::function<uint32_t()>) [this]() {
            return swap_texture2 ? texture2_swap : texture2;
        });
        input_texture3 = make_texture("input_texture3", (std::function<uint32_t()>) [this]() {
            return sampler2d;
        }, 0, (std::function<uint32_t()>) [this]() {
            return texture3;
        });

        input_texture = make_texture("input_texture", (std::function<uint32_t()>) [this]() {
            return get_sampler();
        }, 0, (std::function<uint32_t()>) [this]() {
            return render_texture[swap_texture2 ? 0 : 1];
        });
    }

    // render pass
    {
        input.assign(0, "vertex_position", vertices.data(), vertices.size(), 4, GL_FLOAT);
        input.assignIndex(faces.data(), faces.size(), 3);

        passes[0] = new RenderPass(-1,
                                   input,
                                   {vertex_shader, nullptr, sim1_shader},
                                   {common_uniforms::instance.fixed_delta_time, input_texture1},
                                   {"output_texture1"}
        );
        passes[1] = new RenderPass(-1,
                                   input,
                                   {vertex_shader, nullptr, sim2_shader},
                                   {common_uniforms::instance.fixed_delta_time, input_texture1_swap, input_texture2},
                                   {"output_texture2"}
        );
        passes[2] = new RenderPass(-1,
                                   input,
                                   {vertex_shader, nullptr, sim3_shader},
                                   {common_uniforms::instance.fixed_delta_time, input_texture1_swap, input_texture2},
                                   {"output_texture1", "output_texture3"}
        );
        passes[3] = new RenderPass(-1,
                                   input,
                                   {vertex_shader, nullptr, sim4_shader},
                                   {common_uniforms::instance.fixed_delta_time, input_texture1, input_texture3},
                                   {"output_texture1"}
        );

        pass = new RenderPass(-1,
                              input,
                              {vertex_shader, nullptr, sim1_shader},
                              {common_uniforms::instance.fixed_delta_time, input_texture},
                              {"fragment_color"}
        );
    }
}

void Simulation::update() {
    // sim 1
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbos[0]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, swap_texture1 ? texture1 : texture1_swap, 0);

        glViewport(0, 0, width, height);
        glDisable(GL_BLEND); // enable storing stuff in alpha channel
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        passes[0]->setup();
        passes[0]->render();
    }

    // sim 2
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbos[1]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, swap_texture2 ? texture2 : texture2_swap, 0);

        glViewport(0, 0, width, height);
        glDisable(GL_BLEND);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        passes[1]->setup();
        passes[1]->render();

        swap_texture2 = !swap_texture2;
    }

    // sim 3
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbos[2]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, swap_texture1 ? texture1_swap : texture1, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, texture3, 0);

        glViewport(0, 0, width, height);
        glDisable(GL_BLEND); // enable storing stuff in alpha channel
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        passes[2]->setup();
        passes[2]->render();
    }

    // sim 4
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbos[3]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, swap_texture1 ? texture1 : texture1_swap, 0);

        glViewport(0, 0, width, height);
        glDisable(GL_BLEND); // enable storing stuff in alpha channel
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        passes[3]->setup();
        passes[3]->render();
    }

    swap_texture1 = !swap_texture1;

    // Render to our framebuffer
    /*glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // Set render_texture as our color attachment #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture[swap_texture2 ? 1 : 0], 0);

    glViewport(0, 0, width, height);
    glDisable(GL_BLEND); // enable storing stuff in alpha channel
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    pass->setup();
    pass->render();

    swap_texture2 = !swap_texture2;*/
}
