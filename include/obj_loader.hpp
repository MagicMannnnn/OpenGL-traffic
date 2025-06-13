#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include <iostream>
#include <map>

#include <mesh.hpp>
#include <shader.h> 
#include <texture.hpp>
class Model {
public:
    Model() = default;
    // Constructor
    Model(const std::string& path) {
        loadModel(path);
        std::cout << "Model has " << meshes.size() << " meshes" << std::endl;
    }

    // Draw the model
    void Draw(Shader& shader) {
        for (auto& mesh : meshes) {
            mesh.Draw(shader);
        }
    }

    void DrawInstanced(Shader& shader, int instanceCount) {
        for (auto& mesh : meshes) {
            mesh.DrawInstanced(shader, instanceCount);
        }
    }

    void setupInstancing(std::vector<glm::mat4>& instanceMatrices) {
        for (auto& mesh : meshes) {
            mesh.setupInstancing(instanceMatrices);
        }
    }

private:
    std::vector<Mesh> meshes;
    std::string directory;
    std::map<std::string, unsigned int> loadedTextures; // Cache loaded textures

    // Load the model using Assimp
    void loadModel(const std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }

        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    // Process each node recursively
    void processNode(aiNode* node, const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    // Process a single mesh
    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // Extract vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;

            // Positions
            vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

            // Normals
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

            // Texture coordinates
            if (mesh->mTextureCoords[0]) {
                vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            // Tangents
            vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

            // Bitangents
            vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

            vertices.push_back(vertex);
        }

        // Extract indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Extract materials (textures)
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            // Diffuse textures
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            // Specular textures
            std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            // Normal maps
            std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

            // Height maps
            std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }

        return Mesh(vertices, indices, textures);
    }

    // Load material textures (and colors)
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName) {
        std::vector<Texture> textures;

        // Check if texture is present
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);

            // Check if texture is already loaded
            if (loadedTextures.find(str.C_Str()) != loadedTextures.end()) {
                textures.push_back({ loadedTextures[str.C_Str()], typeName, str.C_Str() });
                continue;
            }

            // Load texture
            std::string filename = directory + "/" + str.C_Str();
            unsigned int textureID = TextureFromFile(filename.c_str());
            textures.push_back({ textureID, typeName, str.C_Str() });
            loadedTextures[str.C_Str()] = textureID; // Cache texture
        }

        // Handle color properties when there are no textures
        aiColor4D color;
        if (type == aiTextureType_DIFFUSE) {
            if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
                // Store the color as a "texture" with type "color_diffuse"
                textures.push_back({ "color_diffuse", color.r, color.g, color.b, color.a });
            }
        }

        // Similar code can be written for other colors like specular, ambient, etc.
        if (type == aiTextureType_SPECULAR) {
            if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
                textures.push_back({ "color_specular", color.r, color.g, color.b, color.a });
            }
        }

        if (type == aiTextureType_AMBIENT) {
            if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
                textures.push_back({ "color_ambient", color.r, color.g, color.b, color.a });
            }
        }

        return textures;
    }

    // Load a texture from file
    unsigned int TextureFromFile(const char* path) {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

        if (data) {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else {
            std::cerr << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }
};


#endif
