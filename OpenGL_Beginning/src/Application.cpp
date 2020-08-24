#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define GLM_FORCE_INLINE

#include <Render/Renderer.h>
#include "Game.h"

#include <glm/vec3.hpp>
#include "glm/vec2.hpp"
#include "glm/geometric.hpp"

int main(void)
{
    //spedTest for efficient chunk drawing
    //glm::vec3 chunk (0.5f, 0.7f, 0.0f);
    //
    //std::vector<glm::vec3> hull;
    //
    //for(int i = 0; i < 8; i++)
    //{
    //    hull.push_back(glm::vec3(0.2f, 0.8f, 0.0f));
	//}
    //
    //auto start = std::chrono::system_clock::now();
    //
    //for(int i = 0; i < 5000; i++)
    //{
    //    bool inside = true;
    //    for(int i = 0; i < hull.size(); i++)
    //    {
    //        glm::vec3 a = hull[(i + 1) % hull.size()] - hull[i];
    //        glm::vec3 b = chunk - hull[i];
    //        //counterclockwise
    //        if(glm::cross(a, b).z < 0){
    //            inside = false;
    //            break;
	//		}
	//	}
    //    //render if inside
	//}
    //
    //std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now()-start;
    //std::cout << elapsed_seconds.count()*1000 << std::endl;
    //
    //return 0;
    //std::this_thread::sleep_for(std::chrono::seconds(10));

    Game::initialize();

    /// 
    int frameCount = 0;
    double lastTime = glfwGetTime();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(Renderer::getWindow()))
    {
        //Frame
        frameCount++;
        if (glfwGetTime() - lastTime >= 1.0f)
        {
            printf("%f ms/frame, %d fps\n", 1000.0f / frameCount, frameCount);
            frameCount = 0;
            lastTime = glfwGetTime();
        }
        //////

        /* Render here */

        Game::run();

        Renderer::endFrame();
    }

    Renderer::terminate();
    return 0;
}