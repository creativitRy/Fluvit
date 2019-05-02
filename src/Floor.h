//
// Created by gahwon on 4/16/19.
//

#ifndef GLSL_FLOOR_H
#define GLSL_FLOOR_H

#include "Entity.h"
#include "render_pass.h"

class Floor : public Entity {
public:
    void start() override;

    void render() override;

    ~Floor() override;

    void setCameraPos(const std::function<glm::vec3()> &cameraPos) {
        camera_pos = cameraPos;
    }

private:
    const float kFloorXMin = -1000.0f;
    const float kFloorXMax = 1000.0f;
    const float kFloorZMin = -1000.0f;
    const float kFloorZMax = 1000.0f;
    const float kFloorY = -2;

    std::vector<glm::vec4> vertices;
    std::vector<glm::uvec3> faces;

    std::function<glm::vec3()> camera_pos;

    std::shared_ptr<ShaderUniformBase> model;

    RenderDataInput input;
    RenderPass *pass;
};


#endif //GLSL_FLOOR_H
