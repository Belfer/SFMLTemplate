#include "Renderer.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#define ASSERT(expr)// assert(expr)

const char* GetGLError()
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
        switch (err)
        {
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        }
    return nullptr;
}

void Renderer::ClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);

    ASSERT(GetGLError() == nullptr);
}

void Renderer::ClearDepth(float d)
{
    glClear(GL_DEPTH_BUFFER_BIT);

    ASSERT(GetGLError() == nullptr);
}

void Renderer::ClearStencil(float s)
{
    glClear(GL_STENCIL_BUFFER_BIT);

    ASSERT(GetGLError() == nullptr);
}

void Renderer::ClearScreen()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    ASSERT(GetGLError() == nullptr);
}

Mesh Renderer::CreateMesh(int vCount, const float* vData, int iCount, const int* iData, bool dynamic)
{
    Mesh vBuffer;
    glGenBuffers(1, &vBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
    glBufferData(GL_ARRAY_BUFFER, vCount * sizeof(float), vData, GL_STATIC_DRAW);

    ASSERT(GetGLError() == nullptr);
    return vBuffer;
}

void Renderer::DeleteMesh(Mesh mesh)
{
    ASSERT(GetGLError() == nullptr);
}

void Renderer::UpdateMesh(Mesh mesh, int vCount, const float* vData, int iCount, const int* iData)
{
    ASSERT(GetGLError() == nullptr);
}

Shader Renderer::CreateShader(const char* vSrc, const char* fSrc, const char* gSrc)
{
    Shader vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vSrc, NULL);
    glCompileShader(vertex_shader);

    Shader fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fSrc, NULL);
    glCompileShader(fragment_shader);

    Shader program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    ASSERT(GetGLError() == nullptr);
    return program;
}

void Renderer::DeleteShader(Shader shader)
{
    ASSERT(GetGLError() == nullptr);
}

void Renderer::BindShader(Shader shader, int count, const char** attributes, const int* formats)
{
    glUseProgram(shader);

    int stride = 0;
    for (int i = 0; i < count; ++i)
        stride += formats[i];
    stride *= sizeof(float);

    int offset = 0;
    for (int i = 0; i < count; ++i)
    {
        unsigned int loc = glGetUniformLocation(shader, attributes[i]);
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, i, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * offset));
        offset += formats[i];
    }

    ASSERT(GetGLError() == nullptr);
}

void Renderer::SetUniform(Shader shader, const char* name, int i)
{
    glUseProgram(shader);

    ASSERT(GetGLError() == nullptr);
}

void Renderer::SetUniform(Shader shader, const char* name, float i)
{
    glUseProgram(shader);

    ASSERT(GetGLError() == nullptr);
}

void Renderer::SetUniform(Shader shader, const char* name, double i)
{
    glUseProgram(shader);

    ASSERT(GetGLError() == nullptr);
}

void Renderer::SetUniform(Shader shader, const char* name, const glm::vec2& v2)
{
    glUseProgram(shader);

    ASSERT(GetGLError() == nullptr);
}

void Renderer::SetUniform(Shader shader, const char* name, const glm::vec3& v3)
{
    glUseProgram(shader);

    ASSERT(GetGLError() == nullptr);
}

void Renderer::SetUniform(Shader shader, const char* name, const glm::vec4& v4)
{
    glUseProgram(shader);

    ASSERT(GetGLError() == nullptr);
}

void Renderer::SetUniform(Shader shader, const char* name, const glm::mat2& m2)
{
    glUseProgram(shader);

    ASSERT(GetGLError() == nullptr);
}

void Renderer::SetUniform(Shader shader, const char* name, const glm::mat3& m3)
{
    glUseProgram(shader);

    ASSERT(GetGLError() == nullptr);
}

void Renderer::SetUniform(Shader shader, const char* name, const glm::mat4& m4)
{
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, GL_FALSE, glm::value_ptr(m4));

    ASSERT(GetGLError() == nullptr);
}

void Renderer::DrawVertices(int primitive, int count)
{
    switch (primitive)
    {
    case 1: glDrawArrays(GL_POINTS, 0, count); break;
    case 2: glDrawArrays(GL_LINES, 0, count); break;
    case 3: glDrawArrays(GL_TRIANGLES, 0, count); break;
    }

    ASSERT(GetGLError() == nullptr);
}

void Renderer::DrawIndexed(int primitive, int count)
{
    ASSERT(GetGLError() == nullptr);
}
