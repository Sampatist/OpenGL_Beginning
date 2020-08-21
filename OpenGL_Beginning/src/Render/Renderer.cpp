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
#include "Shadows.h"
#include "Game.h"
#include "Chunk/pairHash.h"

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
static std::unordered_map<std::pair<int, int>, Renderer::RenderableMesh, hash_pair> drawableMeshes;  
static std::vector<std::pair<int, int>> erasableChunkLocations;   
static unsigned int chunkIndexBufferObject = 0;    
static unsigned int sunShadowMapFramebuffer = 0;
static unsigned int backgroundQuadBufferObject = 0; 
   
void Renderer::initialize() 
{  
    /* Initialize the library */ 
    if (!glfwInit())
        return;

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1600, 900, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /*INPUT = DefaultFrameRate/FrameLimit ///// DefaultFrameRate = (60hz)*/
    
    //glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, , 1080, GLFW_DONT_CARE);
    glfwSwapInterval(1);           

    if (glewInit() != GLEW_OK)
        return;

    //glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    //Initilaize VAO
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    /*Initilaize IBO*/
    glGenBuffers(1, &chunkIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkIndexBufferObject);
    // calculate all indices for maximum possible polygons in a chunk.
    unsigned int* indices = new unsigned int[1179648];
    for (size_t i = 0; i < 196608; i++)
    {
        indices[0 + i * 6] = 0 + i * 4;
        indices[1 + i * 6] = 1 + i * 4;
        indices[2 + i * 6] = 2 + i * 4;
        indices[3 + i * 6] = 2 + i * 4;
        indices[4 + i * 6] = 3 + i * 4;
        indices[5 + i * 6] = 0 + i * 4;
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 1179648, indices, GL_STATIC_DRAW);
    delete[] indices;

    /*Initialize background VBO*/
    glGenBuffers(1, &backgroundQuadBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundQuadBufferObject);
    int backgroundVertex[18] =
    {
        -1,-1,0,
         1,-1,0,
         1, 1,0,
         1, 1,0,
        -1, 1,0,
        -1,-1,0
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertex), backgroundVertex, GL_STATIC_DRAW);

    /*Initilaize VBO's*/
    long cameraChunkX = (int)floor(Camera::GetPosition().x / CHUNK_WIDTH);

    int dummyChunkPos = cameraChunkX + Settings::viewDistance + 4;

    for (int i = 0; i < chunkCountLookup[Settings::viewDistance]; i++)
    {
        std::pair<int, int> chunkLocation(dummyChunkPos, i);
        drawableMeshes[chunkLocation] = RenderableMesh();
        ChunkManager::bufferedInfoMap[chunkLocation] = nullptr;
        glGenBuffers(1, &drawableMeshes[chunkLocation].vboID);
        glBindBuffer(GL_ARRAY_BUFFER, drawableMeshes[chunkLocation].vboID);
        glBufferData(GL_ARRAY_BUFFER, drawableMeshes[chunkLocation].capacity, nullptr, GL_STREAM_DRAW);
    }
    //Theoretical Limit: 786432
    // More than enough!!!  40000
    // 3D NOISE 600000
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Shaders::initialize();

    glm::mat4 ModelMatrix(1.0f);

    Shaders::getBackgroundQuadShader()->Bind();
    Shaders::getBackgroundQuadShader()->SetUniformMatrix4f("u_projMatrix", 1, GL_FALSE, &ProjectionMatrix(Settings::aspectRatio)[0][0]);
    Shaders::getChunkShader()->Bind();
    Shaders::getChunkShader()->SetUniformMatrix4f("u_Projection", 1, GL_FALSE, &ProjectionMatrix(Settings::aspectRatio)[0][0]);
    Shaders::getChunkShader()->SetUniformMatrix4f("u_Model", 1, GL_FALSE, &ModelMatrix[0][0]);
    Shaders::getChunkShader()->SetUniform1f("u_ChunkDistance", Settings::viewDistance);

    /*Create sun shadow map frame buffer and texture*/

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.

    glGenFramebuffers(1, &sunShadowMapFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, sunShadowMapFramebuffer);

    // The texture we're going to render to
    GLuint sunShadowMapTexture;
    glGenTextures(1, &sunShadowMapTexture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, sunShadowMapTexture);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 4096, 4096, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    // Specify texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, sunShadowMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sunShadowMapTexture);

    Shaders::getChunkShader()->SetUniform1i("u_SunShadowTexture", 0);
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

/* Yama yapildi, Maybe we can make the cameraPosition static for every buffer. */

