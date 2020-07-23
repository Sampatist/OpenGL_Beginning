#include "Renderer.h"
#include <iostream>
#include "Chunk/ChunkManager.h"
#include "VertexBuffer.h"
#include <atomic>
#include "Camera.h"
#include "Settings.h"
#include "Shader.h"
#include <View.h>
#include "ShadersNamespace.h"
#include "Chunk/CHUNKARRAY.h"

static void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}



static GLFWwindow* window = nullptr;
static std::vector<Renderer::RenderableMesh> drawableMeshes;

void Renderer::initialize()
{
    /* Initialize the library */
    if (!glfwInit())
        return;

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /*INPUT = DefaultFrameRate/FrameLimit ///// DefaultFrameRate = (60hz)*/
    glfwSwapInterval(1);           

    if (glewInit() != GLEW_OK)
        return;

    //glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    //Initilaize VAO
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    /*Initilaize VBO's*/
    long cameraChunkX = (int)floor(Camera::GetPosition().x / CHUNK_WIDTH);

    int dummyChunkPos = cameraChunkX + Settings::viewDistance + 4;

    for (int i = 0; i < chunkCountLookup[Settings::viewDistance]; i++)
    {
        drawableMeshes.emplace_back(0,0,dummyChunkPos,0);
        glGenBuffers(1, &drawableMeshes[i].vboID);
        glBindBuffer(GL_ARRAY_BUFFER, drawableMeshes[i].vboID);
        glBufferData(GL_ARRAY_BUFFER, 786432, nullptr, GL_STREAM_DRAW);
    }

    glEnable(GL_DEPTH_TEST); 
    glEnable(GL_CULL_FACE);

    Shaders::initialize();

    glm::mat4 ModelMatrix(1.0f);

    Shaders::getChunkShader()->Bind();
    Shaders::getChunkShader()->SetUniformMatrix4f("u_Projection", 1, GL_FALSE, &ProjectionMatrix(4 / 3.0f)[0][0]);
    Shaders::getChunkShader()->SetUniformMatrix4f("u_Model", 1, GL_FALSE, &ModelMatrix[0][0]);
}

GLFWwindow* const Renderer::getWindow()
{
    return window;
}

void Renderer::endFrame()
{
    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
}
void Renderer::terminate()
{
    glfwTerminate();
}

bool isFar(int x, int z)
{
	int relativex = x - (int)floor(Camera::GetPosition().x / CHUNK_WIDTH);
	int relativez = z - (int)floor(Camera::GetPosition().z / CHUNK_LENGTH);
	return (relativex * relativex + relativez * relativez) >= (Settings::viewDistance + 1) * (Settings::viewDistance + 1);
}

static int lastIndex = 0;

void Renderer::bufferChunks()
{
    int bufferOperations = 0;
    for(int i = lastIndex; i < drawableMeshes.size(); i++)
    {
        lastIndex = (lastIndex + 1) % drawableMeshes.size();
        if (isFar(drawableMeshes[i].chunkX, drawableMeshes[i].chunkZ))
        {
            ChunkManager::meshLock.lock();
            if (ChunkManager::chunkMeshes.empty())
            {
                ChunkManager::meshLock.unlock();
                break;
            }
            drawableMeshes[i].bufferSize = ChunkManager::chunkMeshes.front().mesh.size();
            drawableMeshes[i].chunkX = ChunkManager::chunkMeshes.front().x;
            drawableMeshes[i].chunkZ = ChunkManager::chunkMeshes.front().z;
            int32_t* arr = &(ChunkManager::chunkMeshes.front().mesh[0]);
            glBindBuffer(GL_ARRAY_BUFFER, drawableMeshes[i].vboID);
            glBufferSubData(GL_ARRAY_BUFFER, 0, drawableMeshes[i].bufferSize * sizeof(int32_t), arr);
            ChunkManager::chunkMeshes.erase(ChunkManager::chunkMeshes.begin());
            ChunkManager::meshLock.unlock();
            if (++bufferOperations == 10)
                break;
        }
	}
 
 //   for(auto iter = drawableMeshes.begin(); iter != drawableMeshes.end(); iter++)
 //   {
 //       if(isFar((*iter).chunkX, (*iter).chunkZ))
 //       {
 //           ChunkManager::meshLock.lock();
 //           if (ChunkManager::chunkMeshes.empty())
 //           {
 //               ChunkManager::meshLock.unlock();
 //               break;
	//		}
 //           (*iter).bufferSize = ChunkManager::chunkMeshes.front().mesh.size();
 //           (*iter).chunkX = ChunkManager::chunkMeshes.front().x;
 //           (*iter).chunkZ = ChunkManager::chunkMeshes.front().z;
 //           int32_t* arr = &(ChunkManager::chunkMeshes.front().mesh[0]);
 //           glBindBuffer(GL_ARRAY_BUFFER, (*iter).vboID);
 //           glBufferSubData(GL_ARRAY_BUFFER, 0, (*iter).bufferSize * sizeof(int32_t), arr);
 //           ChunkManager::chunkMeshes.erase(ChunkManager::chunkMeshes.begin());
 //           ChunkManager::meshLock.unlock();
 //           if (++bufferOperations == 10)
 //               break;
 //       }
    //}
}

void Renderer::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Shaders::getChunkShader()->SetUniformMatrix4f("u_View", 1, GL_FALSE, &ViewMatrix()[0][0]);
    for (auto iter = drawableMeshes.begin(); iter != drawableMeshes.end(); iter++)
    {
        if ((*iter).bufferSize == 0)
            continue;
        Shaders::getChunkShader()->SetUniform2i("u_ChunkOffset", (*iter).chunkX * CHUNK_WIDTH, (*iter).chunkZ * CHUNK_LENGTH);
        glBindBuffer(GL_ARRAY_BUFFER, (*iter).vboID);
        glEnableVertexAttribArray(0);
        glVertexAttribIPointer(0, 1, GL_INT, sizeof(int), 0);
        glDrawArrays(GL_QUADS, 0, (*iter).bufferSize);
    }
}


