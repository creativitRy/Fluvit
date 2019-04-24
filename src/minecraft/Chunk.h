//
// Created by gahwon on 4/16/19.
//

#ifndef GLSL_CHUNK_H
#define GLSL_CHUNK_H

#include "../render_pass.h"
#include "../Entity.h"
#include "../shader_uniform.h"

class Chunk : public Entity {
public:
    static const uint32_t chunk_size = 16;

    glm::ivec2 position;

    explicit Chunk(glm::ivec2 position);

    ~Chunk() override;

    void start() override;

    void render() override;

    uint8_t get_height(int x, int z);
private:
    bool is_init;

    uint8_t data[chunk_size][chunk_size];

    std::vector<glm::vec4> vertices;
    std::vector<glm::uvec3> faces;

    std::shared_ptr<ShaderUniformBase> model;

    RenderDataInput input;
    RenderPass *pass;

    void setup_mesh();
};


#endif //GLSL_CHUNK_H
