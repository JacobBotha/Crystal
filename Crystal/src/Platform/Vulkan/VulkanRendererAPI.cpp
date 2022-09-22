#include "clpch.h"
#include "VulkanRendererAPI.h"
#include "VulkanShader.h"
#include "VulkanShaderModule.h"
#include "Crystal/Core/Application.h"

namespace Crystal {
    VulkanRendererAPI::VulkanRendererAPI()
        : m_GraphicsPipelineLayout(VK_NULL_HANDLE),
        m_GraphicsPipeline(VK_NULL_HANDLE),
        m_FramebufferResized(false),
        m_PresentReady(false)
    {}

	VulkanRendererAPI::~VulkanRendererAPI() {
        DestroyGraphicsPipeline();
    }

	void Crystal::VulkanRendererAPI::Init() {
		m_Instance = std::make_unique<VulkanInstance>();
		m_PhysicalDevice = std::make_unique<VulkanPhysicalDevice>(m_Instance.get());
		m_Surface = std::make_unique<VulkanSurface>(&Application::Get().GetWindow(), m_Instance.get());
		m_LogicalDevice = std::make_unique<VulkanLogicalDevice>(m_PhysicalDevice.get(), m_Surface.get());
		m_SwapChain = std::make_unique<VulkanSwapChain>(m_LogicalDevice.get(), m_Surface.get());
        m_RenderPass = std::make_unique<VulkanRenderPass>(
            m_LogicalDevice.get(), 
            m_SwapChain.get(), 
            VulkanRenderPass::RenderPassPipeline::Graphics);

        //Create a frame buffer for each swap chain image view. This could be
        //moved to a handler to more cleanly hold/query the framebuffers.
        CreateFramebuffers();
        m_CommandPool = std::make_unique<VulkanCommandPool>(m_LogicalDevice.get());
        m_Frames = std::make_unique<VulkanFramesHandler>(m_LogicalDevice.get(), m_CommandPool.get(), 2);

        InitRecordInfo();
	}

	void Crystal::VulkanRendererAPI::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
        float fX = static_cast<float>(x);
        float fY = static_cast<float>(y);
        float fWidth = static_cast<float>(width);
        float fHeight = static_cast<float>(height);

