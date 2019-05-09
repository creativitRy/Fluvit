R"zzz(
#version 330 core
uniform float fixed_time;
uniform sampler2D input_texture;

in vec2 pos;
out vec4 fragment_color;

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

float noise(vec3 pos) {
    float a = rand(vec2(ivec2(vec2(pos.x, pos.z) * 16 + vec2(0.01))) / 16.0);
    float b = rand(vec2(a, float(int(pos.y * 16 + 0.01)) / 16.0));
    return b;
}

void main() {
    vec4 tex = texture(input_texture, pos);
    float initial_height = tex.y;
    float height = tex.x;
    float water_height = height + tex.z;
    float sediments_rel_height = tex.a;

    fragment_color = vec4(height, initial_height, clamp(water_height - height, 0.0, 1.0), 1.0);
}
)zzz"
