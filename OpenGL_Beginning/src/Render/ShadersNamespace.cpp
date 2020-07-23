#include "Shader.h"
#include "ShadersNamespace.h"
#include <memory>

static std::shared_ptr<Shader> chunkShader;

void Shaders::initialize()
{
    chunkShader = std::make_shared<Shader>("res/shaders/Basic.shader");
}

std::shared_ptr<Shader> Shaders::getChunkShader()
{
    return chunkShader;
}