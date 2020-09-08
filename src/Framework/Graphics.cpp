#include "Graphics.hpp"

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
    ASSERT(shader != 0);

    int success;
    char infoLog[512];

    if (!linked)
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "COMPILATION_FAILED:\n" << infoLog << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 512, NULL, infoLog);
            std::cout << "LINKING_FAILED:\n" << infoLog << std::endl;
        }
    }

    return success;
}

void Graphics::Initialize()
{
    gladLoadGL();
}

void Graphics::SetViewport(float x, float y, float width, float height)
{
    glViewport(x, y, width, height);

    ASSERT(CheckGLError());
}

void Graphics::SetClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);

    ASSERT(CheckGLError());
}

void Graphics::SetClearDepth(float depth)
{
    glClearDepth(depth);

    ASSERT(CheckGLError());
}

void Graphics::SetClearStencil(unsigned int mask)
{
    glStencilMask(mask);

    ASSERT(CheckGLError());
}

void Graphics::ClearScreen(bool color, bool depth, bool stencil)
{
    GLbitfield mask = 0;
    mask |= color ? GL_COLOR_BUFFER_BIT : 0;
    mask |= depth ? GL_DEPTH_BUFFER_BIT : 0;
    mask |= stencil ? GL_STENCIL_BUFFER_BIT : 0;
    glClear(mask);

    ASSERT(CheckGLError());
}

void Graphics::SetCull(bool enable)
{
    if (enable)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    ASSERT(CheckGLError());
}

void Graphics::SetCullFace(CullFace face)
{
    switch (face)
    {
    case CullFace::FRONT: glCullFace(GL_FRONT); break;
    case CullFace::BACK: glCullFace(GL_BACK); break;
    case CullFace::BOTH: glCullFace(GL_FRONT_AND_BACK); break;
    }

    ASSERT(CheckGLError());
}

void Graphics::SetFaceWinding(bool ccw)
{
    if (ccw)
        glFrontFace(GL_CCW);
    else
        glFrontFace(GL_CW);

    ASSERT(CheckGLError());
}

void Graphics::SetBlend(bool enable)
{
    if (enable)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);

    ASSERT(CheckGLError());
}

void Graphics::SetBlendFunc(BlendFunc func)
{
    switch (func)
    {
    case BlendFunc::ADD: glBlendFunc(GL_ONE, GL_ONE); break;
    case BlendFunc::MULTIPLY: glBlendFunc(GL_DST_COLOR, GL_ZERO); break;
    case BlendFunc::INTERPOLATE: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
    }

    ASSERT(CheckGLError());
}

void Graphics::SetDepthTest(bool enable)
{
    if (enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    ASSERT(CheckGLError());
}

void Graphics::SetDepthWrite(bool enable)
{
    glDepthMask(enable);

    ASSERT(CheckGLError());
}

void Graphics::SetDepthFunc(DepthFunc func)
{
    switch (func)
    {
    case DepthFunc::NEVER: glDepthFunc(GL_NEVER); break;
    case DepthFunc::LESS: glDepthFunc(GL_LESS); break;
    case DepthFunc::EQUAL: glDepthFunc(GL_EQUAL); break;
    case DepthFunc::LEQUAL: glDepthFunc(GL_LEQUAL); break;
    case DepthFunc::GREATER: glDepthFunc(GL_GREATER); break;
    case DepthFunc::NOTEQUAL: glDepthFunc(GL_NOTEQUAL); break;
    case DepthFunc::GEQUAL: glDepthFunc(GL_GEQUAL); break;
    case DepthFunc::ALWAYS: glDepthFunc(GL_ALWAYS); break;
    }

    ASSERT(CheckGLError());
}

void Graphics::SetSmoothing(bool enable)
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

Buffer Graphics::CreateBuffer(int bufferCount, int dataCount, const void* data, bool index, bool dynamic)
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

void Graphics::DeleteBuffer(int count, Buffer buffer)
{
    ASSERT(buffer != 0);
    glDeleteBuffers(count, &buffer);
    buffer = 0;

    ASSERT(CheckGLError());
}

void Graphics::UpdateBuffer(Buffer buffer, int count, const void* data, bool index)
{
    ASSERT(buffer != 0);
    // TODO

    ASSERT(CheckGLError());
}

void Graphics::BindBuffer(Buffer buffer, bool index)
{
    ASSERT(buffer != 0);

    if (index)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    else
        glBindBuffer(GL_ARRAY_BUFFER, buffer);

    ASSERT(CheckGLError());
}

void Graphics::DetachBuffer()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    ASSERT(CheckGLError());
}

Shader Graphics::CreateShader(const char* vSrc, const char* pSrc, const char* gSrc)
{
    ASSERT(vSrc != nullptr);
    ASSERT(pSrc != nullptr);

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

void Graphics::DeleteShader(Shader shader)
{
    ASSERT(shader != 0);
    glDeleteShader(shader);

    ASSERT(CheckGLError());
}

void Graphics::BindShader(Shader shader, const std::vector<AttributeFormat>& attributeFormat)
{
    ASSERT(shader != 0);
    glUseProgram(shader);

    int stride = 0;
    for (int i = 0; i < attributeFormat.size(); ++i)
        stride += attributeFormat[i].format;
    stride *= sizeof(float);

    int offset = 0;
    for (int i = 0; i < attributeFormat.size(); ++i)
    {
        int loc = glGetAttribLocation(shader, attributeFormat[i].attribute.c_str());
        if (loc != -1)
        {
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc, attributeFormat[i].format, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * offset));
        }
        offset += attributeFormat[i].format;
    }

    ASSERT(CheckGLError());
}

