#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>

#include <Renderer.hpp>

sf::RenderWindow* pWindow;
constexpr auto SCR_WIDTH = 800;
constexpr auto SCR_HEIGHT = 600;

glm::vec3 sunDir;
glm::vec3 sunCol;

glm::vec3 camPos;
glm::vec3 camRot;

sf::Vector2i mousePos;

float moveSpeed = 5.0f;
float lookSpeed = 1.5f;

float floorSize = 100.0f;

Mesh ground;
Shader shader;
Texture texture;

static const char* attributes[] = { "vPos", "vNor", "vCol", "vTex" };

static const int formats[] = { 3, 3, 4, 2 };

static const char* vertexSrc =
"#version 330 core\n"
"layout (location = 0) in vec3 vPos;\n"
"layout (location = 1) in vec3 vNor;\n"
"layout (location = 2) in vec4 vCol;\n"
"layout (location = 3) in vec2 vTex;\n"
"out vec3 pNormal;\n"
"out vec4 pColor;\n"
"out vec2 pTexCoord;\n"
"uniform mat4 MVP;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    pNormal = vNor;\n"
"    pColor = vCol;\n"
"    pTexCoord = vTex;\n"
"}\n";

static const char* pixelSrc =
"#version 330 core\n"
"precision mediump float;\n"
"in vec3 pNormal;\n"
"in vec4 pColor;\n"
"in vec2 pTexCoord;\n"
"uniform sampler2D Texture;\n"
"uniform vec2 Tiling;\n"
"uniform vec3 SunDirection;\n"
"uniform vec3 SunColor;\n"
"void main()\n"
"{\n"
"    gl_FragColor = max(dot(pNormal, -SunDirection), 0) * texture(Texture, pTexCoord * Tiling) * vec4(pColor);\n"
"}\n";

struct VertexPNCT
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 color;
    glm::vec2 texcoord;
};

static const VertexPNCT vertices[] =
{
    { {-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f} },
    { {0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },
    { {-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f} },
    { {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },
};

static const int indices[] = { 0, 1, 2, 2, 1, 3 };

bool init()
{
    pWindow->setVerticalSyncEnabled(true);
    //pWindow->setMouseCursorGrabbed(true);
    //pWindow->setMouseCursorVisible(false);
    //pWindow->setKeyRepeatEnabled(false);
    //sf::Mouse::setPosition(sf::Vector2i(SCR_WIDTH / 2, SCR_HEIGHT / 2), *pWindow);

    // Setup pipeline
    Renderer::SetClearColor(0.4f, 0.5f, 0.8f, 1.0f);
    Renderer::SetCull(false);
    Renderer::SetCullFace(CullFace::BACK);
    Renderer::SetFaceWinding(true);

    // Init variables
    sunDir = glm::normalize(glm::vec3(2, 5, -10));
    sunCol = glm::vec3(0.1f, 0.3f, 0.9f);

    camPos = glm::vec3(0.0f, 0.0f, 1.8f);
    camRot = glm::vec3(0.0f, 0.0f, 0.0f);

    mousePos = sf::Mouse::getPosition();

    // Load shaders
    shader = Renderer::CreateShader(vertexSrc, pixelSrc, nullptr);

    // Load buffers
    ground.vBuffer = Renderer::CreateBuffer(1, sizeof(vertices) / sizeof(float), vertices, false, false);
    ground.iBuffer = Renderer::CreateBuffer(1, sizeof(indices) / sizeof(int), indices, true, false);

    // Load texture
    sf::Image image;
    if (!image.loadFromFile("data/wests_textures/grass1.png"))
        return false;

    sf::Vector2u imageSize = image.getSize();
    texture = Renderer::CreateTexture(TextureFormat::RBGA32, 1, imageSize.x, imageSize.y, image.getPixelsPtr(), true);
    Renderer::FilterTexture(texture, TextureWrap::REPEAT, TextureWrap::REPEAT, TextureFilter::NEAREST_LINEAR, TextureFilter::NEAREST);

    return true;
}

void input(sf::Event e)
{
    if (e.type == sf::Event::Resized)
        Renderer::SetViewport(0, 0, e.size.width, e.size.height);
}

void update(sf::Time deltaTime)
{
    float dt = deltaTime.asSeconds();

    sf::Vector2i mouseDelta = mousePos - sf::Mouse::getPosition();
    mousePos = sf::Mouse::getPosition();

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
    {
        camRot.z += mouseDelta.x * lookSpeed * dt;
        camRot.x += mouseDelta.y * lookSpeed * dt;

        constexpr float xRange = glm::pi<float>() * 0.45f;
        camRot.x = glm::clamp(camRot.x, -xRange, xRange);
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

    camPos += glm::quat(camRot) * move * moveSpeed * dt;

    //std::cout << "CamPos: " << glm::to_string(camPos) << std::endl;
    //std::cout << "CamRot: " << glm::to_string(camRot) << std::endl;
}

void render()
{
    sf::Vector2u winSize = pWindow->getSize();
    int width = winSize.x, height = winSize.y;
    float aspect = width / (float)height;

    Renderer::ClearScreen(true, true, true);

    Renderer::BindBuffer(ground.vBuffer, false);
    Renderer::BindBuffer(ground.iBuffer, true);
    Renderer::BindShader(shader, sizeof(formats) / sizeof(int), attributes, formats);
    
    glm::mat4 m = glm::scale(glm::mat4(1), glm::vec3(floorSize, floorSize, 1));
    glm::mat4 v = glm::lookAt(camPos, camPos + glm::quat(camRot) * glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
    glm::mat4 p = glm::perspective(70.0f, aspect, 0.1f, 1000.0f);
    glm::mat4 mvp = p * v * m;
    Renderer::SetUniform(shader, "MVP", 1, &mvp);

    int samplerLoc = 0;
    Renderer::BindTexture(texture, 0);
    Renderer::SetUniform(shader, "Texture", 1, &samplerLoc);

    glm::vec2 tiling = glm::vec2(floorSize, floorSize);
    Renderer::SetUniform(shader, "Tiling", 1, &tiling);

    Renderer::SetUniform(shader, "SunDirection", 1, &sunDir);
    Renderer::SetUniform(shader, "SunColor", 1, &sunCol);

    Renderer::DrawIndexed(Primitive::TRIANGLES, sizeof(indices) / sizeof(int));

    Renderer::DetachTexture();
    Renderer::DetachShader();
    Renderer::DetachBuffer();
}

void clean()
{
    Renderer::DeleteTexture(1, texture);
    Renderer::DeleteBuffer(1, ground.iBuffer);
    Renderer::DeleteBuffer(1, ground.vBuffer);
    Renderer::DeleteShader(shader);
}

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    // Create the window
    pWindow = new sf::RenderWindow(sf::VideoMode(SCR_WIDTH, SCR_HEIGHT), "SFML Template", sf::Style::Default, settings);
    settings = pWindow->getSettings();
    std::cout << "Depth bits: " << settings.depthBits << std::endl;
    std::cout << "Stencil bits: " << settings.stencilBits << std::endl;
    std::cout << "Antialiasing level: " << settings.antialiasingLevel << std::endl;
    std::cout << "Version: " << settings.majorVersion << "." << settings.minorVersion << std::endl;

    pWindow->setActive(true);
    gladLoadGL();

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

        render();
        pWindow->display();
        
        frames++;
    }

    clean();
    pWindow->setActive(false);
    
    delete pWindow;
    pWindow = nullptr;

    return EXIT_SUCCESS;
}