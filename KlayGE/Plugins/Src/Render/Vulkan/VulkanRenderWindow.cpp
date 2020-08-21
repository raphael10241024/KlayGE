/**
 * @file VulkanPhysicalDevice.cpp
 * @author Zhang Yakun
 *
 * @section DESCRIPTION
 *
 * This source file is part of KlayGE
 * For the latest info, see http://www.klayge.org
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * You may alternatively use this source under the terms of
 * the KlayGE Proprietary License (KPL). You can obtained such a license
 * from http://www.klayge.org/licensing/.
 */

#include <KlayGE/KlayGE.hpp>
#include <KFL/CXX17.hpp>
#include <KFL/ErrorHandling.hpp>
#include <KlayGE/SceneManager.hpp>
#include <KlayGE/Context.hpp>
#include <KlayGE/RenderFactory.hpp>
#include <KlayGE/RenderSettings.hpp>
#include <KlayGE/App3D.hpp>
#include <KlayGE/Window.hpp>
#include <KlayGE/Vulkan/VulkanRenderWindow.hpp>
#include <KlayGE/Vulkan/VulkanRenderEngine.hpp>
#include <KlayGE/Vulkan/VulkanMapping.hpp>
#include <KFL/ErrorHandling.hpp>
#include <system_error>
#include <windowsx.h>

namespace KlayGE
{

	VulkanRenderWindow::VulkanRenderWindow(VulkanDevicePtr device, std::string const name, RenderSettings const & settings)
	{
		device_ = device;
		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		auto const& main_wnd = Context::Instance().AppInstance().MainWnd().get();
		hWnd_ = main_wnd->HWnd();
		createInfo.hwnd = hWnd_;
		createInfo.hinstance = GetModuleHandle(nullptr);

		vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(device->instance->instance, "vkCreateWin32SurfaceKHR");
		if (vkCreateWin32SurfaceKHR) 
		{
			VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(device->instance->instance, &createInfo, nullptr, &surface_));
		}
		else
		{
			TMSG("Unable to create Surface!");
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device->physicalDevice, device->queueFamilyIndices.graphics, surface_, &presentSupport);
		TIFHR(presentSupport);

		name_ = name;
		isFullScreen_ = settings.full_screen;
		if (isFullScreen_)
		{
			float const dpi_scale = main_wnd->DPIScale();

			left_ = 0;
			top_ = 0;
			width_ = static_cast<uint32_t>(settings.width * dpi_scale + 0.5f);
			height_ = static_cast<uint32_t>(settings.height * dpi_scale + 0.5f);
		}
		else
		{
			left_ = main_wnd->Left();
			top_ = main_wnd->Top();
			width_ = main_wnd->Width();
			height_ = main_wnd->Height();
		}
		
		viewport_->Left(0);
		viewport_->Top(0);
		viewport_->Width(width_);
		viewport_->Height(height_);

		this->InitSwapChainDetials(device);

		back_buffer_format_.format = VulkanMapping::MappingFormat(settings.color_fmt);
		back_buffer_format_.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		if(!(swap_chain_support_details_.formats.size() == 1 && swap_chain_support_details_.formats[0].format == VK_FORMAT_UNDEFINED))
		{
			bool avaliable = false;
			for(const auto& f : swap_chain_support_details_.formats)
			{
				if (f.format == back_buffer_format_.format && f.colorSpace == back_buffer_format_.colorSpace)
				{
					avaliable = true;
					break;
				}
			}
			if (!avaliable)
			{
				back_buffer_format_.format = swap_chain_support_details_.formats[0].format;
				back_buffer_format_.colorSpace = swap_chain_support_details_.formats[0].colorSpace;
			}
		}

		back_present_mode = VK_PRESENT_MODE_FIFO_KHR;
		//尝试选择三倍缓冲
		for(const auto& m : swap_chain_support_details_.presentModes)
		{
			if (m == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				back_present_mode = m;
				break;
			}
		}

		swapChainExtent_.width = std::clamp(width_, swap_chain_support_details_.capabilities.minImageExtent.width,
			swap_chain_support_details_.capabilities.maxImageExtent.width);
		swapChainExtent_.height = std::clamp(height_, swap_chain_support_details_.capabilities.minImageExtent.height,
			swap_chain_support_details_.capabilities.maxImageExtent.height);

