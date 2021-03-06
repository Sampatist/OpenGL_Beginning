#pragma once
#include "GL\glew.h"
#include "GLFW/glfw3.h"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

namespace Renderer
{
	struct DrawableBox
	{
		glm::vec<3, double, glm::packed_highp> pos;
		double w;
		double h;
		double d;
		glm::vec<3, double, glm::packed_highp> color;
	};

	struct DrawableLine
	{
		glm::vec3 start;
		glm::vec3 end;
		glm::vec3 color;
	};

	struct RenderableMesh
	{
		unsigned int vboID;
		unsigned int bufferSize;
		unsigned int capacity;
		static const unsigned int INITIAL_CAPACITY = 200000;
		static const unsigned int DELTA_CAPACITY = 10000;
		RenderableMesh()
			:vboID(0), bufferSize(0), capacity(INITIAL_CAPACITY)
		{};
	};

	void initialize();
	GLFWwindow* const getWindow();
	void bufferChunks();
	void endFrame();
	void terminate();
	void draw();
	void drawDebugBox(DrawableBox box);
}