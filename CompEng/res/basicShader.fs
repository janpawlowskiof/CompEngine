#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform bool useTexture = true;
uniform vec3 color;

void main()
{
	if(useTexture)
		FragColor = texture(ourTexture, TexCoord);
	else
		FragColor = vec4(color, 1.0);
}