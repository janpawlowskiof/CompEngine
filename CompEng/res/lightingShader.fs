#version 330 core

struct Material {
    vec3 specular;
    float shininess;
	sampler2D diffuseMap;
	sampler2D specularMap;
};

struct DirectionalLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
  
	float constant;
    float linear;
    float quadratic; 

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define MAX_POINT_LIGHTS 16
#define MAX_SPOT_LIGHTS 16

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 cameraPos;
uniform Material material;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int activePointLights = 0;

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuseMap, TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuseMap, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specularMap, TexCoord));
    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(Normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

	vec3 ambient  = light.ambient  * vec3(texture(material.diffuseMap, TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuseMap, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specularMap, TexCoord));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 result = CalculateDirectionalLight(directionalLight, Normal, viewDir);

	for(int i = 0; i < activePointLights; ++i)
		result+=CalculatePointLight(pointLights[i], viewDir);

	FragColor = vec4(result, 1.0);
}