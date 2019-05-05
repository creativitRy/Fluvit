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

    /// vector of size height * width with r | g << 8 | b << 16 | a << 24 in that order
    /// r = terrain height
    /// g = original terrain height
    /// b = water height
    /// a = dissolved soil in water
    std::vector<uint32_t> starting_terrain_data;

    std::vector<glm::vec4> vertices;
    std::vector<glm::uvec3> faces;

    bool output_to_second_texture = false;
    GLuint fbo;
    GLuint render_texture[2];
    GLuint sampler2d;

    std::shared_ptr<TextureCombo> input_texture;

    RenderDataInput input;
    RenderPass *pass;
public:
    explicit Simulation(int width = 1024, int height = 1024, const std::string& image = "");

    void start() override;

    void update() override;

    GLuint get_texture() const { return render_texture[output_to_second_texture ? 1 : 0]; }
    GLuint get_sampler() const { return sampler2d; }
};


#endif //FLUVIT_SIMULATION_H
