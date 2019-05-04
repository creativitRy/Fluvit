//
// Created by gahwon on 5/2/19.
//

#ifndef FLUVIT_SIMULATION_H
#define FLUVIT_SIMULATION_H


#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include "../Entity.h"
#include "../render_pass.h"

class Simulation : public Entity {
    int width = 1024;
    int height = 1024;

    std::vector<glm::vec4> vertices;
    std::vector<glm::uvec3> faces;

    RenderDataInput input;
    RenderPass *pass;

    GLuint fbo;
    GLuint renderedTexture;
    GLuint sampler2d;
public:
    void start() override;

    void update() override;

    GLuint get_fbo() const { return fbo; }
    GLuint get_sampler() const { return sampler2d; }
};


#endif //FLUVIT_SIMULATION_H
