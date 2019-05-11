R"zzz(#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform sampler2D simulation1;
uniform vec4 light_position;

out vec4 light_direction;
out vec4 world_position;
out vec4 normal;
out float top_height;

void main() {
	float global_height_scale = 0.5;

	vec3 positions[3];
	positions[0] = gl_in[0].gl_Position.xyz;
	positions[0].y = texture(simulation1, vec2(positions[0].x, positions[0].z)).x * global_height_scale;
	positions[1] = gl_in[1].gl_Position.xyz;
	positions[1].y = texture(simulation1, vec2(positions[1].x, positions[1].z)).x * global_height_scale;
	positions[2] = gl_in[2].gl_Position.xyz;
	positions[2].y = texture(simulation1, vec2(positions[2].x, positions[2].z)).x * global_height_scale;

	vec3 u = normalize(positions[1] - positions[0]);
	vec3 v = normalize(positions[2] - positions[0]);
	vec4 face_normal = normalize(vec4(normalize(cross(u, v)), 0.0));

	int n = 0;
	for (n = 0; n < gl_in.length(); n++) {
		vec4 pos = vec4(positions[n], 1);

		light_direction = normalize(light_position - pos);
		world_position = pos;
		normal = face_normal;
		gl_Position = projection * view * model * pos;
		EmitVertex();
	}
	EndPrimitive();
}
)zzz"
