#include "Shader.h"
#include "ShadersNamespace.h"
#include <memory>

static std::shared_ptr<Shader> chunkShader;
static std::shared_ptr<Shader> sunShadowMapShader;

void Shaders::initialize()
{
    chunkShader = std::make_shared<Shader>("res/shaders/Basic.shader");
    sunShadowMapShader = std::make_shared<Shader>("res/shaders/SunShadowMap.shader");
}

std::shared_ptr<Shader> Shaders::getChunkShader()
{
    return chunkShader;
}

std::shared_ptr<Shader> Shaders::getSunShadowMapShader()
{
    return sunShadowMapShader;
}