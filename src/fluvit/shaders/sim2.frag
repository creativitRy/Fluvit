R"zzz(
#version 330 core
uniform float delta_time;
uniform sampler2D input_texture1;
uniform sampler2D input_texture2;

in vec2 pos;
layout (location = 1) out vec4 output_texture2;

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
    output_texture2 = texture(input_texture2, pos);
}
)zzz"
