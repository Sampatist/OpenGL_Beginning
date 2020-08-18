#pragma once
#include "GL\glew.h"
#include "GLFW/glfw3.h"

namespace Renderer
{
	struct RenderableMesh
	{
		unsigned int vboID;
		unsigned int bufferSize;
		unsigned int capacity;
		int chunkX;
		int chunkZ;
		RenderableMesh(int chunkX, int chunkZ)
			:vboID(0), bufferSize(0), capacity(0), chunkX(chunkX), chunkZ(chunkZ)
		{};
	};
	void initialize();
	GLFWwindow* const getWindow();
	void bufferChunks();
	void endFrame();
	void terminate();
	void draw();
}