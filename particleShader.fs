#version 330 core
out vec4 FragColor;
  
in vec3 fColor;

uniform vec3 objectColor;
uniform float colorAlpha;

void main()
{
    FragColor = vec4(objectColor, colorAlpha);
}