#pragma once
#include "graphicsPipeline.hpp"

class PresentPipeline : public GraphicsPipeline
{

    virtual ShaderInfo getVertexShader() override;
    virtual ShaderInfo getFragmentShader() override;

    virtual std::vector<VkVertexInputBindingDescription> getBindingDescription() override;
    virtual std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() override; 

public:
    PresentPipeline() {}
    virtual ~PresentPipeline() {}


    struct Vertex
    {
        glm::vec2 pos;
        glm::vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
        {
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

            attributeDescriptions.resize(2, {});


            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, texCoord);

            return attributeDescriptions;
        }
    };


};
