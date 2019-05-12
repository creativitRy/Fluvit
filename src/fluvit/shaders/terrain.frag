R"zzz(
#version 330 core
in vec4 normal;
in vec4 light_direction;
in vec4 world_position;

uniform sampler2D simulation1;
uniform sampler2D simulation3;

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
	vec3 color = vec3(texture(simulation3, world_position.xz).z);
	if (texture(simulation1, world_position.xz).z > 0)
		color.g = 0.0;
	float dot_nl = dot(normalize(light_direction), normalize(normal));
	dot_nl = clamp(dot_nl, 0.5, 1.0);
	color = clamp(dot_nl * color, 0.0, 1.0);
	fragment_color = vec4(color, 1.0);
}
)zzz"
