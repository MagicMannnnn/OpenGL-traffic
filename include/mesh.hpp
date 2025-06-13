
#ifndef MESH_H
#define MESH_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <texture.hpp>
#include <iostream>
#include <shader.h>


struct Vertex {
    glm::vec3 Position;   // Position
    glm::vec3 Normal;     // Normal
    glm::vec2 TexCoords;  // Texture coordinates
    glm::vec3 Tangent;    // Tangent vector
    glm::vec3 Bitangent;  // Bitangent vector
};

class Mesh {
public:
    // Mesh data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
        : vertices(vertices), indices(indices), textures(textures) {
        setupMesh();
        glGenBuffers(1, &instanceVBO);
    }

    // Render the mesh
    void Draw(Shader& shader) {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        glm::vec4 diffuseColor(1.0f), specularColor(1.0f), ambientColor(1.0f); // Default colors

        for (unsigned int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i); // Activate texture unit
            std::string number;
            std::string name = textures[i].type;

            if (name == "texture_diffuse") {
                number = std::to_string(diffuseNr++);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }
            else if (name == "texture_specular") {
                number = std::to_string(specularNr++);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }
            else if (name == "texture_normal") {
                number = std::to_string(normalNr++);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }
            else if (name == "texture_height") {
                number = std::to_string(heightNr++);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }
            else if (name == "color_diffuse") { // Handle diffuse color
                diffuseColor = glm::vec4(textures[i].r, textures[i].g, textures[i].b, textures[i].a);
            }
            else if (name == "color_specular") { // Handle specular color
                specularColor = glm::vec4(textures[i].r, textures[i].g, textures[i].b, textures[i].a);
            }
            else if (name == "color_ambient") { // Handle ambient color
                ambientColor = glm::vec4(textures[i].r, textures[i].g, textures[i].b, textures[i].a);
            }

            shader.setInt((name + number).c_str(), i);
        }

        // Set color uniforms if colors are present
        shader.setVec4("diffuseColor", diffuseColor);
        shader.setVec4("specularColor", specularColor);
        shader.setVec4("ambientColor", ambientColor);

        // Draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0); // Reset texture
    }

    void DrawInstanced(Shader& shader, int instanceCount) {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        glm::vec4 diffuseColor(1.0f), specularColor(1.0f), ambientColor(1.0f); // Default colors

        for (unsigned int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i); // Activate texture unit
            std::string number;
            std::string name = textures[i].type;

            if (name == "texture_diffuse") {
                number = std::to_string(diffuseNr++);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }
            else if (name == "texture_specular") {
                number = std::to_string(specularNr++);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }
            else if (name == "texture_normal") {
                number = std::to_string(normalNr++);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }
            else if (name == "texture_height") {
                number = std::to_string(heightNr++);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }
            else if (name == "color_diffuse") { // Handle diffuse color
                diffuseColor = glm::vec4(textures[i].r, textures[i].g, textures[i].b, textures[i].a);
            }
            else if (name == "color_specular") { // Handle specular color
                specularColor = glm::vec4(textures[i].r, textures[i].g, textures[i].b, textures[i].a);
            }
            else if (name == "color_ambient") { // Handle ambient color
                ambientColor = glm::vec4(textures[i].r, textures[i].g, textures[i].b, textures[i].a);
            }

            shader.setInt((name + number).c_str(), i);
        }

        // Set color uniforms if colors are present
        shader.setVec4("diffuseColor", diffuseColor);
        shader.setVec4("specularColor", specularColor);
        shader.setVec4("ambientColor", ambientColor);

        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0, instanceCount);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);

        

    }

    void setupInstancing(std::vector<glm::mat4>& instanceMatrices) {
        if (instanceMatrices.empty()) return; // No instances, nothing to do
        
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceMatrices.size() * sizeof(glm::mat4), instanceMatrices.data(), GL_STATIC_DRAW);

        glBindVertexArray(VAO);

        // Mat4 occupies 4 attribute locations (mat4 = 4 * vec4)
        for (int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(5 + i);
            glVertexAttribPointer(5 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
            glVertexAttribDivisor(5 + i, 1); // Important: Divisor tells OpenGL to use one per instance
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        
    }

private:
    unsigned int VAO, VBO, EBO, instanceVBO;

    void setupMesh() {
        // Generate buffers
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Vertex attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
};


#endif

