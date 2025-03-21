#version 330 core

in vec4 vertexColor;  // Couleur reçue du vertex shader
out vec4 FragColor;   // Couleur finale du pixel

void main() {
    FragColor = vertexColor;
}
