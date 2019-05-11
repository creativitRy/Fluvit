R"zzz(
#version 330 core

uniform float min_raindrop_radius;
uniform float max_raindrop_radius;
uniform float min_raindrop_amount;
uniform float max_raindrop_amount;

uniform float time;
uniform float delta_time;
uniform sampler2D input_texture1;

in vec2 pos;
layout (location = 0) out vec4 output_texture1;

/*
1. Water level increases from various sources
*/

// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
float rand(float n) {
    return fract(sin(n) * 43758.5453123);
}

vec2 rand2(float n) {
    return vec2(rand(0.9 * n + 0.2), rand(0.7 * n + 0.3));
}

void main() {
    vec4 tex = texture(input_texture1, pos);

    vec2 rand_pos = rand2(time);
    float rand_radius = rand(time * 0.11 + 0.13) * (max_raindrop_radius - min_raindrop_radius) + min_raindrop_radius;
    float rand_amount = rand(time * 0.17 + 0.23) * (max_raindrop_amount - min_raindrop_amount) + min_raindrop_amount;
    vec2 delta = pos - rand_pos;

    float d = tex.y;
    if (dot(delta, delta) <= rand_radius * rand_radius)
        d += rand_amount * delta_time;

    output_texture1 = vec4(tex.x, min(d, 1.0), tex.z, tex.w);
}
)zzz"