void Renderer::bufferChunks()
{
    for(auto& pair : drawableMeshes)
    {
        RenderableMesh& drawableMesh = pair.second;
        auto& chunkLocation = pair.first;

        if(isFar(chunkLocation.first, chunkLocation.second))
        {
            ChunkManager::bufferMapLock.lock();
            ChunkManager::bufferedInfoMap.erase(chunkLocation);
            ChunkManager::bufferMapLock.unlock();

            ChunkManager::meshLock.lock();
            if(ChunkManager::chunkMeshes.empty())
            {
                ChunkManager::meshLock.unlock();
                break;
			}
            
            MeshGenerator::Mesh& frontMesh = ChunkManager::chunkMeshes.front();
            std::pair<int, int> incomingChunkLocation(frontMesh.x, frontMesh.z);
            
            if(drawableMeshes.count(incomingChunkLocation))
            {
                ChunkManager::chunkMeshes.erase(ChunkManager::chunkMeshes.begin());
                std::cout << "Mesh exists, erasing mesh.\n";
			}
            else
            {
                // buffer mesh data to buffer
                drawableMeshes[incomingChunkLocation].vboID = drawableMesh.vboID;
                drawableMeshes[incomingChunkLocation].capacity = drawableMesh.capacity;
                drawableMeshes[incomingChunkLocation].bufferSize = frontMesh.mesh.size();

                erasableChunkLocations.push_back(chunkLocation);

                drawableMesh = drawableMeshes[incomingChunkLocation];

                int32_t* arr = &(frontMesh.mesh[0]);
                int bufferSizeBytes = drawableMesh.bufferSize * 4;
                if(glm::abs(int(drawableMesh.capacity) - int(RenderableMesh::DELTA_CAPACITY)/2 - bufferSizeBytes) > int(RenderableMesh::DELTA_CAPACITY/2))          
                {          
                    int extraCapacity = (bufferSizeBytes - int(drawableMesh.capacity)) / int(RenderableMesh::DELTA_CAPACITY) + (int(drawableMesh.capacity) < bufferSizeBytes);
                    drawableMesh.capacity += RenderableMesh::DELTA_CAPACITY * extraCapacity;
                    glBindBuffer(GL_ARRAY_BUFFER, drawableMesh.vboID);
                    glBufferData(GL_ARRAY_BUFFER, drawableMesh.capacity, nullptr, GL_STREAM_DRAW);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSizeBytes, arr);
				}
                else
                {
                    glBindBuffer(GL_ARRAY_BUFFER, drawableMesh.vboID);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, drawableMesh.bufferSize * sizeof(int32_t), arr);
				}
                ChunkManager::chunkMeshes.erase(ChunkManager::chunkMeshes.begin());
                ChunkManager::bufferMapLock.lock();
                ChunkManager::bufferedInfoMap[incomingChunkLocation] = nullptr;
                ChunkManager::bufferMapLock.unlock();
			}
            ChunkManager::meshLock.unlock();
		}
	}
    for (auto it = begin(erasableChunkLocations); it != end(erasableChunkLocations); ++it)
    {
        drawableMeshes.erase(*it);
    }
    erasableChunkLocations.clear();
    std::cout << drawableMeshes.size() << std::endl;
    ChunkManager::meshLock.lock();
    std::cout << ChunkManager::chunkMeshes.size() << std::endl;
    ChunkManager::meshLock.unlock();

 //   for(int i = lastIndex; i < drawableMeshes.size(); i++)
 //   {
 //       lastIndex = (lastIndex + 1) % drawableMeshes.size();
 //       if (isFar(drawableMeshes[i].chunkX, drawableMeshes[i].chunkZ))
 //       {
 //           ChunkManager::meshLock.lock();
 //           if (ChunkManager::chunkMeshes.empty())
 //           {
 //               ChunkManager::meshLock.unlock();
 //               break;
 //           }
 //           MeshGenerator::Mesh& frontMesh = ChunkManager::chunkMeshes.front();
 //           // find_if uzun suruyo olabilir
 //           // unordered map kullanilip O(1) lookupdan 
 //           // yararlanilabilir...
 //           if(std::find_if(drawableMeshes.begin(), drawableMeshes.end(), 
 //               [&](RenderableMesh& rm) { return rm.chunkX == frontMesh.x && rm.chunkZ == frontMesh.z; }) != drawableMeshes.end()) {
 //               ChunkManager::chunkMeshes.erase(ChunkManager::chunkMeshes.begin());
 //               std::cout << "Mesh exists, erasing mesh.\n";
 //           }
 //           else{
 //               drawableMeshes[i].bufferSize = frontMesh.mesh.size();
 //               drawableMeshes[i].chunkX = frontMesh.x;
 //               drawableMeshes[i].chunkZ = frontMesh.z;
 //               int32_t* arr = &(frontMesh.mesh[0]);
 //               int bufferSizeBytes = drawableMeshes[i].bufferSize * 4;
 //               // do this
 //               if(glm::abs(drawableMeshes[i].capacity - 5000 - bufferSizeBytes) > 5000)          
 //               {
 //                   unsigned int tempVboID = 0;
 //                   int extraCapacity = int(bufferSizeBytes - drawableMeshes[i].capacity) / 10000 + (drawableMeshes[i].capacity < bufferSizeBytes);
 //                   //printf("%d %d %d\n", drawableMeshes[i].capacity, bufferSizeBytes, extraCapacity);
 //                   drawableMeshes[i].capacity += 10000 * extraCapacity;
 //                   glGenBuffers(1, &tempVboID);
 //                   glBindBuffer(GL_ARRAY_BUFFER, tempVboID);
 //                   glBufferData(GL_ARRAY_BUFFER, drawableMeshes[i].capacity, nullptr, GL_STREAM_DRAW);
 //                   glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSizeBytes, arr);
 //                   glDeleteBuffers(1, &drawableMeshes[i].vboID);
 //                   drawableMeshes[i].vboID = tempVboID;
	//			}
 //               else
 //               {
 //                   glBindBuffer(GL_ARRAY_BUFFER, drawableMeshes[i].vboID);
 //                   glBufferSubData(GL_ARRAY_BUFFER, 0, drawableMeshes[i].bufferSize * sizeof(int32_t), arr);
	//			}
 //               ChunkManager::chunkMeshes.erase(ChunkManager::chunkMeshes.begin());
 //           }
 //           ChunkManager::meshLock.unlock();
 //           //if (++bufferOperations == 100)
 //           //    break;
 //       }
	//}
}

