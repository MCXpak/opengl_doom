#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;
in vec3 pos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float aMix;

const vec4 innerColor = vec4(0.5, 0.0, 0.0, 1.0);
const vec2 center = vec2(0.5, 0.5);
const float cutoffHalfSize = 0.4;

void main()
{
    FragColor = vec4(1.0);
}