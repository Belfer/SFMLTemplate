#pragma once

#include <glm/glm.hpp>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <Framework/Graphics.hpp>

#include <vector>
#include <string>

struct Camera
{
	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);

	glm::mat4 projection = glm::mat4(1);
};

struct DirectionalLight
{
	glm::vec3 direction = glm::vec3(0);
	glm::vec3 color = glm::vec3(1);
	float intensisty = 1.0f;
};

struct VertexPNCT
{
	glm::vec3 position = glm::vec3(0);
	glm::vec3 normal = glm::vec3(0);
	glm::vec4 color = glm::vec4(1);
	glm::vec2 texcoord = glm::vec2(0);
};

struct Mesh
{
	Primitive primitive = Primitive::TRIANGLES;
	Buffer vBuffer = 0;
	Buffer iBuffer = 0;
	unsigned int count = 0;
};

struct Material
{
	std::vector<AttributeFormat> attributeFormat;
	Shader shader = 0;
	
	glm::vec3 diffuse = glm::vec3(1);
	Texture albedo = 0;
};

struct Transform
{
	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
};

struct Model
{
	Transform transform;
	Material material;
	Mesh mesh;
};

class Scene
{
public:
	void Render();

	Camera camera;
	DirectionalLight sun;
	std::vector<Model> models;
};

class IApplication
{
public:
	virtual ~IApplication() {}

	virtual bool Initialize(sf::RenderWindow& window) = 0;
	virtual void Input(const sf::Event& e) = 0;
	virtual void Update(const sf::Time& deltaTime) = 0;
	virtual void Render() = 0;
	virtual void Clean() = 0;
};

class Engine
{
public:
	static int Run(IApplication* pApp, const std::string& title, int width, int height, const sf::ContextSettings& settings);
};