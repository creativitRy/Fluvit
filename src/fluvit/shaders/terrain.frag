R"zzz(
#version 330 core
in vec4 normal;
in vec4 light_direction;
in vec4 world_position;
in float top_height;

uniform sampler2D simulation;

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
	vec4 pos = world_position;
	float check_width = 5.0;
	float i = floor(pos.x / check_width);
	float j  = floor(pos.z / check_width);

	float pixel_noise = noise(vec3(world_position));
	vec3 color = vec3(normal);
	if (world_position.y < 64.01)
		color = vec3(219, 211, 160) / 256.0;
	else {
		if (world_position.y < 90.01)
			color = vec3(69, 110, 51) / 256.0;
		else
			color = vec3(180) / 256.0;

		if (top_height - world_position.y > 0.4375)
			color = vec3(134, 96, 67) / 256.0;
        else if (top_height - world_position.y > 0.375 && pixel_noise > 0.5)
            color = vec3(134, 96, 67) / 256.0;
	}
	if (top_height - world_position.y > 3)
		color = vec3(123) / 256.0;

	color *= (pixel_noise) * 0.25 + 0.75;

	float dot_nl = dot(normalize(light_direction), normalize(normal));
	dot_nl = clamp(dot_nl, 0.5, 1.0);
	color = clamp(dot_nl * color, 0.0, 1.0);
	color.x = texture(simulation, vec2(i, j)).x;
	fragment_color = vec4(color, 1.0);
}
)zzz"
