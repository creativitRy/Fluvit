R"zzz(
#version 330 core
in vec4 vertex_position;
uniform vec4 light_position;
uniform sampler2D simulation;
out vec4 vs_light_direction;
void main() {
	vec4 pos = vertex_position;
	pos.y = texture(simulation, vec2(pos.x, pos.z)).x * 0.75;
	gl_Position = pos;
	vs_light_direction = light_position - pos;
}
)zzz"