void Renderer::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Render Sun
    Sun::SetDirections(Game::getGameTime());
    glm::vec3 lightDir = Sun::GetDirection();
    glm::vec3 lightDirForw = Sun::GetDirectionForw();
    glm::vec3 lightDirBackw = Sun::GetDirectionBackw();

    Shaders::getChunkShader()->SetUniform3f("u_lightDir", lightDir.x, lightDir.y, lightDir.z);
    Shaders::getChunkShader()->SetUniform3f("u_lightDirForw", lightDirForw.x, lightDirForw.y, lightDirForw.z);
    Shaders::getChunkShader()->SetUniform3f("u_lightDirBackw", lightDirBackw.x, lightDirBackw.y, lightDirBackw.z);

    Shaders::getChunkShader()->SetUniformMatrix4f("u_View", 1, GL_FALSE, &ViewMatrix()[0][0]);
    auto camPos = Camera::GetPosition();
    Shaders::getChunkShader()->SetUniform3f("u_CamPos", camPos.x, camPos.y, camPos.z);

    Shaders::getSunShadowMapShader()->Bind();
    glm::mat4 svpm = Shadows::calculateSunVPMatrix();
    Shaders::getSunShadowMapShader()->SetUniformMatrix4f("u_SunViewProjectionMatrix", 1, GL_FALSE, &svpm[0][0]);
    
    // Render background quad
    Shaders::getBackgroundQuadShader()->Bind();
    auto camDir = Camera::GetCameraAngle();
    Shaders::getBackgroundQuadShader()->SetUniform3f("u_CamPos", camPos.x, camPos.y, camPos.z);
    Shaders::getBackgroundQuadShader()->SetUniform3f("u_lightDir", lightDir.x, lightDir.y, lightDir.z);
    Shaders::getBackgroundQuadShader()->SetUniformMatrix4f("u_viewMatrix", 1, GL_FALSE, &ViewMatrix()[0][0]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1600, 900);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundQuadBufferObject);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, 3 * sizeof(int), nullptr);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Render to shadow map frame buffer
    Shaders::getSunShadowMapShader()->Bind();
    glBindFramebuffer(GL_FRAMEBUFFER, sunShadowMapFramebuffer);
    glViewport(0, 0, 4096, 4096);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    for (auto pair : drawableMeshes)
    {
        std::pair<int, int> chunkLocation = pair.first;
        RenderableMesh& drawableMesh = pair.second;

        if (drawableMesh.bufferSize == 0)
            continue;
        Shaders::getSunShadowMapShader()->SetUniform2i("u_ChunkOffset", chunkLocation.first * CHUNK_WIDTH, chunkLocation.second * CHUNK_LENGTH);
        glBindBuffer(GL_ARRAY_BUFFER, drawableMesh.vboID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkIndexBufferObject);
        glEnableVertexAttribArray(0);
        glVertexAttribIPointer(0, 1, GL_INT, sizeof(int), 0);
        glDrawElements(GL_TRIANGLES, drawableMesh.bufferSize * 3 / 2, GL_UNSIGNED_INT, nullptr);
    }     
       
    Shaders::getChunkShader()->Bind();
    Shaders::getChunkShader()->SetUniformMatrix4f("u_SunViewProjectionMatrix", 1, GL_FALSE, &svpm[0][0]);
    // Render to the screen

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1600, 900);

    for (auto pair : drawableMeshes)
    {
        std::pair<int, int> chunkLocation = pair.first;
        RenderableMesh& drawableMesh = pair.second;

        if (drawableMesh.bufferSize == 0)
            continue;
        Shaders::getChunkShader()->SetUniform2i("u_ChunkOffset", chunkLocation.first * CHUNK_WIDTH, chunkLocation.second * CHUNK_LENGTH);
        glBindBuffer(GL_ARRAY_BUFFER, drawableMesh.vboID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkIndexBufferObject);
        glEnableVertexAttribArray(0);
        glVertexAttribIPointer(0, 1, GL_INT, sizeof(int), 0);
        glDrawElements(GL_TRIANGLES, drawableMesh.bufferSize * 3 / 2, GL_UNSIGNED_INT, nullptr);
    }
}
