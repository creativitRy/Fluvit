R"zzz(
#version 330 core
uniform float evaporation_constant;

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
    vec4 tex1 = texture(input_texture1, pos);
    vec4 tex3 = texture(input_texture3, pos);

    float b = tex1.x;
    float s = texture(input_texture1, tex3.xy).z;
    float d = tex1.y * (1 - delta_time * evaporation_constant);
    if (d <= 0.001) {
        d = 0.0;
        b += s;
        s = 0.0;
    }

    output_texture1 = vec4(b, d, s, tex1.w);
}
)zzz"