        m_RecordInfo.viewport = CreateViewport(fX, fY, fWidth, fHeight);
	}

	void Crystal::VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
        VkClearValue clearValue = { {color.r, color.g, color.b, color.a} };
        m_RecordInfo.clearColor = clearValue;
	}

	void VulkanRendererAPI::Clear()
	{
	}

	void VulkanRendererAPI::CreateGraphicsPipeline(GraphicsPipelineCreateInfo createInfo) {
        if (m_GraphicsPipeline != VK_NULL_HANDLE) {
            DestroyGraphicsPipeline();
        }

        CL_CORE_INFO("Creating Graphics Pipeline.");

		VulkanShader* vertexShader = (VulkanShader*) createInfo.vertexShader;
		VulkanShader* fragmentShader = (VulkanShader*) createInfo.fragmentShader;

        VkShaderModule vertModule = VulkanShaderModule::Create(vertexShader, m_LogicalDevice.get());
        VkShaderModule fragModule = VulkanShaderModule::Create(fragmentShader, m_LogicalDevice.get());

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragModule;
		fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        if (!createInfo.fillPolygon) //Enable wirefram mode
            rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

		VkPipelineDynamicStateCreateInfo dynamicState{};
        //This could be passed as a parameter instead (since there are other possible dynamic states)
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
        if (createInfo.dynamicViewport) {
			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
			dynamicState.pDynamicStates = dynamicStates.data();
        }
        else {
            viewportState.pViewports = &m_RecordInfo.viewport;
            viewportState.pScissors = &m_RecordInfo.scissor;
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        VkResult err = vkCreatePipelineLayout(m_LogicalDevice->GetVkDevice(), &pipelineLayoutInfo, nullptr, &m_GraphicsPipelineLayout);
        CL_CORE_ASSERT(err == VK_SUCCESS, "Failed to create graphics pipeline layout!");

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = m_GraphicsPipelineLayout;
        pipelineInfo.renderPass = m_RenderPass->GetVkRenderPass();
        pipelineInfo.subpass = 0;

        err = vkCreateGraphicsPipelines(m_LogicalDevice->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline);
        CL_CORE_ASSERT(err == VK_SUCCESS, "Failed to create graphics pipeline!");

        (void)err;
        VulkanShaderModule::Destroy(vertModule, m_LogicalDevice.get());
        VulkanShaderModule::Destroy(fragModule, m_LogicalDevice.get());
	}

    void VulkanRendererAPI::DrawFrame()
    {
        VkFence inFlightFence = m_Frames->GetCurrentInFlightFence();
        //Image available semaphore should most likely go into swap chain and then check if the image semaphore is available
        VkSemaphore imageAvailableSemaphore = m_Frames->GetCurrentImageAvailableSemaphore();
        VkSemaphore renderFinishedSemaphore = m_Frames->GetCurrentRenderFinishedSemaphore();

        vkWaitForFences(m_LogicalDevice->GetVkDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult err = m_SwapChain->GetNextImageIndex(imageAvailableSemaphore, &imageIndex);
        if (err == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapChain();
            return;
        }
        else CL_CORE_ASSERT(err == VK_SUCCESS || err == VK_SUBOPTIMAL_KHR, "Failed to retrieve next image from swap chain!");

        vkResetFences(m_LogicalDevice->GetVkDevice(), 1, &inFlightFence);

        VulkanCommandBuffer* commandBuffer = m_Frames->GetCurrentCommandBuffer();
        commandBuffer->Reset();

        commandBuffer->Record(m_Framebuffers[imageIndex].get(), m_GraphicsPipeline, m_RecordInfo);

        //Bellow should be seperate function
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        //Additional command buffers and semaphores go here.
        VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkCommandBuffer commandBuffers[] = { commandBuffer->GetVkCommandBuffer() };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffers;

        VkSemaphore signalSemaphores[] = { m_Frames->GetCurrentRenderFinishedSemaphore() };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        err = vkQueueSubmit(m_LogicalDevice->GetQueue(QueueFlags::Graphics), 1, &submitInfo, inFlightFence);
        CL_CORE_ASSERT(err == VK_SUCCESS, "Failed to submit to graphics queue!");
        (void)err;

        m_CurrentImageIndex = imageIndex;
    }
        //Bellow is subbmission to pressentation and should wait until the very end of the frame before executing.
	void VulkanRendererAPI::PresentFrame() {
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        VkSemaphore signalSemaphores[] = { m_Frames->GetCurrentRenderFinishedSemaphore() };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_SwapChain->GetVkSwapChain()};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_CurrentImageIndex;

		presentInfo.pResults = nullptr;

		VkResult err = vkQueuePresentKHR(m_LogicalDevice->GetQueue(QueueFlags::Present), &presentInfo);

		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR || m_FramebufferResized) {
			m_FramebufferResized = false;
			RecreateSwapChain();
		}
		else CL_CORE_ASSERT(err == VK_SUCCESS, "Failed to submit to present queue!");

		m_Frames->IterFrame();
	}

    void VulkanRendererAPI::DestroyGraphicsPipeline() {
        m_LogicalDevice->WaitGPU();
        vkDestroyPipeline(m_LogicalDevice->GetVkDevice(), m_GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(m_LogicalDevice->GetVkDevice(), m_GraphicsPipelineLayout, nullptr);
        m_GraphicsPipelineLayout = VK_NULL_HANDLE;
        m_GraphicsPipeline = VK_NULL_HANDLE;
        CL_CORE_INFO("Destroyed Graphics Pipeline!");
    }

    void VulkanRendererAPI::RecreateSwapChain() {
        m_LogicalDevice->WaitGPU();

        //Delete existing swap chain, frame buffers, and command buffers.
        m_SwapChain.reset();
        m_Framebuffers.clear();
        m_Frames.reset();

        m_SwapChain = std::make_unique<VulkanSwapChain>(m_LogicalDevice.get(), m_Surface.get());
        m_Frames = std::make_unique<VulkanFramesHandler>(m_LogicalDevice.get(), m_CommandPool.get(), 2);

        CreateFramebuffers();
        InitRecordInfo();
    }

    void VulkanRendererAPI::CreateFramebuffers() {
        for (VkImageView imageView : m_SwapChain->GetImageViews()) {
            std::unique_ptr<VulkanFramebuffer> frameBuffer = std::make_unique<VulkanFramebuffer>(
                m_LogicalDevice.get(),
                m_RenderPass.get(),
                m_SwapChain->GetVkExtent2D(),
                imageView);
            m_Framebuffers.push_back(std::move(frameBuffer));
        }
    }

    VkViewport VulkanRendererAPI::CreateViewport(float x, float y, float width, float height) {
        VkViewport viewport {};
		viewport.x = x;
		viewport.y = y;
		viewport.width = width;
        viewport.height = height;
        viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

        return viewport;
    }

    VkRect2D VulkanRendererAPI::CreateScissor(VkOffset2D offset, VkExtent2D extent) {
		VkRect2D scissor{};
		scissor.offset = offset;
		scissor.extent = extent;
        
        return scissor;
    }

    void VulkanRendererAPI::InitRecordInfo() {
        VkExtent2D extent = m_SwapChain->GetVkExtent2D();
        float width = static_cast<float>(extent.width);
        float height = static_cast<float>(extent.height);
        VkViewport viewport = CreateViewport(0.0f, 0.0f, width, height);
        VkRect2D scissor = CreateScissor({0, 0}, extent);
        
        VulkanCommandBuffer::RecordInfo recordInfo{};
        recordInfo.pipelineType = VulkanRenderPass::RenderPassPipeline::Graphics;
        recordInfo.dynamicState = true;
        recordInfo.viewport = viewport;
        recordInfo.scissor = scissor;
        recordInfo.extent = extent;

        m_RecordInfo = recordInfo;
    }
}
