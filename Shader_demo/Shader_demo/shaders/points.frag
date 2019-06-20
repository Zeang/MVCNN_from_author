#version 330 core
out vec4 Color;
  
smooth in vec4 color;

void main()
{
	Color = color;
}