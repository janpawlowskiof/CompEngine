#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform bool useTexture = true;
uniform vec3 lightColor;

void main()
{
	FragColor = vec4(lightColor, 1.0f);
}