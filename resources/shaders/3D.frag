#version 330 core

struct Material 
{
    vec3 albedo;
    vec3 specular;
    vec3 emissive;
    float shininess;
    float metalness;
    float roughness;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 Tangent;
in vec3 Bitangent;

out vec4 FragColor;
uniform Material material;
uniform vec3 ligth_position;
uniform vec3 viewPos;
uniform float transparency; // Transparency value (0.0 = fully transparent, 1.0 = fully opaque)

void main()
{
    // Blinn-Phong lighting
    
    // ambient
    float ambientStrength = 0.6;
    vec3 ambient = ambientStrength * material.albedo;
    
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(ligth_position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.albedo;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = vec3(0.1) * spec; 
        
    vec3 result = (ambient + diffuse + specular + material.emissive);
    FragColor = vec4(result, transparency); // Apply transparency to the final color
}
