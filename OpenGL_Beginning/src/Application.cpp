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

#include <View.h>
#include "Game.h"
#include "Shader.h"

#include "Settings.h"


//genTerrain TEST
#include "Chunk/TerrainGenerator.h"


int main(void)
{
    
    Renderer::initialize();
    Game::initialize();

    {
        float positions[] = {
            -0.5f, -0.5f,  0.0f, //0
             0.5f, -0.5f,  0.0f, //1
             0.5f,  0.5f,  0.0f, //2
            -0.5f,  0.5f,  0.0f, //3

            -0.5f, -0.5f,  -1.0f, //4
             0.5f, -0.5f,  -1.0f, //5
             0.5f,  0.5f,  -1.0f, //6
            -0.5f,  0.5f,  -1.0f, //7
        };

        unsigned int indicies[] = {
            0,1,2,
            2,3,0,

            1,5,6,
            6,2,1,

            3,2,6,
            6,7,3,

            0,4,7,
            7,3,0,

            0,1,5,
            5,4,0,

            4,5,6,
            6,7,4,
        };

        unsigned int vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        VertexBuffer vb(positions, sizeof(positions));

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

        IndexBuffer ib(indicies, 6 * 6);

        //SHADER
        Shader shader("res/shaders/Basic.shader");
        shader.Bind();

        shader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);

        /// 
        int frameCount = 0;
        int lastTime = glfwGetTime();
        ///

        //std::array<char, CHUNK_WIDTH* CHUNK_LENGTH* CHUNK_HEIGHT>* heapBlocks =
        //    new std::array<char, CHUNK_WIDTH* CHUNK_LENGTH* CHUNK_HEIGHT>;
        //heapBlocks[0] = TerrainGenerator::generateTerrain(0, 0, 0);

        //for (int i = 0; i <= 15; i++) {
        //    std::cout << (int)heapBlocks->at(INDEX(i,0,0));
        //}

        std::vector<Chunk> chunks;
        for(int i = 0; i < 21; i++)
        {
            for (int j = 0; j < 21; j++)
                chunks.emplace_back(i, j, 0);
		}

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(Renderer::getWindow()))
        {
                   
            //Frame
          //  frameCount++;
          //  if (glfwGetTime() - lastTime >= 1.0f)
          //  {
          //      printf("%f ms/frame, %d fps\n", 1000.0f / frameCount, frameCount);
          //      frameCount = 0;
          //      lastTime = glfwGetTime();
          //  }
            //////

            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            Game::run();

            shader.SetUniformMatrix4f("u_View", 1, GL_FALSE, &ViewMatrix(Game::getcamcont().getCamera())[0][0]);
            shader.SetUniformMatrix4f("u_Projection", 1, GL_FALSE, &ProjectionMatrix(45, 4 / 3.0f)[0][0]);

            glBindVertexArray(vao);
            ib.Bind();

            glm::mat4x4 ModelMatrix(1.0f);
            ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
            shader.SetUniformMatrix4f("u_Model", 1, GL_FALSE, &ModelMatrix[0][0]);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

            ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, -3.0f));
            shader.SetUniformMatrix4f("u_Model", 1, GL_FALSE, &ModelMatrix[0][0]);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

            Renderer::endFrame();
        }
    }
    Renderer::terminate();
    return 0;
}