#include "Utility.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <SFML/Graphics.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

#define ASSERT(expr) assert(expr)

std::string Utility::LoadTextFile(const std::string& filepath)
{
    std::ifstream t(filepath);
    return std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
}

std::vector<Model> Utility::LoadScene(const std::string& directory, const std::string& filename)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, (directory + "/" + filename).c_str(), directory.c_str());

    if (!warn.empty())
        std::cout << warn << std::endl;

    if (!err.empty())
        std::cerr << err << std::endl;

    if (!ret) throw;
    
    std::vector<Model> models(materials.size());
    
    for (size_t i = 0; i < materials.size(); i++)
    {
        Material& material = models[i].material;

        sf::Image image;
        if (image.loadFromFile((directory + "/" + materials[i].diffuse_texname).c_str()))
        {
            sf::Vector2u imageSize = image.getSize();
            material.albedo = Graphics::CreateTexture(TextureFormat::RBGA32, 1, imageSize.x, imageSize.y, image.getPixelsPtr(), true);
            Graphics::FilterTexture(material.albedo, TextureWrap::REPEAT, TextureWrap::REPEAT, TextureFilter::LINEAR_LINEAR, TextureFilter::LINEAR);
        }

        material.attributeFormat.emplace_back("vPos", 3);
        material.attributeFormat.emplace_back("vNor", 3);
        material.attributeFormat.emplace_back("vCol", 4);
        material.attributeFormat.emplace_back("vTex", 2);
    }

    std::vector<std::vector<VertexPNCT>> meshData(materials.size());

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++)
    {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            // per-face material
            int matId = shapes[s].mesh.material_ids[f];
            std::vector<VertexPNCT>& data = meshData[matId];

            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++)
            {
                VertexPNCT vertex;

                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                vertex.position.x = attrib.vertices[(3 * idx.vertex_index) + 0];
                vertex.position.y = attrib.vertices[(3 * idx.vertex_index) + 1];
                vertex.position.z = attrib.vertices[(3 * idx.vertex_index) + 2];
                vertex.normal.x = attrib.normals[(3 * idx.normal_index) + 0];
                vertex.normal.y = attrib.normals[(3 * idx.normal_index) + 1];
                vertex.normal.z = attrib.normals[(3 * idx.normal_index) + 2];
                vertex.texcoord.x = attrib.texcoords[(2 * idx.texcoord_index) + 0];
                vertex.texcoord.y = attrib.texcoords[(2 * idx.texcoord_index) + 1];
                vertex.color.r = attrib.colors[3*idx.vertex_index+0];
                vertex.color.g = attrib.colors[3*idx.vertex_index+1];
                vertex.color.b = attrib.colors[3*idx.vertex_index+2];
                vertex.color.a = 1.0f;

                data.emplace_back(vertex);
            }
            index_offset += fv;
        }
    }

    for (size_t i = 0; i < materials.size(); i++)
    {
        std::vector<VertexPNCT>& data = meshData[i];

        Mesh& mesh = models[i].mesh;
        mesh.vBuffer = Graphics::CreateBuffer(1, data.size() * 12, &data[0], false, false);
        mesh.count = data.size();
    }

    return models;
}