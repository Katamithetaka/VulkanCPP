#include "presentPipeline.hpp"





ShaderInfo PresentPipeline::getVertexShader()
{
    return ShaderInfo{"shaders/present.vert.spv", "main"};
}

ShaderInfo PresentPipeline::getFragmentShader()
{
    return ShaderInfo{"shaders/present.frag.spv", "main"};
}

std::vector<VkVertexInputBindingDescription> PresentPipeline::getBindingDescription()
{
    return {Vertex::getBindingDescription()};
}

std::vector<VkVertexInputAttributeDescription> PresentPipeline::getAttributeDescriptions()
{
    return Vertex::getAttributeDescriptions();
}
