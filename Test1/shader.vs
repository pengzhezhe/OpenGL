#version 460 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
out vec4 color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	gl_Position = projection * view * (model * vec4(vertex_position,1.0f));
	color=vec4(vertex_color,1.0f);
}