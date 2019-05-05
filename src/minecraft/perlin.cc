//
// Created by gahwon on 4/16/19.
//

#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include "perlin.h"

namespace {
    perlin *instance;
}

void perlin::init() {
    if (instance == nullptr)
        instance = new perlin(time(nullptr));
}

double perlin::noise(float x, float y) {
    auto octave = 0.5 * instance->octave(x, y) + 0.5;
    return octave * octave * octave + 0.05;
}

perlin::perlin(uint32_t seed) : random_unit_vectors() {
    for (int i = 0; i < (int) random_size; ++i) {
        auto angle = i * glm::two_pi<float>() / random_size;
        random_unit_vectors[i] = glm::vec2(glm::cos(angle), glm::sin(angle));
    }

    std::shuffle(std::begin(random_unit_vectors), std::begin(random_unit_vectors) + random_size,
                 std::default_random_engine(seed));
}

float perlin::octave(float x, float y) {
    auto pos = initial_scale * glm::vec2(x, y);
    return noise(pos * 0.8f) + 0.5f * noise(pos * 2.0f) + 0.25f * noise(pos * 4.0f) + 0.125f * noise(pos * 8.0f);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ArgumentSelectionDefectsInspection"

float perlin::noise(glm::vec2 pos) {
    auto floor = glm::floor(pos);
    auto ifloor = glm::ivec2(floor);
    auto frac = pos - floor;

    auto a = glm::dot(glm::vec2(frac.x, frac.y), sample(ifloor.x, ifloor.y));
    auto b = glm::dot(glm::vec2(1 - frac.x, frac.y), sample(ifloor.x + 1, ifloor.y));
    auto c = glm::dot(glm::vec2(1 - frac.x, 1 - frac.y), sample(ifloor.x + 1, ifloor.y + 1));
    auto d = glm::dot(glm::vec2(frac.x, 1 - frac.y), sample(ifloor.x, ifloor.y + 1));

    return glm::mix(glm::mix(a, b, frac.x), glm::mix(d, c, frac.x), frac.y) * 0.5f + 0.5f;
}

#pragma clang diagnostic pop

union converter {
    uint64_t unsign;
    int64_t sign;

    converter(int64_t s) : sign(s) {}
};

glm::vec2 perlin::sample(int x, int y) {

    // pseudo random number generation algorithm I found online
    // https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
    auto rand = glm::fract(glm::sin(glm::dot(glm::vec2(x, y), glm::vec2(12.9898,78.233))) * 43758.5453);

    // uint64_t pos = ((converter(y).unsign << 32UL) | converter(x).unsign);
    // pos = (pos << 7UL) | (pos >> (64UL - 7UL));
    // uint64_t random_value = 6364136223846793005UL * pos + 1442695040888963407UL;
    return random_unit_vectors[(int) (rand * random_size)];
}
