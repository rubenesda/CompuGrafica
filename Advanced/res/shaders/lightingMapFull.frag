#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;


struct Material
{	
	sampler2D texture_diffuse;
	sampler2D texture_normal;
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
//in vec3 FragPos;
//in vec3 Normal;
//in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

//uniform sampler2D texture_diffuse;
//uniform sampler2D texture_specular;


void main()
{
	
	
	// ambient
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse, fs_in.TexCoords));

	//diffuse
	vec3 norm = normalize(texture(material.texture_normal, fs_in.TexCoords) * 2.0 - 1.0);
	vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse, fs_in.TexCoords));

	//specular
	
	vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 halfwayDir = normalize(reflecDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular, TexCoords));

	vec3 result = ambient + diffuse + specular;
	color = vec4(result, 1.0f); 
};