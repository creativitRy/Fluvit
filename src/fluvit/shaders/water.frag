R"zzz(
#version 330 core
in vec4 light_direction;
in vec4 world_position;

uniform sampler2D simulation1;
uniform sampler2D simulation3;
uniform vec2 grid_delta;

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
	vec2 pos = world_position.xz;
	float height_left = texture(simulation1, max(vec2(0.0), pos - vec2(grid_delta.x, 0.0))).x;
	float height_right = texture(simulation1, min(vec2(1.0), pos + vec2(grid_delta.x, 0.0))).x;
	float height_top = texture(simulation1, min(vec2(1.0), pos + vec2(0.0, grid_delta.y))).x;
	float height_bottom = texture(simulation1, max(vec2(0.0), pos - vec2(0.0, grid_delta.y))).x;

	float dx = (height_right - height_left) / (2 * grid_delta.x);
	float dy = (height_top - height_bottom) / (2 * grid_delta.y);

	vec4 normal = vec4(-dx, 1.0f, -dy, 0.0);

	vec3 color = vec3(0.4, 0.4 + (texture(simulation1, world_position.xz).z > 0 ? 0.6 : 0.0), 1.0);
	float dot_nl = dot(normalize(light_direction), normalize(normal));
	dot_nl = clamp(dot_nl, 0.5, 1.0);
	color = clamp(dot_nl * color, 0.0, 1.0);
	fragment_color = vec4(color, 0.8);
}
)zzz"
