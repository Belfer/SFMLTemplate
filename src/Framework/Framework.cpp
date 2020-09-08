#include "Framework.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

void Scene::Render()
{
    Graphics::ClearScreen(true, true, true);

    glm::mat4 v = glm::lookAt(camera.position, camera.position + glm::quat(camera.rotation) * glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
    
    int samplerLoc = 0;
    glm::vec2 tiling = glm::vec2(1, -1);

    for (size_t i = 0; i < models.size(); i++)
    {
        Model& model = models[i];

        glm::mat4 m = glm::translate(glm::mat4(1), model.transform.position) * glm::mat4(glm::quat(model.transform.rotation)) * glm::scale(glm::mat4(1), model.transform.scale);
        glm::mat4 mvp = camera.projection * v * m;

        Graphics::BindBuffer(model.mesh.vBuffer, false);
        if (model.mesh.iBuffer != 0)
            Graphics::BindBuffer(model.mesh.iBuffer, true);

        Graphics::BindShader(model.material.shader, model.material.attributeFormat);

        if (model.material.albedo != 0)
            Graphics::BindTexture(model.material.albedo, 0);

        Graphics::SetUniform(model.material.shader, "Model", 1, &m);
        Graphics::SetUniform(model.material.shader, "MVP", 1, &mvp);
        Graphics::SetUniform(model.material.shader, "SunDirection", 1, &sun.direction);
        Graphics::SetUniform(model.material.shader, "SunColor", 1, &sun.color);
        Graphics::SetUniform(model.material.shader, "SunIntensity", 1, &sun.intensisty);

        Graphics::SetUniform(model.material.shader, "Texture", 1, &samplerLoc);
        Graphics::SetUniform(model.material.shader, "Tiling", 1, &tiling);
        
        if (model.mesh.iBuffer != 0)
            Graphics::DrawIndexed(Primitive::TRIANGLES, model.mesh.count);
        else
            Graphics::DrawVertices(Primitive::TRIANGLES, 0, model.mesh.count);

        Graphics::DetachTexture();
        Graphics::DetachShader();
        Graphics::DetachBuffer();
    }
}

int Engine::Run(IApplication* pApp, const std::string& title, int width, int height, const sf::ContextSettings& settings)
{
    // Create the window
    sf::RenderWindow window(sf::VideoMode(width, height), title.c_str(), sf::Style::Default, settings);
    std::cout << "Depth bits: " << window.getSettings().depthBits << std::endl;
    std::cout << "Stencil bits: " << window.getSettings().stencilBits << std::endl;
    std::cout << "Antialiasing level: " << window.getSettings().antialiasingLevel << std::endl;
    std::cout << "Version: " << window.getSettings().majorVersion << "." << window.getSettings().minorVersion << std::endl;

    window.setActive(true);
    Graphics::Initialize();

    if (!pApp->Initialize(window))
        return EXIT_FAILURE;

    int frames = 0;
    float timer = 0;
    sf::Clock deltaClock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            pApp->Input(event);
        }

        sf::Time dt = deltaClock.restart();
        timer += dt.asSeconds();
        if (timer >= 1)
        {
            std::cout << "FPS: " << frames << std::endl;
            timer -= 1;
            frames = 0;
        }

        pApp->Update(dt);

        pApp->Render();
        window.display();

        frames++;
    }

    pApp->Clean();
    window.setActive(false);

    return EXIT_SUCCESS;
}