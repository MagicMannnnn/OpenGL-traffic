#version 330 core

layout (location = 0) in vec3 aPos;       // Vertex position
layout (location = 1) in vec3 aNormal;    // Vertex normal
layout (location = 2) in vec2 aTexCoords; // Texture coordinates
layout (location = 3) in vec3 aTangent;   // Tangent vector
layout (location = 4) in vec3 aBitangent; // Bitangent vector
layout (location = 5) in mat4 instanceModel; // Instanced model matrix

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;  // Fragment position in world space
out vec3 Normal;   // Normal in world space
out vec2 TexCoords;

void main() {
    FragPos = vec3(instanceModel * vec4(aPos, 1.0)); // Transform position
    Normal = mat3(transpose(inverse(instanceModel))) * aNormal; // Transform normal
    TexCoords = aTexCoords; // Pass texture coordinates

    gl_Position = projection * view * vec4(FragPos, 1.0); // Final position
}