void Graphics::DetachShader()
{
    glUseProgram(0);

    ASSERT(CheckGLError());
}

void Graphics::SetUniform(Shader shader, const char* name, int count, int* i)
{
    ASSERT(shader != 0);
    glUniform1iv(glGetUniformLocation(shader, name), count, i);

    ASSERT(CheckGLError());
}

void Graphics::SetUniform(Shader shader, const char* name, int count, float* f)
{
    ASSERT(shader != 0);
    glUniform1fv(glGetUniformLocation(shader, name), count, f);

    ASSERT(CheckGLError());
}

void Graphics::SetUniform(Shader shader, const char* name, int count, glm::vec2* v2)
{
    ASSERT(shader != 0);
    glUniform2fv(glGetUniformLocation(shader, name), count, glm::value_ptr(v2[0]));

    ASSERT(CheckGLError());
}

void Graphics::SetUniform(Shader shader, const char* name, int count, glm::vec3* v3)
{
    ASSERT(shader != 0);
    glUniform3fv(glGetUniformLocation(shader, name), count, glm::value_ptr(v3[0]));

    ASSERT(CheckGLError());
}

void Graphics::SetUniform(Shader shader, const char* name, int count, glm::vec4* v4)
{
    ASSERT(shader != 0);
    glUniform4fv(glGetUniformLocation(shader, name), count, glm::value_ptr(v4[0]));

    ASSERT(CheckGLError());
}

void Graphics::SetUniform(Shader shader, const char* name, int count, glm::mat2* m2)
{
    ASSERT(shader != 0);
    glUniformMatrix2fv(glGetUniformLocation(shader, name), count, GL_FALSE, glm::value_ptr(m2[0]));

    ASSERT(CheckGLError());
}

void Graphics::SetUniform(Shader shader, const char* name, int count, glm::mat3* m3)
{
    ASSERT(shader != 0);
    glUniformMatrix3fv(glGetUniformLocation(shader, name), count, GL_FALSE, glm::value_ptr(m3[0]));

    ASSERT(CheckGLError());
}

void Graphics::SetUniform(Shader shader, const char* name, int count, glm::mat4* m4)
{
    ASSERT(shader != 0);
    glUniformMatrix4fv(glGetUniformLocation(shader, name), count, GL_FALSE, glm::value_ptr(m4[0]));

    ASSERT(CheckGLError());
}

void Graphics::DrawVertices(Primitive primitive, int offset, int count)
{
    switch (primitive)
    {
    case Primitive::POINTS: glDrawArrays(GL_POINTS, offset, count); break;
    case Primitive::LINES: glDrawArrays(GL_LINES, offset, count); break;
    case Primitive::TRIANGLES: glDrawArrays(GL_TRIANGLES, offset, count); break;
    }

    ASSERT(CheckGLError());
}

void Graphics::DrawIndexed(Primitive primitive, int count)
{
    switch (primitive)
    {
    case Primitive::POINTS: glDrawElements(GL_POINTS, count, GL_UNSIGNED_INT, 0); break;
    case Primitive::LINES: glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, 0); break;
    case Primitive::TRIANGLES: glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0); break;
    }

    ASSERT(CheckGLError());
}

Texture Graphics::CreateTexture(TextureFormat format, int count, int width, int height, const void* data, bool mipmap)
{
    Texture texture;
    glGenTextures(count, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    switch (format)
    {
    case TextureFormat::RBG24: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); break;
    case TextureFormat::RBGA32: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); break;
    }
    
    if (mipmap) glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
    ASSERT(CheckGLError());
}

void Graphics::DeleteTexture(int count, Texture texture)
{
    ASSERT(texture != 0);
    glDeleteTextures(count, &texture);
    texture = 0;

    ASSERT(CheckGLError());
}

void Graphics::FilterTexture(Texture texture, TextureWrap s, TextureWrap t, TextureFilter min, TextureFilter mag)
{
    ASSERT(texture != 0);
    glBindTexture(GL_TEXTURE_2D, texture);

    switch (s)
    {
    case TextureWrap::REPEAT: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); break;
    case TextureWrap::MIRROR: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); break;
    case TextureWrap::EDGE_CLAMP: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); break;
    case TextureWrap::BORDER_CLAMP: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); break;
    }

    switch (t)
    {
    case TextureWrap::REPEAT: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); break;
    case TextureWrap::MIRROR: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT); break;
    case TextureWrap::EDGE_CLAMP: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); break;
    case TextureWrap::BORDER_CLAMP: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); break;
    }

    switch (min)
    {
    case TextureFilter::NEAREST: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); break;
    case TextureFilter::LINEAR: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;

    case TextureFilter::NEAREST_NEAREST: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); break;
    case TextureFilter::NEAREST_LINEAR: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); break;
    case TextureFilter::LINEAR_NEAREST: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); break;
    case TextureFilter::LINEAR_LINEAR: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); break;
    }

    switch (mag)
    {
    case TextureFilter::NEAREST: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); break;
    case TextureFilter::LINEAR: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); break;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    ASSERT(CheckGLError());
}

void Graphics::BindTexture(Texture texture, int loc)
{
    ASSERT(texture != 0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE0 + loc);

    ASSERT(CheckGLError());
}

void Graphics::DetachTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);

    ASSERT(CheckGLError());
}