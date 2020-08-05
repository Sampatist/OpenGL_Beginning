#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define GLM_FORCE_INLINE

#include <Render/Renderer.h>

#include <Render/VertexBuffer.h>
#include <Render/IndexBuffer.h>

#include <glm/vec3.hpp>
#include <glm/vec3.hpp> // glm::vec3 
#include <glm/vec4.hpp> // glm::vec4 
#include <glm/mat4x4.hpp> // glm::mat4 
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include <input_handlers/KeyboardEventHandler.h>
#include <input_handlers/MouseEventHandler.h>

#include "Game.h"
#include "Shader.h"

#include "Settings.h"

//genTerrain TEST
#include "Chunk/TerrainGenerator.h"

int main(void)
{
    //std::this_thread::sleep_for(std::chrono::seconds(10));

    Game::initialize();

    /// 
    int frameCount = 0;
    double lastTime = glfwGetTime();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(Renderer::getWindow()))
    {
        //Frame
        frameCount+=5;
        if (glfwGetTime() - lastTime >= 0.2f)
        {
            printf("%f ms/frame, %d fps\n", 1000.0f / frameCount, frameCount);
            frameCount = 0;
            lastTime = glfwGetTime();
            ChunkManager::update();
        }
        //////

        /* Render here */

        Game::run();

        Renderer::endFrame();
    }

    Renderer::terminate();
    return 0;
}