R"zzz(
#version 330 core
uniform float delta_time;
uniform sampler2D input_texture1;
uniform sampler2D input_texture3;

in vec2 pos;
layout (location = 0) out vec4 output_texture1;

/*
5 steps:

1. Water level increases from various sources
2. Flow is simulated
3. Erosion deposition
4. Suspended sediments transported
5. Water evaporation
*/

float rand(vec2 co){
    // https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    vec4 tex = texture(input_texture1, pos);
    float initial_height = tex.y;
    float height = tex.x;
    float water_height = height + tex.z;
    float sediments_rel_height = tex.a;

    output_texture1 = vec4(height, initial_height, clamp(water_height - height, 0.0, 1.0), sediments_rel_height);
}
)zzz"
