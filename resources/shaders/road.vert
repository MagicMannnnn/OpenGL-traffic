#version 330 core

layout(location = 0) in vec3 aPos;       // Vertex position
layout(location = 1) in vec2 aTexCoord;  // Texture coordinates

out vec2 TexCoord; // Output to fragment shader (texture coordinates)

uniform mat4 model;       // Model transformation matrix
uniform mat4 view;        // View transformation matrix (camera)
uniform mat4 projection;  // Projection transformation matrix

void main()
{
    // Apply model, view, and projection transformations to the vertex position
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    //gl_Position = projection * model * vec4(aPos, 1.0f);
    
    // Pass the texture coordinates to the fragment shader
    TexCoord = aTexCoord;
}
