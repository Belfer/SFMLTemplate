#include "Renderer.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#define ASSERT(expr) assert(expr)

bool CheckGLError()
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
        switch (err)
        {
        case GL_INVALID_ENUM: std::cout << "GL_INVALID_ENUM" << std::endl; return false;
        case GL_INVALID_VALUE: std::cout << "GL_INVALID_VALUE" << std::endl; return false;
        case GL_INVALID_OPERATION: std::cout << "GL_INVALID_OPERATION" << std::endl; return false;
        case GL_INVALID_FRAMEBUFFER_OPERATION: std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl; return false;
        case GL_OUT_OF_MEMORY: std::cout << "GL_OUT_OF_MEMORY" << std::endl; return false;
        }
    return true;
}

bool CheckShaderStatus(Shader shader, bool linked)
{
    int success;
    char infoLog[512];

    if (!linked)
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "COMPILATION_FAILED:\n" << infoLog << std::endl;
            return false;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 512, NULL, infoLog);
            std::cout << "LINKING_FAILED:\n" << infoLog << std::endl;
            return false;
        }
    }

    return true;
}

void Renderer::SetViewport(float x, float y, float width, float height)
{
    glViewport(x, y, width, height);

    ASSERT(CheckGLError());
}

void Renderer::ClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);

    ASSERT(CheckGLError());
}

void Renderer::ClearDepth(float d)
{
    glClear(GL_DEPTH_BUFFER_BIT);

    ASSERT(CheckGLError());
}

void Renderer::ClearStencil(float s)
{
    glClear(GL_STENCIL_BUFFER_BIT);

    ASSERT(CheckGLError());
}

void Renderer::ClearScreen()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    ASSERT(CheckGLError());
}

void Renderer::SetCull(bool enable)
{
    if (enable)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    ASSERT(CheckGLError());
}

void Renderer::SetCullFace(CullFace face)
{
    switch (face)
    {
    case CullFace::FRONT: glCullFace(GL_FRONT); break;
    case CullFace::BACK: glCullFace(GL_BACK); break;
    case CullFace::BOTH: glCullFace(GL_FRONT_AND_BACK); break;
    }

    ASSERT(CheckGLError());
}

void Renderer::SetFaceWinding(bool ccw)
{
    if (ccw)
        glFrontFace(GL_CCW);
    else
        glFrontFace(GL_CW);

    ASSERT(CheckGLError());
}

void Renderer::SetBlend(bool enable)
{
    if (enable)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);

    ASSERT(CheckGLError());
}

void Renderer::SetBlendFunc(BlendFunc func)
{
    switch (func)
    {
    case BlendFunc::ADD: glBlendFunc(GL_ONE, GL_ONE); break;
    case BlendFunc::MULTIPLY: glBlendFunc(GL_DST_COLOR, GL_ZERO); break;
    case BlendFunc::INTERPOLATE: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
    }

    ASSERT(CheckGLError());
}

void Renderer::SetDepthTest(bool enable)
{
    if (enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    ASSERT(CheckGLError());
}

void Renderer::SetDepthWrite(bool enable)
{
    glDepthMask(enable);

    ASSERT(CheckGLError());
}

void Renderer::SetSmoothing(bool enable)
{
    if (enable)
    {
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);
    }
    else
    {
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_POLYGON_SMOOTH);
    }

    ASSERT(CheckGLError());
}

Buffer Renderer::CreateBuffer(int bufferCount, int dataCount, const void* data, bool index, bool dynamic)
{
    Buffer buffer;
    glGenBuffers(bufferCount, &buffer);
    if (index)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataCount * sizeof(unsigned int), data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, dataCount * sizeof(float), data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    ASSERT(CheckGLError());
    return buffer;
}

void Renderer::DeleteBuffer(int count, Buffer buffer)
{
    glDeleteBuffers(count, &buffer);

    ASSERT(CheckGLError());
}

void Renderer::UpdateBuffer(Buffer buffer, int count, const void* data, bool index)
{
    // TODO

    ASSERT(CheckGLError());
}

