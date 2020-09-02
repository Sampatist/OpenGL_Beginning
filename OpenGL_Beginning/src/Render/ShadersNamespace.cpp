#include "Shader.h"
#include "ShadersNamespace.h"
#include <memory>

static std::unique_ptr<Shader> chunkShader;
static std::unique_ptr<Shader> sunShadowMapShader;
static std::unique_ptr<Shader> backgroundQuadShader;
static std::unique_ptr<Shader> debugShader;

void Shaders::initialize()
{
    chunkShader = std::make_unique<Shader>("res/shaders/Basic.shader");
    sunShadowMapShader = std::make_unique<Shader>("res/shaders/SunShadowMap.shader");
    backgroundQuadShader = std::make_unique<Shader>("res/shaders/BackgroundQuad.shader");
    debugShader = std::make_unique<Shader>("res/shaders/Debug.shader");
}

Shader& Shaders::getChunkShader()
{
    return *chunkShader;
}

Shader& Shaders::getSunShadowMapShader()
{
    return *sunShadowMapShader;
}

Shader& Shaders::getBackgroundQuadShader()
{
    return *backgroundQuadShader;
}

Shader& Shaders::getDebugShader()
{
    return *debugShader;
}