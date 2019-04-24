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

class Terrain : public Entity {
public:
    void start() override;

    void render() override;
private:
    static constexpr uint32_t num_rows = 256;
    static constexpr uint32_t num_cols = 256;

    std::vector<glm::vec4> vertices;
    std::vector<glm::uvec3> faces;

    std::shared_ptr<ShaderUniformBase> model;

    RenderDataInput input;
    RenderPass *pass;

    void init_terrain(int rows, int cols);
};


#endif //FLUVIT_TERRAIN_H
