#version 330 core

in vec2 TexCoord; // Texture coordinates passed from the vertex shader

out vec4 FragColor; // Final color of the fragment (pixel)

uniform sampler2D texture1; // Texture sampler

void main()
{
    // Sample the texture and assign it to the fragment color
    FragColor = texture(texture1, TexCoord);
}
