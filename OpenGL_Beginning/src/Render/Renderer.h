#pragma once
#include "GL\glew.h"
#include "GLFW/glfw3.h"

namespace Renderer
{
	struct RenderableMesh
	{
		unsigned int vboID;
		unsigned int bufferSize;
		int chunkX;
		int chunkZ;
		RenderableMesh(unsigned int vboID, unsigned int bufferSize, int chunkX, int chunkZ)
			:vboID(vboID), bufferSize(bufferSize), chunkX(chunkX), chunkZ(chunkZ)
		{};
	};
	void initialize();
	GLFWwindow* const getWindow();
	void bufferChunks();
	void endFrame();
	void terminate();
	void draw();
}