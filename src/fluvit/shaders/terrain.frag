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

	// normal
	float height_left = texture(simulation1, max(vec2(0.0), pos - vec2(grid_delta.x, 0.0))).x;
	float height_right = texture(simulation1, min(vec2(1.0), pos + vec2(grid_delta.x, 0.0))).x;
	float height_top = texture(simulation1, min(vec2(1.0), pos + vec2(0.0, grid_delta.y))).x;
	float height_bottom = texture(simulation1, max(vec2(0.0), pos - vec2(0.0, grid_delta.y))).x;

	float dx = (height_right - height_left) / (2 * grid_delta.x);
	float dy = (height_top - height_bottom) / (2 * grid_delta.y);

	vec4 normal = normalize(vec4(-dx, 1.0f, -dy, 0.0));

	// curvature
	vec4 tex = texture(simulation1, pos);
	float height_center = tex.x;
	float height_top_left = texture(simulation1, vec2(max(0.0, pos.x - grid_delta.x), min(1.0, pos.y + grid_delta.y))).x;
	float height_top_right = texture(simulation1, vec2(min(1.0, pos.x + grid_delta.x), min(1.0, pos.y + grid_delta.y))).x;
	float height_bottom_left = texture(simulation1, vec2(max(0.0, pos.x - grid_delta.x), max(0.0, pos.y - grid_delta.y))).x;
	float height_bottom_right = texture(simulation1, vec2(min(1.0, pos.x + grid_delta.x), max(0.0, pos.y - grid_delta.y))).x;

	float dxx = (height_right + height_left - 2.0 * height_center) / (grid_delta.x * grid_delta.x);
	float dyy = (height_top + height_bottom - 2.0 * height_center) / (grid_delta.y * grid_delta.y);
	float dxy = (height_top_right - height_bottom_right - height_top_left + height_bottom_left) / (4.0 * grid_delta.x * grid_delta.y);

	float ddot = dx * dx + dy * dy;
	float kh = -2.0f * (dy * dy * dxx + dx * dx * dyy - dx * dy * dxy);
	if (ddot > 0.0)
		kh /= dx * dx + dy * dy;
	float kv = -2.0f * (dx * dx * dxx + dy * dy * dyy + dx * dy * dxy);
	if (ddot > 0.0)
		kv /= dx * dx + dy * dy;
	float curve = 0.5 * (kh + kv);
	float neg_curvature = clamp(curve / 10000 + 1.0, 0.0, 1.0);

	// ambient occlusion
	vec3 tangent = normalize(cross(normal.xyz, vec3(0.0, 0.0, 1.0)));
	vec3 bitangent = normalize(cross(tangent, normal.xyz));
	mat3 orthobasis = mat3(tangent, normal.xyz, bitangent);
	float occlusion = 0.0;

	float i;
	for(i = 1; i < 33; i++){
		float s = i / 32.0;
		float a = sqrt(s * 512.0);
		float b = sqrt(s);
		float x = sin(a) * b * 10.0;
		float y = cos(a) * b * 10.0;
		vec3 sample_uv = orthobasis * vec3(x, 0.0, y);
		vec3 sample_pos = vec3(sample_uv.x, texture(simulation1, sample_uv.xz).x, sample_uv.z);
		vec3 sample_dir = normalize(sample_pos - world_position.xyz);
		float lambert = clamp(dot(normal.xyz, sample_dir), 0.0, 1.0);
		float dist_factor = 0.23 / sqrt(length(sample_pos - world_position.xyz));
		occlusion += dist_factor * lambert;
	}
	float incident = 1.0 - occlusion / 32.0;
	incident = pow(incident, 8.0);

	vec3 color = vec3(0.8);
	if (tex.z > 0)
		color.g = 0.0;
	float dot_nl = dot(normalize(light_direction), normal);
	dot_nl = clamp(dot_nl, 0.5, 1.0);
	//dot_nl = clamp(dot_nl * neg_curvature, 0.4, 1.0);
	color = clamp(dot_nl * color, 0.0, 1.0);
	fragment_color = vec4(color, 1.0);
}
)zzz"
