#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>

#include <Renderer.hpp>

static const float vertices[] =
{
    -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 1.0f, 1.0f, 1.0f
};

static const int indices[] = { 0, 1, 2, 2, 1, 3 };

static const char* attributes[] = { "vPos", "vCol" };

static const int formats[] = { 2, 3 };

static const char* vertexSrc =
"#version 330 core\n"
"layout (location = 0) in vec2 vPos;\n"
"layout (location = 1) in vec3 vCol;\n"
"uniform mat4 MVP;\n"
"out vec3 pColor;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    pColor = vCol;\n"
"}\n";

static const char* pixelSrc =
"#version 330 core\n"
"precision mediump float;\n"
"in vec3 pColor;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(pColor, 1.0);\n"
"}\n";

sf::RenderWindow* pWindow;

Buffer vBuffer;
Buffer iBuffer;
Shader shader;

constexpr auto SCR_WIDTH = 800;
constexpr auto SCR_HEIGHT = 600;

bool init()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    // create the window
    pWindow = new sf::RenderWindow(sf::VideoMode(SCR_WIDTH, SCR_HEIGHT), "SFML Template", sf::Style::Default, settings);
    pWindow->setActive(true);
    pWindow->setVerticalSyncEnabled(true);
    //pWindow->setMouseCursorGrabbed(true);
    //pWindow->setMouseCursorVisible(false);
    //pWindow->setKeyRepeatEnabled(false);
    //sf::Mouse::setPosition(sf::Vector2i(SCR_WIDTH / 2, SCR_HEIGHT / 2), *pWindow);

    settings = pWindow->getSettings();

    std::cout << "depth bits:" << settings.depthBits << std::endl;
    std::cout << "stencil bits:" << settings.stencilBits << std::endl;
    std::cout << "antialiasing level:" << settings.antialiasingLevel << std::endl;
    std::cout << "version:" << settings.majorVersion << "." << settings.minorVersion << std::endl;

    gladLoadGL();

    vBuffer = Renderer::CreateBuffer(1, sizeof(vertices) / sizeof(float), vertices, false, false);
    iBuffer = Renderer::CreateBuffer(1, sizeof(indices) / sizeof(int), indices, true, false);
    shader = Renderer::CreateShader(vertexSrc, pixelSrc, nullptr);

    Renderer::ClearColor(0, 0, 0, 1);
    Renderer::SetCull(true);
    Renderer::SetCullFace(CullFace::BACK);
    Renderer::SetFaceWinding(true);

    pWindow->setActive(false);
    return true;
}

void input(sf::Event e)
{
    if (e.type == sf::Event::Resized)
        glViewport(0, 0, e.size.width, e.size.height);
}

void update(sf::Time dt)
{
}

void render()
{
    sf::Vector2u winSize = pWindow->getSize();
    int width = winSize.x, height = winSize.y;
    float ratio = width / (float)height;

    Renderer::SetViewport(0, 0, width, height);
    Renderer::ClearScreen();

    glm::mat4 mvp = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 1.0f);

    Renderer::BindBuffer(vBuffer, false);
    Renderer::BindBuffer(iBuffer, true);
    Renderer::BindShader(shader, sizeof(formats) / sizeof(int), attributes, formats);

    Renderer::SetUniform(shader, "MVP", 1, &mvp);
    Renderer::DrawIndexed(Primitive::TRIANGLES, sizeof(indices) / sizeof(int));

    Renderer::DetachShader();
    Renderer::DetachBuffer();
}

void clean()
{
    Renderer::DeleteShader(shader);
    Renderer::DeleteBuffer(1, vBuffer);

    delete pWindow;
    pWindow = nullptr;
}

int main()
{
    if (!init())
        return EXIT_FAILURE;

    int frames = 0;
    float timer = 0;
    sf::Clock deltaClock;
    while (pWindow->isOpen())
    {
        sf::Event event;
        while (pWindow->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                pWindow->close();
            input(event);
        }

        sf::Time dt = deltaClock.restart();
        timer += dt.asSeconds();
        if (timer >= 1)
        {
            std::cout << "FPS: " << frames << std::endl;
            timer -= 1;
            frames = 0;
        }

        update(dt);

        pWindow->setActive(true);
        render();
        pWindow->display();
        pWindow->setActive(false);

        frames++;
    }

    clean();
    return EXIT_SUCCESS;
}