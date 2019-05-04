//
// Created by gahwon on 5/2/19.
//

#include "Simulation.h"
#include "../common_uniforms.h"
#include <GL/glew.h>
#include <iostream>
#include <stdlib.h>

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

void Simulation::start() {
    // simple quad that covers fbo
    {
        vertices.emplace_back(0, 0, 0, 1);
        vertices.emplace_back(0, 1, 0, 1);
        vertices.emplace_back(1, 1, 0, 1);
        vertices.emplace_back(1, 0, 0, 1);
        faces.emplace_back(0, 1, 3);
        faces.emplace_back(1, 2, 3);
    }

    // fbo setup
    {
        // create FBO (off-screen framebuffer)
        glGenFramebuffers(1, &fbo);
        // bind offscreen buffer
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // The texture we're going to render to
        glGenTextures(1, &renderedTexture);
        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, renderedTexture);
        // Give an empty image to OpenGL ( the last "0" )
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        // Poor filtering. Needed !
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // Set "renderedTexture" as our color attachement #0
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
        // Set the list of draw buffers.
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer error :(" << std::endl;
        }

        CHECK_GL_ERROR(glGenSamplers(1, &sampler2d));
        CHECK_GL_ERROR(glSamplerParameteri(sampler2d, GL_TEXTURE_WRAP_S, GL_REPEAT));
        CHECK_GL_ERROR(glSamplerParameteri(sampler2d, GL_TEXTURE_WRAP_T, GL_REPEAT));
        CHECK_GL_ERROR(glSamplerParameteri(sampler2d, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        CHECK_GL_ERROR(glSamplerParameteri(sampler2d, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    }

    // render pass
    {
        input.assign(0, "vertex_position", vertices.data(), vertices.size(), 4, GL_FLOAT);
        input.assignIndex(faces.data(), faces.size(), 3);
        pass = new RenderPass(-1,
                              input,
                              {vertex_shader, nullptr, fragment_shader},
                              {common_uniforms::instance.time},
                              {"fragment_color"}
        );
    }
}

void Simulation::update() {
    if (pass->enabled) {
        // Render to our framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        glClear(GL_COLOR_BUFFER_BIT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        pass->setup();
        pass->render();
    }
}
