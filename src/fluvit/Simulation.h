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

    /// vector of starting terrain heights in [0, 255]
    std::vector<uint32_t> starting_terrain_height;

    /// r = terrain height b
    /// g = water height d
    /// b = suspended sediment amount s
    /// a = initial terrain height
    GLuint texture1;
    GLuint texture1_swap;
    bool swap_texture1 = false; // if false, then texture1_swap is output
    /// r = right flow
    /// g = top flow
    /// b = left flow
    /// a = bottom flow
    GLuint texture2;
    GLuint texture2_swap;
    bool swap_texture2 = false;
    /// r = water velocity x
    /// g = water velocity y
    /// b = previous timestep sin(local tilt angle) = sin(alpha)
    /// a = nothing
    GLuint texture3;

    GLuint render_texture[2];
    GLuint fbo;
    std::shared_ptr<TextureCombo> input_texture;
    RenderPass *pass;

    GLuint fbos[4];
    GLuint sampler2d;

    std::shared_ptr<TextureCombo> input_texture1;
    std::shared_ptr<TextureCombo> input_texture1_swap;
    std::shared_ptr<TextureCombo> input_texture2;
    std::shared_ptr<TextureCombo> input_texture3;

    RenderDataInput input;
    RenderPass *passes[4];
public:
    explicit Simulation(const std::string &image = "");

    void start() override;

    void update() override;

    /// r = terrain height b
    /// g = water height d
    /// b = suspended sediment amount s
    /// a = initial terrain height
    GLuint get_texture1() const { return swap_texture1 ? texture1 : texture1_swap; }

    /// r = water velocity x
    /// g = water velocity y
    /// b = previous timestep sin(local tilt angle) = sin(alpha)
    /// a = nothing
    GLuint get_texture3() const { return texture3; }

    GLuint get_texture() const { return render_texture[swap_texture2 ? 1 : 0]; }

    GLuint get_sampler() const { return sampler2d; }
};


#endif //FLUVIT_SIMULATION_H
