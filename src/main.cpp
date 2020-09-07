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
    -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.5f, 0.0f, 0.0f, 1.0f
};

static const char* attributes[] = { "vPos", "vCol" };

static const int formats[] = { 2, 3 };

static const char* vertex_shader_text =
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

static const char* fragment_shader_text =
"#version 330 core\n"
"in vec3 pColor;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(pColor, 1.0);\n"
"}\n";

sf::RenderWindow* pWindow;

GLuint mesh;
GLuint shader;

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
    //pWindow->setVerticalSyncEnabled(true);
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

    mesh = Renderer::CreateMesh(sizeof(vertices) / sizeof(float), vertices, 0, nullptr, false);
    shader = Renderer::CreateShader(vertex_shader_text, fragment_shader_text, nullptr);
    Renderer::ClearColor(1, 0, 0, 1);

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
    Renderer::ClearScreen();

    //sf::Vector2u winSize = pWindow->getSize();
    //int width = winSize.x, height = winSize.y;
    //float ratio = width / (float)height;

    glm::mat4 mvp = glm::mat4(1);

    Renderer::BindShader(shader, sizeof(formats) / sizeof(int), attributes, formats);
    Renderer::SetUniform(shader, "MVP", mvp);
    Renderer::DrawVertices(3, 3);
}

void clean()
{
    delete pWindow;
    pWindow = nullptr;
}

int main()
{
    if (!init())
        return EXIT_FAILURE;

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

        update(deltaClock.restart());

        pWindow->setActive(true);
        render();
        pWindow->display();
        pWindow->setActive(false);
    }

    clean();
    return EXIT_SUCCESS;
}