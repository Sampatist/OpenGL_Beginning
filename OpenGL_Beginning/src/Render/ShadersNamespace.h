#pragma once
#include <memory>
namespace Shaders
{
    void initialize();
    Shader& getChunkShader();
    Shader& getSunShadowMapShader();
    Shader& getBackgroundQuadShader();
    Shader& getDebugShader();
}