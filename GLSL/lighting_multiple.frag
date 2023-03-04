#version 330 core
out vec4 FragColor;

uniform vec3 viewPos; 

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

struct Material {
    sampler2D   texture_diffuse1;
    sampler2D   texture_specular1;
    float       shininess;
}; 

uniform Material material;


struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;

struct PointLight {    
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 attenuationCoefficients;
};  

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 attenuationCoefficients;
};

uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir); 
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir);


void main()
{
    vec3 viewDir    = normalize(viewPos - FragPos);
    vec3 normal     = normalize(Normal);
    // define an output color value
    vec3 output = vec3(0.0);
    // add the directional light's contribution to the output
    output += CalcDirLight(dirLight, normal, viewDir);
    // do the same for all point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
  	    output += CalcPointLight(pointLights[i], normal, FragPos, viewDir);
    // and add others lights as well (like spotlights)
  	output += CalcSpotLight(spotLight, normal, FragPos, viewDir);
  
    FragColor = vec4(output, 1.0);
}  

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 0;
    if(light.attenuationCoefficients.x > 0 || light.attenuationCoefficients.y > 0 || light.attenuationCoefficients.z > 0) {
        attenuation = 1.0 / (light.attenuationCoefficients.x + light.attenuationCoefficients.y * distance + light.attenuationCoefficients.z * (distance * distance));    
    }
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir){
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
       
    // do lighting calculations
    // ambient
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;
  	
    // diffuse 
    float diff      = max(dot(normal, lightDir), 0.0);
    vec3 diffuse    = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;  
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular   = light.specular * spec * texture(material.texture_specular1, TexCoords).rgb;
        
    diffuse  *= intensity;
    specular *= intensity;

    float distance    = length(light.position - FragPos);
    float attenuation = 0;
    if(light.attenuationCoefficients.x > 0 || light.attenuationCoefficients.y > 0 || light.attenuationCoefficients.z > 0) {
        attenuation = 1.0 / (light.attenuationCoefficients.x + light.attenuationCoefficients.y * distance + light.attenuationCoefficients.z * (distance * distance));    
    }

    ambient     *= attenuation;
    diffuse     *= attenuation;
    specular    *= attenuation;

    return (ambient + diffuse + specular);
}