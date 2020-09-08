#pragma once

#include <glm/glm.hpp>

#include <Framework/Graphics.hpp>
#include <Framework/Framework.hpp>

#include <string>
#include <vector>

class Utility
{
public:
    static std::string LoadTextFile(const std::string& filepath);

    static std::vector<Model> LoadScene(const std::string& directory, const std::string& filename);
};