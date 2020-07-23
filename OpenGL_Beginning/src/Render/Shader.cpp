#include "Renderer.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Shader.h"

Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetUniform4f(const std::string& name, float vx, float vy, float vz, float vw)
{
    glUniform4f(GetUniformLocation(name), vx, vy, vz, vw);
}

void Shader::SetUniform2i(const std::string& name, int vx, int vy)
{
    glUniform2i(GetUniformLocation(name), vx, vy);
}

void Shader::SetUniformMatrix4f(const std::string& name, int num, GLboolean transposed, const GLfloat* value)
{
    glUniformMatrix4fv(GetUniformLocation(name), num, transposed, value);
}

ShaderProgramSource Shader::ParseShader(const std::string& filePath)
{
        std::ifstream stream(filePath);

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

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
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

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
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

int Shader::GetUniformLocation(const std::string& name)
{
    if(m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    int location = glGetUniformLocation(m_RendererID, name.c_str());
    m_UniformLocationCache[name] = location;
    return location;
}
