#version 330 core
out vec4 FragColor;

in vec2 ourTexture;

uniform vec3 aColor;
uniform sampler2D texture_diffuse1;

void main()
{

    FragColor = texture(texture_diffuse1, ourTexture) * vec4(aColor,1.0f);
}