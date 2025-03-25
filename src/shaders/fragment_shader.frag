#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

in vec4 vertexColor;

out vec4 FragColor;

void main()
{
    vec3 ligth_position = vec3(1.0, 0.0, -3.0);
    // Phong basic lighting 
    // ambient
    float ambientStrength = 0.8;
    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);
  	
    // diffuse 
    // vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(ligth_position - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(- FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);  
        
    vec3 result = (ambient + diffuse + specular) * vertexColor.rgb;
    FragColor = vec4(result, 1.0);

    
    // FragColor = vec4(diff * vertexColor.rgb, 1.0);
}