//
// Created by gahwon on 4/16/19.
//

#ifndef GLSL_PERLIN_H
#define GLSL_PERLIN_H


#include <glm/glm.hpp>
#include <random>

class perlin {
public:
    static void init();

    static double noise(int x, int y);
private:
    static constexpr float initial_scale = 0.012f;

    static constexpr uint32_t random_size = 234;
    glm::vec2 random_unit_vectors[random_size];

    explicit perlin(uint32_t seed);

    float octave(int x, int y);

    float noise(glm::vec2 pos);

    glm::vec2 sample(int x, int y);
};


#endif //GLSL_PERLIN_H
