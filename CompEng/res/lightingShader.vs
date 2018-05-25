#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aNormal; // the color variable has attribute position 1
layout (location = 2) in vec2 aTexCoord;
  
out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

	TexCoord = aTexCoord;
	Normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
	FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
} 