#version 330 core
out vec4 FragColor;

in vec3 Color;

// texture samplers

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = vec4(Color, 1);
}