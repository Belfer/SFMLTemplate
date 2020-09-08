#include "Application.hpp"

#include <glm/gtx/string_cast.hpp>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <cmath>

#include <Framework/Framework.hpp>
#include <Framework/Graphics.hpp>
#include <Framework/Utility.hpp>

bool Application::Initialize(sf::RenderWindow& window)
{
    sf::Vector2u size = window.getSize();
    float width = (float)size.x;
    float height = (float)size.y;
    float aspect = width / height;

    Graphics::SetViewport(0, 0, width, height);
    scene.camera.projection = glm::perspective(70.0f, aspect, 0.1f, 1000.0f);

    window.setVerticalSyncEnabled(true);
    //window.setMouseCursorGrabbed(true);
    //window.setMouseCursorVisible(false);
    //window.setKeyRepeatEnabled(false);
    //sf::Mouse::setPosition(sf::Vector2i(width / 2, height / 2), window);

    // Setup pipeline
    Graphics::SetClearColor(0.4f, 0.5f, 0.8f, 1.0f); // Day
    //Graphics::SetClearColor(0.05f, 0.1f, 0.25f, 1.0f); // Night

    Graphics::SetCull(true);
    Graphics::SetCullFace(CullFace::BACK);
    Graphics::SetFaceWinding(true);
    Graphics::SetDepthTest(true);
    Graphics::SetDepthWrite(true);
    Graphics::SetBlend(true);
    Graphics::SetBlendFunc(BlendFunc::INTERPOLATE);

    // Init variables
    scene.sun.direction = glm::normalize(glm::vec3(1, 3, -10));

    scene.sun.color = glm::vec3(0.95f, 0.95f, 1.0f); // Day
    scene.sun.intensisty = 1.15f;
    //scene.sun.color = glm::vec3(0.15f, 0.25f, 0.4f); // Night
    //scene.sun.intensisty = 0.2f;

    scene.camera.position = glm::vec3(0.0f, 0.0f, 1.7f);
    scene.camera.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

    mousePos = (sf::Vector2f)sf::Mouse::getPosition();

    // Load shaders
    std::string vSource = Utility::LoadTextFile("data/Shaders/lit.v.glsl");
    std::string pSource = Utility::LoadTextFile("data/Shaders/lit.p.glsl");
    shader = Graphics::CreateShader(vSource.c_str(), pSource.c_str(), nullptr);

    // Load scene
    std::vector<Model> sponza = Utility::LoadScene("data/Sponza", "sponza.obj");
    for (size_t i = 0; i < sponza.size(); ++i)
    {
        sponza[i].transform.rotation.x = glm::pi<float>() * 0.5f;
        sponza[i].transform.scale = glm::vec3(0.1f);

        sponza[i].material.shader = shader;
    }

    std::vector<Model> statue = Utility::LoadScene("data/Statue", "statue.obj");
    for (size_t i = 0; i < statue.size(); ++i)
    {
        statue[i].transform.rotation.z = glm::pi<float>() * 0.5f;
        statue[i].transform.scale = glm::vec3(0.02f);

        statue[i].material.shader = shader;
    }

    scene.models.insert(scene.models.end(), sponza.begin(), sponza.end());
    scene.models.insert(scene.models.end(), statue.begin(), statue.end());

    return true;
}

void Application::Input(const sf::Event& e)
{
    if (e.type == sf::Event::Resized)
    {
        float width = (float)e.size.width;
        float height = (float)e.size.height;
        float aspect = width / height;

        Graphics::SetViewport(0, 0, width, height);
        scene.camera.projection = glm::perspective(70.0f, aspect, 0.1f, 1000.0f);
    }
}

void Application::Update(const sf::Time& deltaTime)
{
    float dt = deltaTime.asSeconds();

    sf::Vector2f mouseTarget = (sf::Vector2f)sf::Mouse::getPosition();
    sf::Vector2f mouseDelta = 0.5f * (mouseTarget - mousePos);
    mousePos += mouseDelta;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
    {
        scene.camera.rotation.z -= mouseDelta.x * lookSpeed * dt;
        scene.camera.rotation.x -= mouseDelta.y * lookSpeed * dt;

        constexpr float xRange = glm::pi<float>() * 0.45f;
        scene.camera.rotation.x = glm::clamp(scene.camera.rotation.x, -xRange, xRange);
    }

    glm::vec3 move = glm::vec3(0, 0, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        move.x -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        move.x += 1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        move.y += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        move.y -= 1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        move.z -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        move.z += 1;

    if (glm::dot(move, move) > 1)
        move = glm::normalize(move);

    scene.camera.position += glm::quat(scene.camera.rotation) * move * moveSpeed * dt;

    //std::cout << "CamPos: " << glm::to_string(camPos) << std::endl;
    //std::cout << "CamRot: " << glm::to_string(camRot) << std::endl;
}

void Application::Render()
{
    scene.Render();
}

void Application::Clean()
{
    Graphics::DeleteShader(shader);
}