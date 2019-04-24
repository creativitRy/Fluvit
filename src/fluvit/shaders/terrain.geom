R"zzz(#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
in vec4 vs_light_direction[3];
out vec4 light_direction;
out vec4 world_position;
out vec4 normal;
out float top_height;

void main() {
	int n = 0;
	vec3 a = gl_in[0].gl_Position.xyz;
	vec3 b = gl_in[1].gl_Position.xyz;
	vec3 c = gl_in[2].gl_Position.xyz;
	vec3 u = normalize(b - a);
	vec3 v = normalize(c - a);
	vec4 face_normal = normalize(vec4(normalize(cross(u, v)), 0.0));

	top_height = max(a.y, max(b.y, c.y));

	for (n = 0; n < gl_in.length(); n++) {
		light_direction = normalize(vs_light_direction[n]);
		world_position = gl_in[n].gl_Position;
		normal = face_normal;
		gl_Position = projection * view * model * gl_in[n].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}
)zzz"
