#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D ourTexture;
uniform bool useTexture = true;
uniform vec3 lightPos;
uniform vec3 cameraPos;

struct Material {
    vec3 specular;
    float shininess;
};
uniform sampler2D diffuseMap;

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform Material material;

void main()
{
	//diffuse
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 norm = normalize(Normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * vec3(texture(diffuseMap, TexCoord));

	//ambient
	vec3 ambient = light.ambient * vec3(texture(diffuseMap, TexCoord));

	//specular
	vec3 cameraDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(cameraDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.ambient * spec * material.specular;

	vec3 result = (ambient + diffuse + specular);
	FragColor = vec4(result, 1.0);
	//FragColor = texture(diffuseMap, TexCoord);
}