R"zzz(
#version 330 core
uniform float grid_distance_x;
uniform float grid_distance_y;

uniform vec2 grid_delta;

uniform float delta_time;
uniform sampler2D input_texture1;
uniform sampler2D input_texture2;

in vec2 pos;
layout (location = 0) out vec4 output_texture1;
layout (location = 1) out vec4 output_texture3;

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
    float b_prev = tex.x;
    float d_prev = tex.y;
    float s_prev = tex.z;

    vec4 flow_center = texture(input_texture2, pos);
    float flow_center_sum = flow_center.x + flow_center.y + flow_center.z + flow_center.w;

    float laplacian_estimate = texture(input_texture2, max(vec2(0.0), pos - vec2(grid_delta.x, 0.0))).x
        + texture(input_texture2, max(vec2(0.0), pos - vec2(0.0, grid_delta.y))).y
        + texture(input_texture2, min(vec2(1.0), pos + vec2(grid_delta.x, 0.0))).z
        + texture(input_texture2, min(vec2(1.0), pos + vec2(0.0, grid_delta.y))).w
        - flow_center_sum;

    float d_next = d_prev + delta_time / grid_distance_x / grid_distance_y * laplacian_estimate;

    output_texture1 = vec4(b_prev, d_next, s_prev, tex.w);
    output_texture3 = vec4(1.0);
}
)zzz"
