#version 330 core
out vec4 FragColor;

struct DirLight {
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

struct Material 
{
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};
  
in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float aMix;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform PointLight pointLights[2];
uniform PointLight lasers[64];
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform bool isCube;
uniform Material material;
uniform int numPointLights;
uniform int numLasers;
uniform float lightIntensity;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec4 texColor = texture(material.diffuse, TexCoord);

    // directional lighting
    vec3 result = vec3(0.0);
    result = CalcDirLight(dirLight, norm, viewDir);

    // point light
    for (int i = 0; i < numPointLights; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir) * vec3(lightIntensity);

    // laser light
    for (int i = 0; i < numLasers; i++)
        result += CalcPointLight(lasers[i], norm, FragPos, viewDir);
    
    //FragColor = vec4(finalColor, 1.0);
    
    FragColor = vec4(result, texColor.a);
    //FragColor = vec4(0.4,0.2,0.2, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.5);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * material.specular;
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = max(dot(viewDir, reflectDir), 0.0);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
    //return ambient;

}