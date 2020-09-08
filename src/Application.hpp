#pragma once

#include <Framework/Framework.hpp>

class Application : public IApplication
{
public:
	virtual bool Initialize(sf::RenderWindow& window);
	virtual void Input(const sf::Event& e);
	virtual void Update(const sf::Time& deltaTime);
	virtual void Render();
	virtual void Clean();

private:
	sf::Vector2f mousePos;

	float moveSpeed = 25.0f;
	float lookSpeed = 0.5f;

	Shader shader;
	Scene scene;
};