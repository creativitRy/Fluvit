//
// Created by gahwon on 4/16/19.
//

#ifndef GLSL_COMMON_UNIFORMS_H
#define GLSL_COMMON_UNIFORMS_H

class common_uniforms {
public:
    static common_uniforms instance;

    std::shared_ptr<ShaderUniformBase> view;
    std::shared_ptr<ShaderUniformBase> camera_position;
    std::shared_ptr<ShaderUniformBase> projection;
    std::shared_ptr<ShaderUniformBase> light_position;
    std::shared_ptr<ShaderUniformBase> time;
    std::shared_ptr<ShaderUniformBase> fixed_delta_time;
    std::shared_ptr<ShaderUniformBase> fixed_time;
    std::shared_ptr<ShaderUniformBase> draw_mode;
};

#endif //GLSL_COMMON_UNIFORMS_H
