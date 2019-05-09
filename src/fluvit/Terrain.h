//
// Created by gahwon on 4/24/19.
//

#ifndef FLUVIT_TERRAIN_H
#define FLUVIT_TERRAIN_H


#include <vector>
#include <glm/glm.hpp>
#include "../Entity.h"
#include "../shader_uniform.h"
#include "../render_pass.h"
#include "Simulation.h"

class Terrain : public Entity {
public:
    explicit Terrain(Simulation *simulation);

    void start() override;

    void render() override;
private:
    static constexpr uint32_t num_rows = 256;
    static constexpr uint32_t num_cols = 256;

    Simulation *simulation;

    std::vector<glm::vec4> vertices;
    std::vector<glm::uvec3> faces;

    std::shared_ptr<ShaderUniformBase> model;
    std::shared_ptr<TextureCombo> sim_texture;

    RenderDataInput input;
    RenderPass *terrain_pass;
    RenderPass *water_pass;

    void init_terrain(int rows, int cols);
};


#endif //FLUVIT_TERRAIN_H