		uint32_t imageCount = swap_chain_support_details_.capabilities.minImageCount + 1;
		if (swap_chain_support_details_.capabilities.maxImageCount > 0 && imageCount > swap_chain_support_details_.capabilities.maxImageCount)
		{
			imageCount = swap_chain_support_details_.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR swapCreateInfo = {};
		swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapCreateInfo.surface = surface_;
		swapCreateInfo.minImageCount = imageCount;
		swapCreateInfo.imageFormat = back_buffer_format_.format;
		swapCreateInfo.imageColorSpace = back_buffer_format_.colorSpace;
		swapCreateInfo.imageExtent = swapChainExtent_;
		swapCreateInfo.imageArrayLayers = 1;
		swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		swapCreateInfo.preTransform = swap_chain_support_details_.capabilities.currentTransform;
		swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapCreateInfo.presentMode = back_present_mode;
		swapCreateInfo.clipped = VK_TRUE;
		swapCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		VK_CHECK_RESULT(vkCreateSwapchainKHR(device->logicalDevice, &swapCreateInfo, nullptr, &swapChain_));

		vkGetSwapchainImagesKHR(device->logicalDevice, swapChain_, &imageCount, nullptr);
		swapChainImages_.resize(imageCount);
		vkGetSwapchainImagesKHR(device->logicalDevice, swapChain_, &imageCount, swapChainImages_.data());



		RenderFactory& rf = Context::Instance().RenderFactoryInstance();
		TexturePtr ptr = rf.MakeTexture2D(width_, height_, 1, 1, settings.depth_stencil_fmt, 1, 1, EAH_GPU_Write | EAH_GPU_Read);
		depth_stencil_ = rf.Make2DDsv(ptr, 0, 1, 0);
		swapChainImageViews_.resize(imageCount);
		fences_.resize(imageCount);
		frameBuffers_.resize(imageCount);
		CreateRenderPass();
		for (size_t i = 0; i < swapChainImages_.size(); i++)
		{
			VkImageViewCreateInfo ivCI = {};
			ivCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			ivCI.image = swapChainImages_[i];
			ivCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
			ivCI.format = back_buffer_format_.format;
			ivCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			ivCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			ivCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			ivCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			ivCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			ivCI.subresourceRange.baseMipLevel = 0;
			ivCI.subresourceRange.levelCount = 1;
			ivCI.subresourceRange.baseArrayLayer = 0;
			ivCI.subresourceRange.layerCount = 1;
			VK_CHECK_RESULT(vkCreateImageView(device_->logicalDevice, &ivCI, nullptr, &swapChainImageViews_[i]));

			std::array<VkImageView, 2> attachments = {
                swapChainImageViews_[i],
                checked_cast<VulkanDepthStencilView &>(*depth_stencil_).GetVkImageView()
            };
			VkFramebufferCreateInfo fbCI = {};
			fbCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			fbCI.renderPass = renderPass_;
			fbCI.attachmentCount = 1;
			fbCI.pAttachments = attachments.data();
			fbCI.attachmentCount = 2;
			fbCI.width = width_;
			fbCI.height = height_;
			fbCI.layers = 1;
			VK_CHECK_RESULT(vkCreateFramebuffer(device_->logicalDevice, &fbCI, nullptr, &frameBuffers_[i]));

			VkFenceCreateInfo fCI = {};
			fCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			VK_CHECK_RESULT(vkCreateFence(device_->logicalDevice, &fCI, nullptr, &fences_[i]));
		}
		VkSemaphoreCreateInfo sCI = {};
		sCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VK_CHECK_RESULT(vkCreateSemaphore(device_->logicalDevice, &sCI, nullptr, &image_avaliable_semaphore_));
		VK_CHECK_RESULT(vkCreateSemaphore(device_->logicalDevice, &sCI, nullptr, &signal_semaphore));
    }

	std::wstring const & VulkanRenderWindow:: Description() const
	{
		static std::wstring const desc(L"Vulkan Render Window");
		return desc;
	}
	void VulkanRenderWindow::OnBind()
	{

	}
	void VulkanRenderWindow::OnUnbind()
	{

	}
	void VulkanRenderWindow::Discard(uint32_t flags)
	{
		KFL_UNUSED(flags);
	}

