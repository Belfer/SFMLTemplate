#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

using Shader = unsigned int;
using Mesh = unsigned int;

class Renderer
{
public:
	static void ClearColor(float r, float g, float b, float a);
	static void ClearDepth(float d);
	static void ClearStencil(float s);
	static void ClearScreen();

	static Mesh CreateMesh(int vCount, const float* vData, int iCount, const int* iData, bool dynamic);
	static void DeleteMesh(Mesh mesh);
	static void UpdateMesh(Mesh mesh, int vCount, const float* vData, int iCount, const int* iData);

	static Shader CreateShader(const char* vSrc, const char* fSrc, const char* gSrc);
	static void DeleteShader(Shader shader);
	static void BindShader(Shader shader, int count, const char** attributes, const int* formats);
	static void SetUniform(Shader shader, const char* name, int i);
	static void SetUniform(Shader shader, const char* name, float i);
	static void SetUniform(Shader shader, const char* name, double i);
	static void SetUniform(Shader shader, const char* name, const glm::vec2& v2);
	static void SetUniform(Shader shader, const char* name, const glm::vec3& v3);
	static void SetUniform(Shader shader, const char* name, const glm::vec4& v4);
	static void SetUniform(Shader shader, const char* name, const glm::mat2& m2);
	static void SetUniform(Shader shader, const char* name, const glm::mat3& m3);
	static void SetUniform(Shader shader, const char* name, const glm::mat4& m4);

	static void DrawVertices(int primitive, int count);
	static void DrawIndexed(int primitive, int count);
};