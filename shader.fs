#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 ourColor;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue = 0;
uniform int textureMode;

void main()
{
	if (textureMode == 1){
		FragColor = texture(texture1, TexCoord);
	} else {
		FragColor = ourColor;
	}
   
}