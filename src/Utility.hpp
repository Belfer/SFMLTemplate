#pragma once

#include <Renderer.hpp>

#include <string>
#include <vector>

struct VertexPNCT
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 color;
    glm::vec2 texcoord;
};

struct Model
{
    struct Mesh
    {
        Buffer vBuffer = 0;
        Buffer iBuffer = 0;
        unsigned int count = 0;
    };

    struct Material
    {
        Texture texture = 0;
    };

    std::vector<Mesh> meshes;
    std::vector<Material> materials;
};

class Utility
{
public:
    static Model LoadModel(const std::string& dir, const std::string& filename);
};