void Renderer::BindBuffer(Buffer buffer, bool index)
{
    if (index)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    else
        glBindBuffer(GL_ARRAY_BUFFER, buffer);

    ASSERT(CheckGLError());
}

void Renderer::DetachBuffer()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    ASSERT(CheckGLError());
}

Shader Renderer::CreateShader(const char* vSrc, const char* pSrc, const char* gSrc)
{
    Shader vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vSrc, NULL);
    glCompileShader(vShader);
    ASSERT(CheckShaderStatus(vShader, false));

    Shader pShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(pShader, 1, &pSrc, NULL);
    glCompileShader(pShader);
    ASSERT(CheckShaderStatus(pShader, false));

    Shader program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, pShader);
    glLinkProgram(program);
    ASSERT(CheckShaderStatus(program, true));

    glDeleteShader(vShader);
    glDeleteShader(pShader);

    ASSERT(CheckGLError());
    return program;
}

void Renderer::DeleteShader(Shader shader)
{
    glDeleteShader(shader);

    ASSERT(CheckGLError());
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
        unsigned int loc = glGetAttribLocation(shader, attributes[i]);
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, formats[i], GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * offset));
        offset += formats[i];
    }

    ASSERT(CheckGLError());
}

void Renderer::DetachShader()
{
    glUseProgram(0);

    ASSERT(CheckGLError());
}

void Renderer::SetUniform(Shader shader, const char* name, int count, int* i)
{
    glUniform1iv(glGetUniformLocation(shader, name), count, i);

    ASSERT(CheckGLError());
}

void Renderer::SetUniform(Shader shader, const char* name, int count, float* f)
{
    glUniform1fv(glGetUniformLocation(shader, name), count, f);

    ASSERT(CheckGLError());
}

void Renderer::SetUniform(Shader shader, const char* name, int count, glm::vec2* v2)
{
    glUniform2fv(glGetUniformLocation(shader, name), count, glm::value_ptr(v2[0]));

    ASSERT(CheckGLError());
}

void Renderer::SetUniform(Shader shader, const char* name, int count, glm::vec3* v3)
{
    glUniform3fv(glGetUniformLocation(shader, name), count, glm::value_ptr(v3[0]));

    ASSERT(CheckGLError());
}

void Renderer::SetUniform(Shader shader, const char* name, int count, glm::vec4* v4)
{
    glUniform4fv(glGetUniformLocation(shader, name), count, glm::value_ptr(v4[0]));

    ASSERT(CheckGLError());
}

void Renderer::SetUniform(Shader shader, const char* name, int count, glm::mat2* m2)
{
    glUniformMatrix2fv(glGetUniformLocation(shader, name), count, GL_FALSE, glm::value_ptr(m2[0]));

    ASSERT(CheckGLError());
}

void Renderer::SetUniform(Shader shader, const char* name, int count, glm::mat3* m3)
{
    glUniformMatrix3fv(glGetUniformLocation(shader, name), count, GL_FALSE, glm::value_ptr(m3[0]));

    ASSERT(CheckGLError());
}

void Renderer::SetUniform(Shader shader, const char* name, int count, glm::mat4* m4)
{
    glUniformMatrix4fv(glGetUniformLocation(shader, name), count, GL_FALSE, glm::value_ptr(m4[0]));

    ASSERT(CheckGLError());
}

void Renderer::DrawVertices(Primitive primitive, int offset, int count)
{
    switch (primitive)
    {
    case Primitive::POINTS: glDrawArrays(GL_POINTS, offset, count); break;
    case Primitive::LINES: glDrawArrays(GL_LINES, offset, count); break;
    case Primitive::TRIANGLES: glDrawArrays(GL_TRIANGLES, offset, count); break;
    }

    ASSERT(CheckGLError());
}

void Renderer::DrawIndexed(Primitive primitive, int count)
{
    switch (primitive)
    {
    case Primitive::POINTS: glDrawElements(GL_POINTS, count, GL_UNSIGNED_INT, 0); break;
    case Primitive::LINES: glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, 0); break;
    case Primitive::TRIANGLES: glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0); break;
    }

    ASSERT(CheckGLError());
}
