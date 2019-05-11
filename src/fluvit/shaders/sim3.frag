R"zzz(
#version 330 core
uniform float grid_distance_x;
uniform float grid_distance_y;

uniform vec2 grid_delta;

uniform float sediment_capacity;
uniform float dissolving_constant;
uniform float deposition_constant;

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
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec4 tex = texture(input_texture1, pos);
    float b_prev = tex.x;
    float d_prev = tex.y;
    float s_prev = tex.z;

    vec4 flow_center = texture(input_texture2, pos);
    float flow_center_sum = flow_center.x + flow_center.y + flow_center.z + flow_center.w;

    vec4 flow_left = texture(input_texture2, max(vec2(0.0), pos - vec2(grid_delta.x, 0.0)));
    vec4 flow_right = texture(input_texture2, min(vec2(1.0), pos + vec2(grid_delta.x, 0.0)));
    vec4 flow_top = texture(input_texture2, min(vec2(1.0), pos + vec2(0.0, grid_delta.y)));
    vec4 flow_bottom = texture(input_texture2, max(vec2(0.0), pos - vec2(0.0, grid_delta.y)));

    float laplacian_estimate = flow_left.x + flow_bottom.y + flow_right.z + flow_top.w - flow_center_sum;

    float d_next = d_prev + delta_time / grid_distance_x / grid_distance_y * laplacian_estimate;

    vec2 velocity = vec2(flow_left.x - flow_center.z + flow_center.x - flow_right.z, flow_bottom.y - flow_center.w + flow_center.y - flow_top.w);
    if (d_next + d_prev > 0)
        velocity = velocity / (d_next + d_prev) / vec2(grid_distance_y, grid_distance_x);
    else
        velocity = vec2(0.0);

    float dbdx = (texture(input_texture1, min(vec2(1.0), pos + vec2(grid_delta.x, 0.0))).x
        - texture(input_texture1, max(vec2(0.0), pos - vec2(grid_delta.x, 0.0))).x) / (2 * grid_delta.x);
    float dbdy = (texture(input_texture1, min(vec2(1.0), pos + vec2(0.0, grid_delta.y))).x
        - texture(input_texture1, max(vec2(0.0), pos - vec2(0.0, grid_delta.y))).x) / (2 * grid_delta.y);
    float db_dot = dot(vec2(dbdx, dbdy), vec2(dbdx, dbdy));

    float sin_alpha = sqrt(db_dot / (1 + db_dot));

    float c = sediment_capacity * sin_alpha * sqrt(dot(velocity, velocity));

    float b_next;
    float s_next;
    if (c > s_prev) {
        float delta = dissolving_constant * (c - s_prev);
        b_next = b_prev - delta;
        s_next = s_prev + delta;
    } else {
        float delta = deposition_constant * (s_prev - c);
        b_next = b_prev + delta;
        s_next = s_prev - delta;
    }

    output_texture1 = vec4(b_next, d_next, s_next, tex.w);
    output_texture3 = vec4(clamp(pos - delta_time * velocity, vec2(0.0), vec2(1.0)), 0.0, 0.0);
}
)zzz"
