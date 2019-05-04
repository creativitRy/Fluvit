R"zzz(
#version 330 core
in vec4 vertex_position;
out vec4 pos;
void main() {
    gl_Position = 2.0 * vertex_position - 1.0;
    pos = vertex_position;
}
)zzz"
