#include "renderPipeline.hpp"

ShaderInfo RenderPipeline::getVertexShader()
{
    return ShaderInfo{"shaders/simple.vert.spv", "main"};
}

ShaderInfo RenderPipeline::getFragmentShader()
{
    return ShaderInfo{"shaders/simple.frag.spv", "main"};
}

std::vector<VkVertexInputBindingDescription> RenderPipeline::getBindingDescription()
{
    return {Vertex::getBindingDescription()};
}

std::vector<VkVertexInputAttributeDescription> RenderPipeline::getAttributeDescriptions()
{
    return Vertex::getAttributeDescriptions();
}
