#include "clpch.h"

#include "VulkanImguiRenderer.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <GLFW/glfw3.h>

static void check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    CL_CORE_ERROR("[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

namespace Crystal {

	VulkanImGuiRenderer::VulkanImGuiRenderer() 
		: m_RendererAPI(nullptr), m_Window(nullptr), m_CommandBufferIndex(0)
	{

	}

	VulkanImGuiRenderer::~VulkanImGuiRenderer() 
	{

	}

	void VulkanImGuiRenderer::Init(RendererAPI* rendererAPI, Window* window)
	{
		if (m_RendererAPI != nullptr) {
			CL_CORE_ERROR("ImGui renderer already initialised!");
			return;
		}

		m_RendererAPI = (VulkanRendererAPI*)rendererAPI;
		m_Window = window;
		//m_Frames = std::make_unique<VulkanFramesHandler>(m_RendererAPI->GetLogicalDevice(), m_RendererAPI->GetCommandPool(), 2);
		for (uint32_t i = 0; i < m_RendererAPI->GetSwapChain()->GetImageCount(); i++)
		{
			m_CommandBuffers.push_back(std::make_unique<VulkanCommandBuffer>(m_RendererAPI->GetCommandPool()));
		}

		//Descriptor pool from ImGui demo
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = std::size(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		vkCreateDescriptorPool(m_RendererAPI->GetLogicalDevice()->GetVkDevice(), &pool_info, nullptr, &m_Pool);

        ImGui_ImplGlfw_InitForVulkan((GLFWwindow*) window->GetNativeWindow(), true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = m_RendererAPI->GetInstance()->GetVkInstance();
        init_info.PhysicalDevice = m_RendererAPI->GetPhysicalDevice()->GetVkPhysicalDevice();
        init_info.Device = m_RendererAPI->GetLogicalDevice()->GetVkDevice();
        init_info.QueueFamily = m_RendererAPI->GetLogicalDevice()->GetQueueIndex(QueueFlags::Graphics);
        init_info.Queue = m_RendererAPI->GetLogicalDevice()->GetQueue(QueueFlags::Graphics);
        //init_info.PipelineCache = g_PipelineCache;
        init_info.DescriptorPool = m_Pool;
        init_info.Subpass = 0;
		uint32_t imageCount = m_RendererAPI->GetSwapChain()->GetImageCount();
        init_info.MinImageCount = imageCount;
        init_info.ImageCount = imageCount;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info, m_RendererAPI->GetRenderPass()->GetVkRenderPass());

		// Upload Fonts
		{
			// Use any command queue
			VkCommandPool command_pool = m_RendererAPI->GetCommandPool()->GetVkCommandPool();
			VkCommandBuffer command_buffer = m_CommandBuffers[m_CommandBufferIndex]->GetVkCommandBuffer();

			VkResult err = vkResetCommandPool(m_RendererAPI->GetLogicalDevice()->GetVkDevice(), command_pool, 0);
			CL_CORE_ASSERT(err == VK_SUCCESS, "Could not reset command pool!");
			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			err = vkBeginCommandBuffer(command_buffer, &begin_info);
			CL_CORE_ASSERT(err == VK_SUCCESS, "Could not begin command buffer!");

			ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

			VkSubmitInfo end_info = {};
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &command_buffer;
			err = vkEndCommandBuffer(command_buffer);
			CL_CORE_ASSERT(err == VK_SUCCESS, "Could not begin command buffer!");
			err = vkQueueSubmit(m_RendererAPI->GetLogicalDevice()->GetQueue(QueueFlags::Graphics), 1, &end_info, VK_NULL_HANDLE);
			CL_CORE_ASSERT(err == VK_SUCCESS, "Could not sumbit to queue!");

			m_RendererAPI->GetLogicalDevice()->WaitGPU();
			ImGui_ImplVulkan_DestroyFontUploadObjects();
			CL_CORE_INFO("Debug rederer initialised.");
		}
	}

	void VulkanImGuiRenderer::NewFrame()
	{
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
	}

	void VulkanImGuiRenderer::FrameRender(void* drawData) {
		
        VkFence inFlightFence = m_RendererAPI->GetCurrentInFlightFence();
        VkSemaphore imageAvailableSemaphore = m_RendererAPI->GetCurrentImageAvailableSemaphore();
        //VkSemaphore renderFinishedSemaphore = m_Frames->GetCurrentRenderFinishedSemaphore();

        VulkanCommandBuffer* commandBuffer = m_CommandBuffers[m_CommandBufferIndex].get();
		VkDevice device = m_RendererAPI->GetLogicalDevice()->GetVkDevice();
		VulkanCommandPool* commandPool = m_RendererAPI->GetCommandPool();

        vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &inFlightFence);

		VkResult err;
        commandBuffer->Reset();
		{
			err = vkResetCommandPool(device, commandPool->GetVkCommandPool(), 0);
			check_vk_result(err);
			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			err = vkBeginCommandBuffer(commandBuffer->GetVkCommandBuffer(), &info);
			check_vk_result(err);
		}
		{
			VkRenderPassBeginInfo info = {};
			VkClearValue clearValue[] = {{{1.0f, 1.0f, 1.0f, 1.0f}}};
			VkExtent2D extent = m_RendererAPI->GetSwapChain()->GetVkExtent2D();
			VkFramebuffer framebuffer = m_RendererAPI->GetCurrentFramebuffer()->GetVkFramebuffer();

			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = m_RendererAPI->GetRenderPass()->GetVkRenderPass();
			info.framebuffer = framebuffer;
			info.renderArea.extent.width = extent.width;
			info.renderArea.extent.height = extent.height;
			info.clearValueCount = 1;
			info.pClearValues = clearValue;
			vkCmdBeginRenderPass(commandBuffer->GetVkCommandBuffer(), &info, VK_SUBPASS_CONTENTS_INLINE);
		}

		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData((ImDrawData*)drawData, commandBuffer->GetVkCommandBuffer());

		// Submit command buffer
		vkCmdEndRenderPass(commandBuffer->GetVkCommandBuffer());
		{
			VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
			VkCommandBuffer commandBuffers[] = { commandBuffer->GetVkCommandBuffer() };
			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkSubmitInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			info.waitSemaphoreCount = 0;
			info.pWaitSemaphores = nullptr;
			info.pWaitDstStageMask = &wait_stage;
			info.commandBufferCount = 1;
			info.pCommandBuffers = commandBuffers;
			info.signalSemaphoreCount = 0;
			info.pSignalSemaphores = nullptr;

			err = vkEndCommandBuffer(commandBuffer->GetVkCommandBuffer());
			check_vk_result(err);
			err = vkQueueSubmit(m_RendererAPI->GetLogicalDevice()->GetQueue(QueueFlags::Graphics), 1, &info, nullptr);
			check_vk_result(err);
		}

        //commandBuffer->Record(m_Framebuffers[imageIndex].get(), m_GraphicsPipeline, m_RecordInfo);

		++m_CommandBufferIndex;
	}

	void VulkanImGuiRenderer::Shutdown()
	{
		VulkanLogicalDevice* device = m_RendererAPI->GetLogicalDevice();
		device->WaitGPU();
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();

		vkDestroyDescriptorPool(device->GetVkDevice(), m_Pool, nullptr);
		CL_CORE_INFO("Debug rederer has shutdown.");

		m_CommandBuffers.clear();
	}
}