// Fragment Shader
// Note: no input in this shader, it just outputs the colour of all fragments,
// in this case set to red (format: R, G, B, A).

#version 330 core

in vec4 color;
out vec4 FragColor;

void main() {
  FragColor = color;
}
