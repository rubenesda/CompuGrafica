#version 330 core

struct Material
{
	sampler2D texture_diffuse;
	sampler2D texture_specular;
	float shininess;
};

struct Light 
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 color;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

//uniform sampler2D texture_diffuse;
//uniform sampler2D texture_specular;


void main()
{
	// ambient
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse, TexCoords));

	//diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse, TexCoords));

	//specular
	
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular, TexCoords));

	vec3 result = ambient + diffuse + specular;
	color = vec4(result, 1.0f); 
};