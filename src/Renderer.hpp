#pragma once

#include <glm/glm.hpp>

using Buffer = unsigned int;
using Shader = unsigned int;
using Texture = unsigned int;

enum struct Primitive { POINTS, LINES, TRIANGLES };

enum struct CullFace { FRONT, BACK, BOTH };

enum struct BlendFunc { ADD, MULTIPLY, INTERPOLATE };

enum struct DepthFunc { NEVER, LESS, EQUAL, LEQUAL, GREATER, NOTEQUAL, GEQUAL, ALWAYS };

class Renderer
{
public:
	static void SetViewport(float x, float y, float width, float height);
	static void SetClearColor(float r, float g, float b, float a);
	static void SetClearDepth(float depth);
	static void SetClearStencil(unsigned int mask);
	static void ClearScreen(bool color, bool depth, bool stencil);

	static void SetCull(bool enable);
	static void SetCullFace(CullFace face);
	static void SetFaceWinding(bool ccw);
	static void SetBlend(bool enable);
	static void SetBlendFunc(BlendFunc func);
	static void SetDepthTest(bool enable);
	static void SetDepthWrite(bool enable);
	static void SetDepthFunc(DepthFunc func);
	static void SetSmoothing(bool enable);
	
	static Buffer CreateBuffer(int bufferCount, int dataCount, const void* data, bool index, bool dynamic);
	static void DeleteBuffer(int count, Buffer buffer);
	static void UpdateBuffer(Buffer buffer, int count, const void* data, bool index);
	static void BindBuffer(Buffer buffer, bool index);
	static void DetachBuffer();

	static Shader CreateShader(const char* vSrc, const char* pSrc, const char* gSrc);
	static void DeleteShader(Shader shader);
	static void BindShader(Shader shader, int count, const char** attributes, const int* formats);
	static void DetachShader();
	static void SetUniform(Shader shader, const char* name, int count, int* i);
	static void SetUniform(Shader shader, const char* name, int count, float* f);
	static void SetUniform(Shader shader, const char* name, int count, glm::vec2* v2);
	static void SetUniform(Shader shader, const char* name, int count, glm::vec3* v3);
	static void SetUniform(Shader shader, const char* name, int count, glm::vec4* v4);
	static void SetUniform(Shader shader, const char* name, int count, glm::mat2* m2);
	static void SetUniform(Shader shader, const char* name, int count, glm::mat3* m3);
	static void SetUniform(Shader shader, const char* name, int count, glm::mat4* m4);

	static void DrawVertices(Primitive primitive, int offset, int count);
	static void DrawIndexed(Primitive primitive, int count);
};