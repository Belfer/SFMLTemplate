#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <iostream>

#include <Renderer.hpp>

sf::RenderWindow* pWindow;
constexpr auto SCR_WIDTH = 800;
constexpr auto SCR_HEIGHT = 600;

glm::vec3 sunDir;
glm::vec3 sunCol;
float sunIntensisty;

glm::vec3 camPos;
glm::vec3 camRot;

sf::Vector2i mousePos;

float moveSpeed = 50.0f;
float lookSpeed = 1.5f;

struct VertexPNCT
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 color;
    glm::vec2 texcoord;
};

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
"uniform mat4 Model;\n"
"void main()\n"
"{\n"
"   gl_Position = MVP * vec4(vPos, 1.0);\n"
"   pNormal = mat3(Model) * vNor;\n"
"   pColor = vCol;\n"
"   pTexCoord = vTex;\n"
"}\n";

static const char* pixelSrc =
"#version 330 core\n"
"precision mediump float;\n"
"in vec3 pNormal;\n"
"in vec4 pColor;\n"
"in vec2 pTexCoord;\n"
"out vec4 FragColor;\n"
"uniform sampler2D Texture;\n"
"uniform vec2 Tiling;\n"
"uniform vec3 SunDirection;\n"
"uniform vec3 SunColor;\n"
"uniform float SunIntensity;\n"
"void main()\n"
"{\n"
"   float illumination = max(dot(normalize(pNormal), -SunDirection), 0);\n"
"   vec4 albedo = texture(Texture, pTexCoord * Tiling) * pColor;\n"
"   vec3 radiosity = albedo.rgb * SunColor;\n"
"   vec3 color = (radiosity * pow(SunIntensity * 0.1, 0.5)) + (radiosity * illumination * SunIntensity);\n"
"   FragColor = vec4(color, albedo.a);\n"
"   if (FragColor.a < 0.5) discard;\n"
"}\n";

struct Model
{
    struct Mesh
    {
        Buffer vBuffer = 0;
        Buffer iBuffer = 0;
        unsigned int count = 0;
    };

    struct Material
    {
        Texture texture = 0;
    };

    std::vector<Mesh> meshes;
    std::vector<Material> materials;
};

Shader shader;
Model scene;

Model LoadModel(const std::string& dir, const std::string& filename)
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

void DrawModel()
{
}

bool init()
{
    pWindow->setVerticalSyncEnabled(true);
    //pWindow->setMouseCursorGrabbed(true);
    //pWindow->setMouseCursorVisible(false);
    //pWindow->setKeyRepeatEnabled(false);
    //sf::Mouse::setPosition(sf::Vector2i(SCR_WIDTH / 2, SCR_HEIGHT / 2), *pWindow);

    // Setup pipeline
    Renderer::SetClearColor(0.4f, 0.5f, 0.8f, 1.0f);
    Renderer::SetCull(true);
    Renderer::SetCullFace(CullFace::BACK);
    Renderer::SetFaceWinding(true);
    Renderer::SetDepthTest(true);
    Renderer::SetDepthWrite(true);
    Renderer::SetBlend(true);
    Renderer::SetBlendFunc(BlendFunc::INTERPOLATE);

    // Init variables
    sunDir = glm::normalize(glm::vec3(1, 3, -10));
    sunCol = glm::vec3(0.95f, 0.95f, 1.0f);
    sunIntensisty = 1.15f;

    camPos = glm::vec3(0.0f, 0.0f, 1.7f);
    camRot = glm::vec3(0.0f, 0.0f, 0.0f);

    mousePos = sf::Mouse::getPosition();

    // Load shaders
    shader = Renderer::CreateShader(vertexSrc, pixelSrc, nullptr);

    // Load scene
    scene = LoadModel("data/Sponza", "sponza.obj");

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
    //camPos.z = 1.7f;

    //std::cout << "CamPos: " << glm::to_string(camPos) << std::endl;
    //std::cout << "CamRot: " << glm::to_string(camRot) << std::endl;
}

void render()
{
    sf::Vector2u winSize = pWindow->getSize();
    int width = winSize.x, height = winSize.y;
    float aspect = width / (float)height;

    Renderer::ClearScreen(true, true, true);

    glm::mat4 m = glm::rotate(glm::mat4(1), glm::pi<float>() * 0.5f, glm::vec3(1, 0, 0)) * glm::scale(glm::mat4(1), glm::vec3(1.0f) * 0.1f);
    glm::mat4 v = glm::lookAt(camPos, camPos + glm::quat(camRot) * glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
    glm::mat4 p = glm::perspective(70.0f, aspect, 0.1f, 1000.0f);
    glm::mat4 mvp = p * v * m;

    int samplerLoc = 0;
    glm::vec2 tiling = glm::vec2(1, -1);
    
    for (size_t i = 0; i < scene.meshes.size(); i++)
    {
        Buffer& vBuffer = scene.meshes[i].vBuffer;
        Buffer& iBuffer = scene.meshes[i].iBuffer;

        Texture& texture = scene.materials[i].texture;

        Renderer::BindBuffer(vBuffer, false);
        Renderer::BindBuffer(iBuffer, true);

        Renderer::BindShader(shader, sizeof(formats) / sizeof(int), attributes, formats);
        Renderer::BindTexture(texture, 0);

        Renderer::SetUniform(shader, "MVP", 1, &mvp);
        Renderer::SetUniform(shader, "Model", 1, &m);
        Renderer::SetUniform(shader, "Texture", 1, &samplerLoc);
        Renderer::SetUniform(shader, "Tiling", 1, &tiling);
        Renderer::SetUniform(shader, "SunDirection", 1, &sunDir);
        Renderer::SetUniform(shader, "SunColor", 1, &sunCol);
        Renderer::SetUniform(shader, "SunIntensity", 1, &sunIntensisty);

        if (iBuffer != 0)
            Renderer::DrawIndexed(Primitive::TRIANGLES, scene.meshes[i].count);
        else
            Renderer::DrawVertices(Primitive::TRIANGLES, 0, scene.meshes[i].count);

        Renderer::DetachTexture();
        Renderer::DetachShader();
        Renderer::DetachBuffer();
    }
}

void clean()
{
    for (size_t i = 0; i < scene.meshes.size(); i++)
    {
        Renderer::DeleteTexture(1, scene.materials[i].texture);
        Renderer::DeleteBuffer(1, scene.meshes[i].iBuffer);
        Renderer::DeleteBuffer(1, scene.meshes[i].vBuffer);
    }
    
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