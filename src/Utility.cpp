#include "Utility.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <SFML/Graphics.hpp>

#include <iostream>

#define ASSERT(expr) assert(expr)

Model Utility::LoadModel(const std::string& dir, const std::string& filename)
{
    Model model;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, (dir + "/" + filename).c_str(), dir.c_str());

    if (!warn.empty())
        std::cout << warn << std::endl;

    if (!err.empty())
        std::cerr << err << std::endl;

    if (!ret) throw;

    std::vector<std::vector<VertexPNCT>> meshes;

    for (size_t i = 0; i < materials.size(); i++)
    {
        tinyobj::material_t& mat = materials[i];
        Model::Material material = Model::Material();

        sf::Image image;
        if (image.loadFromFile((dir + "/" + mat.diffuse_texname).c_str()))
        {
            sf::Vector2u imageSize = image.getSize();
            material.texture = Renderer::CreateTexture(TextureFormat::RBGA32, 1, imageSize.x, imageSize.y, image.getPixelsPtr(), true);
            Renderer::FilterTexture(material.texture, TextureWrap::REPEAT, TextureWrap::REPEAT, TextureFilter::LINEAR_LINEAR, TextureFilter::LINEAR);
        }

        model.materials.emplace_back(material);
        meshes.emplace_back(std::vector<VertexPNCT>());
    }

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++)
    {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            // per-face material
            int matId = shapes[s].mesh.material_ids[f];
            tinyobj::material_t mat = materials[matId];

            std::vector<VertexPNCT>& meshData = meshes[matId];

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
                // Optional: vertex colors
                // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
                vertex.color = glm::vec4(1, 1, 1, 1);

                meshData.emplace_back(vertex);
            }
            index_offset += fv;
        }
    }

    for (size_t i = 0; i < materials.size(); i++)
    {
        std::vector<VertexPNCT>& meshData = meshes[i];

        Model::Mesh mesh;
        mesh.vBuffer = Renderer::CreateBuffer(1, meshData.size() * 12, &meshData[0], false, false);
        mesh.count = meshData.size();

        model.meshes.emplace_back(mesh);
    }

    return model;
}