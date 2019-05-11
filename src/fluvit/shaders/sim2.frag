R"zzz(
#version 330 core
uniform float delta_time;
uniform sampler2D input_texture1;
uniform sampler2D input_texture2;

uniform float gravity;
uniform float area_over_len;
uniform float grid_distance_x;
uniform float grid_distance_y;

uniform vec2 grid_delta;

in vec2 pos;
layout (location = 1) out vec4 output_texture2;

/*
2.1. Flow is simulated
*/

void main() {
    vec4 center_tex = texture(input_texture1, pos);
    float center = center_tex.x + center_tex.y;
    vec4 left_tex = texture(input_texture1, max(vec2(0.0), pos - vec2(grid_delta.x, 0.0)));
    float left = left_tex.x + left_tex.y;
    vec4 right_tex = texture(input_texture1, min(vec2(1.0), pos + vec2(grid_delta.x, 0.0)));
    float right = right_tex.x + right_tex.y;
    vec4 top_tex = texture(input_texture1, min(vec2(1.0), pos + vec2(0.0, grid_delta.y)));
    float top = top_tex.x + top_tex.y;
    vec4 bottom_tex = texture(input_texture1, max(vec2(0.0), pos - vec2(0.0, grid_delta.y)));
    float bottom = bottom_tex.x + bottom_tex.y;

    vec4 f_prev = texture(input_texture2, pos);
    vec4 height_difference = vec4(center - right, center - top, center - left, center - bottom);
    vec4 f_next = max(vec4(0.0), f_prev + area_over_len * gravity * height_difference);
    float k = min(1.0, center_tex.y * grid_distance_x * grid_distance_y / (f_next.x + f_next.y + f_next.z + f_next.w) / delta_time);

    output_texture2 = f_next * k;
}
)zzz"
