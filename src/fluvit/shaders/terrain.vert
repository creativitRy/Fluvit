R"zzz(
#version 330 core
uniform vec4 light_position;
in vec4 vertex_position;
out vec4 vs_light_direction;
void main() {
	gl_Position = vertex_position;
	vs_light_direction = light_position - gl_Position;
}
)zzz"