	void VulkanRenderWindow::BeginFrame()
	{
		//获取当前帧 图像索引
		VK_CHECK_RESULT(vkAcquireNextImageKHR(device_->logicalDevice, swapChain_, UINT64_MAX, image_avaliable_semaphore_, VK_NULL_HANDLE, &cur_image_index));
		vkWaitForFences(device_->logicalDevice, 1, &fences_[cur_image_index], VK_TRUE, UINT64_MAX);
	}

	void VulkanRenderWindow::SwapBuffers()
	{
		auto const& re = dynamic_cast<VulkanRenderEngine&>(Context::Instance().RenderFactoryInstance().RenderEngineInstance());
		auto const& commandBuffer = re.CurCommandBuffer();
		VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

		vkResetFences(device_->logicalDevice, 1, &fences_[cur_image_index]);
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &image_avaliable_semaphore_;
        submitInfo.pWaitDstStageMask = waitStages;
        
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &signal_semaphore;
		vkQueueSubmit(device_->deviceQueues.graphics, 1, &submitInfo, fences_[cur_image_index]);

		// 提交到queue
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &signal_semaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapChain_;
		presentInfo.pImageIndices = &cur_image_index;

		VK_CHECK_RESULT(vkQueuePresentKHR(device_->deviceQueues.graphics, &presentInfo));
	}

	void VulkanRenderWindow::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = back_buffer_format_.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        
        VkAttachmentDescription depthAttachment = {};
        depthAttachment.format = checked_cast<VulkanTexture&>(*depth_stencil_->TextureResource()).vkFormat_;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        
        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        VkAttachmentReference depthAttachmentRef = {};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        
        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        
        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

		vkCreateRenderPass(device_->logicalDevice, &renderPassInfo, nullptr, &renderPass_);
	}

	void VulkanRenderWindow::WaitOnSwapBuffers()
	{
		
	}

	void VulkanRenderWindow::InitSwapChainDetials(VulkanDevicePtr device)
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_->physicalDevice, surface_, &swap_chain_support_details_.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device_->physicalDevice, surface_, &formatCount, nullptr);
		if (formatCount != 0) {
			swap_chain_support_details_.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device_->physicalDevice, surface_, &formatCount, swap_chain_support_details_.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device_->physicalDevice, surface_, &presentModeCount, nullptr);
		if (presentModeCount != 0) {
			swap_chain_support_details_.presentModes.resize(formatCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device_->physicalDevice, surface_, &formatCount, swap_chain_support_details_.presentModes.data());
		}
	}

	void VulkanRenderWindow::Clear(uint32_t flags, Color const & clr, float depth, int32_t stencil)
	{
		clearValues_.flags = flags;
		clearValues_.color = clr;
		clearValues_.depth = depth;
		clearValues_.stencil = stencil;
	}

	VkRenderPassBeginInfo VulkanRenderWindow::RenderPassInfo()
	{
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass_;
		renderPassInfo.framebuffer = frameBuffers_[cur_image_index];
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = {width_, height_};
		VkClearValue clearValues[2];
		clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
		clearValues[1].depthStencil = { 1.0f, 0 };
		if (clearValues_.flags & CBM_Color)
		{
			clearValues[0].color = {clearValues_.color.r(), clearValues_.color.g(), clearValues_.color.b(), clearValues_.color.a()};
		}
		if (clearValues_.flags & CBM_Depth)
		{
			clearValues[1].depthStencil.depth = clearValues_.depth;
		}
		if (clearValues_.flags & CBM_Stencil)
		{
			clearValues[1].depthStencil.stencil = clearValues_.stencil;
		}
		clearValues_.flags = 0;
		renderPassInfo.pClearValues = clearValues;
		renderPassInfo.clearValueCount = 2;
		return renderPassInfo;
	}

	VulkanRenderWindow::~VulkanRenderWindow()
	{
		if (swapChain_ != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(device_->logicalDevice, swapChain_, nullptr);
			for (auto image : swapChainImages_)
			{
				vkDestroyImage(device_->logicalDevice, image, nullptr);
			}
			for (auto imageView : swapChainImageViews_)
			{
				vkDestroyImageView(device_->logicalDevice, imageView, nullptr);
			}
		}	
	}
}
