#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 Tangent;
out vec3 Bitangent;

// MVP
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Color 
uniform vec4 color;
out vec4 vertexColor;

void main()
{
    FragPos = vec3(model * vec4(aPosition, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    Tangent = mat3(model) * aTangent;
    Bitangent = mat3(model) * aBitangent;

    
    if (color == vec4(0.0)) {
        vertexColor = vec4(0.5);
    } else vertexColor = color;

    gl_Position = projection * view * vec4(FragPos, 1.0);
    // gl_Position = vec4(FragPos, 1.0);

}