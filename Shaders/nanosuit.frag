#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{    
	vec4 diffuse = texture(texture_diffuse1, TexCoords);
	vec4 specular = texture(texture_specular1, TexCoords);
	//color = diffuse+specular;
    color = mix(diffuse, specular, 0.5);;
}