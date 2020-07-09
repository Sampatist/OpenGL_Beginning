#include <GL\glew.h>
#include <GLFW/glfw3.h>

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

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = 0, VERTEX = 1, FRAGMENT = 2
    };

    std::string line;
    std::stringstream ss[3];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << "\n";
        }
    }
    return { ss[1].str(), ss[2].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length; 
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)_malloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex " : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
        
}


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);                                /****************************************************************************************/

    if (glewInit() != GLEW_OK)
        return -1;
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

        ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
        /*std::cout << "VERTEX" << std::endl;
        std::cout << source.VertexSource << std::endl;
        std::cout << "FRAGMENT" << std::endl;
        std::cout << source.FragmentSource << std::endl;
        */
        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);


        int location = glGetUniformLocation(shader, "u_Color");



        float r = 0.0f;
        float increment = 0.05f;

        int Location_Model = glGetUniformLocation(shader, "u_Model");
        int Location_View = glGetUniformLocation(shader, "u_View");
        int Location_Projection = glGetUniformLocation(shader, "u_Projection");

        glm::mat4x4 ModelMatrix(1);
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
        
        Camera Camera_(glm::vec3(0, 0, 1), glm::vec3(0, 0, -1));

        glUseProgram(shader);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            glUniform4f(location, r, 1.0f - r, 1.0f + r / 2, 1.0f);

            ModelMatrix = glm::rotate(ModelMatrix, glm::radians(2.0f+r), glm::vec3(1, 1, 1));
            glUniformMatrix4fv(Location_Model, 1, GL_FALSE, &ModelMatrix[0][0]);

            glUniformMatrix4fv(Location_View, 1, GL_FALSE, &ViewMatrix(Camera_)[0][0]);

            glUniformMatrix4fv(Location_Projection, 1, GL_FALSE, &ProjectionMatrix(45,4/3.0f)[0][0]);


            glBindVertexArray(vao);
            ib.Bind();

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

            if (r > 1.0f)
                increment = -0.01f;
            else if (r < 0.0f)
                increment = 0.01f;


            r += increment;

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

        glDeleteProgram(shader);
    }
    glfwTerminate();
    return 0;
}