R"zzz(
#version 330 core
in vec2 pos;
uniform float time;
uniform sampler2D input_texture;
out vec4 fragment_color;

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
    //fragment_color = vec4(sin(time * 0.5 + pos.x + pos.y) * 0.5 + 0.5, 0.0, 0.0, 1.0);
    float height = fract(texture(input_texture, pos).x /*+ 0.01 * rand(pos + 7.0 * time)*/);
    //height = sin(pos.x + pos.y) * 0.5 + 0.5;
    fragment_color = vec4(height, 0.0, 0.0, 1.0);
}
)zzz"
