#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

in vec4 VertexColor;

uniform sampler2D texture_diffuse1;
uniform vec3 ligth_position;
uniform vec3 viewPos;


void main()
{
    // Phong basic lighting 
    // ambient
    float ambientStrength = 0.8;
    vec3 ambient = ambientStrength * VertexColor.rgb; //* texture(texture_diffuse1, TexCoords).rgb;
  	
    // diffuse 
    vec3 lightDir = normalize(ligth_position - FragPos);
    vec3 norm = normalize(Normal);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * VertexColor.rgb;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);  
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.3) * spec; 
        
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);

    // FragColor = vec4(VertexColor.rgb, 1.0);

}