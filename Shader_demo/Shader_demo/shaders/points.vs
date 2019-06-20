#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 pointColor;

uniform mat4 model, view, projection;

smooth out vec4 color;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    color = vec4(pointColor, 1.0f);
}