#pragma once

#include <glm/glm.hpp>
#include <Shader.h>
#include <vector>
#include <texture.hpp>

class Road {
public:
    Road() = default;
    Road(const std::string& texturePath, float width, float length);
    ~Road();


    void setupRender(Shader& shader);

private:
    unsigned int VAO, VBO, EBO;  // OpenGL buffers for geometry
    unsigned int textureID;       // OpenGL texture ID for the road texture
    float width, length;          // Dimensions of the road segment

    // Generate a simple rectangular mesh for the road
    void generateRoadMesh();

    // Load the texture from the file path
    void loadTexture(const std::string& path);
};

inline Road::Road(const std::string& texturePath, float width, float length)
    : width(width), length(length) {
    generateRoadMesh();
    loadTexture(texturePath);
}

inline Road::~Road() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &textureID); // Deleting the texture ID directly
}

inline void Road::generateRoadMesh() {
    // Define the geometry of the road (a quad)
    std::vector<float> vertices = {
        // Positions          // Texture Coords
        -width / 2, 0.0f, -length / 2,  0.0f, 0.0f,
         width / 2, 0.0f, -length / 2,  1.0f, 0.0f,
         width / 2, 0.0f,  length / 2,  1.0f, 1.0f,
        -width / 2, 0.0f,  length / 2,  0.0f, 1.0f
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3
    };

    // Set up buffers and array objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);  // Unbind the VAO
}

inline void Road::loadTexture(const std::string& path) {
    // Load the texture using stb_image
    int texWidth, texHeight, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &texWidth, &texHeight, &nrChannels, 0);

    if (data) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
}



inline void Road::setupRender(Shader& shader) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID); // Use the direct texture ID

    shader.setInt("texture1", 0);  // Set the texture unit to 0

    glBindVertexArray(VAO);
}
