/**
 * @file VulkanFrameBuffer.cpp
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

#include <KFL/ErrorHandling.hpp>
#include <KFL/Log.hpp>
#include <KlayGE/KlayGE.hpp>
#include <KlayGE/Vulkan/VulkanFrameBuffer.hpp>
#include <KlayGE/Vulkan/VulkanRenderView.hpp>
#include <fstream>
#include <system_error>

namespace KlayGE
{

	VulkanCommonFrameBuffer::VulkanCommonFrameBuffer(VulkanDevicePtr device)
	{
		device_ = device;
	}

	std::wstring const& VulkanCommonFrameBuffer::Description() const
	{
        static std::wstring const desc(L"Vulkan Framebuffer");
		return desc;
	}

	void VulkanCommonFrameBuffer::InitFrameBuffer()
	{
		if (views_dirty_)
		{
			views_dirty_ = false;
			if (frameBuffer_ != VK_NULL_HANDLE)
			{
				vkDestroyFramebuffer(device_->logicalDevice, frameBuffer_, nullptr);
				// TODO:缓存共享renderpass信息
				vkDestroyRenderPass(device_->logicalDevice, renderPass_, nullptr);
			}
			std::vector<VkImageView> views;
			std::vector<VkAttachmentDescription> descriptions;
			std::vector<VkAttachmentReference> colorReferences;
			uint32_t index = 0;
			for(int i = 0; i < rt_views_.size(); i++)
			{
				if(rt_views_[i])
				{
					auto& v = checked_cast<VulkanRenderTargetView&>(*rt_views_[i]);
					auto& t = checked_cast<VulkanTexture&>(*rt_views_[i]->TextureResource());
					views.push_back(v.GetVkImageView());

					VkAttachmentDescription desc = {};
					desc.format = t.GetVkFormat();
					desc.samples = VK_SAMPLE_COUNT_1_BIT;
					// TODO: 是否有的framebuffer不需要clear?
					desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
					desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
					desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
					desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					desc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					descriptions.push_back(desc);

					colorReferences.push_back({ index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
					index++;
				}
			}
			VkSubpassDescription subpassDescription = {};
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
			subpassDescription.pColorAttachments = colorReferences.data();
			
			if (ds_view_)
			{
				auto& v = checked_cast<VulkanDepthStencilView&>(*ds_view_);
				auto& t = checked_cast<VulkanTexture&>(*ds_view_->TextureResource());
				views.push_back(v.GetVkImageView());

				VkAttachmentDescription desc = {};
				desc.format = t.GetVkFormat();
				desc.samples = VK_SAMPLE_COUNT_1_BIT;
				desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				desc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				descriptions.push_back(desc);
				
				VkAttachmentReference depthReference = { index, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };		
				subpassDescription.pDepthStencilAttachment = &depthReference;
			}

			

			std::array<VkSubpassDependency, 2> dependencies;
			dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[0].dstSubpass = 0;
			dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			dependencies[1].srcSubpass = 0;
			dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			VkRenderPassCreateInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = static_cast<uint32_t>(descriptions.size());
			renderPassInfo.pAttachments = descriptions.data();
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpassDescription;
			renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
			renderPassInfo.pDependencies = dependencies.data();

			VK_CHECK_RESULT(vkCreateRenderPass(device_->logicalDevice, &renderPassInfo, nullptr, &renderPass_));

			VkFramebufferCreateInfo fbCI = {};
			fbCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			fbCI.renderPass = renderPass_;
			fbCI.pAttachments = views.data();
			fbCI.attachmentCount = static_cast<uint32_t>(views.size());
			fbCI.width = width_;
			fbCI.height = height_;
			fbCI.layers = 1;
			VK_CHECK_RESULT(vkCreateFramebuffer(device_->logicalDevice, &fbCI, nullptr, &frameBuffer_));
		}
	}

	void VulkanCommonFrameBuffer::OnBind()
	{
		InitFrameBuffer();
	}
	void VulkanCommonFrameBuffer::OnUnbind()
	{
	}

	void VulkanCommonFrameBuffer::Discard(uint32_t flags)
	{
        KFL_UNUSED(flags);
	}

	void VulkanCommonFrameBuffer::Clear(uint32_t flags, Color const & clr, float depth, int32_t stencil)
	{
		if (flags & CBM_Color)
		{
			for (uint32_t i = 0; i < rt_views_.size(); ++ i)
			{
				if (rt_views_[i])
				{
					rt_views_[i]->ClearColor(clr);
				}
			}
		}
		if (ds_view_)
		{
			if (flags & CBM_Depth)
			{
				ds_view_->ClearDepth(depth);
			}
			if (flags & CBM_Stencil)
			{
					ds_view_->ClearStencil(stencil);
			}
		}
	}

	VkFramebuffer VulkanCommonFrameBuffer::CurFrameBuffer()
	{
		return frameBuffer_;
	}

	VkRenderPassBeginInfo VulkanCommonFrameBuffer::RenderPassInfo()
	{
		if (views_dirty_)
		{
			TMSG("Frame buffer not bind!");
		}
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass_;
		renderPassInfo.framebuffer = frameBuffer_;
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = {width_, height_};
		
		std::vector<VkClearValue> clearValues;
		for(int i = 0; i < rt_views_.size(); i++)
		{
			auto& v = checked_cast<VulkanRenderTargetView&>(*rt_views_[i]);
			VkClearValue c;
			if (v.nextClearColor_)
			{
				c.color = {v.nextClearColor_->r(), v.nextClearColor_->g(), v.nextClearColor_->b(), v.nextClearColor_->a()};
				v.nextClearColor_.reset();
			}
			else
			{
				c.color = {0.0f, 0.0f, 0.0f, 1.0f};
			}
			clearValues.push_back(c);
		}
		if (ds_view_)
		{
			auto& v = checked_cast<VulkanDepthStencilView&>(*ds_view_);
			VkClearValue c;
			c.depthStencil = {1.0f, 0};
			if (v.nextClearDepth_)
			{
				c.depthStencil.depth = v.nextClearDepth_.value();
				v.nextClearDepth_.reset();
			}
			if (v.nextClearStencil_)
			{
				c.depthStencil.stencil = v.nextClearStencil_.value();
				v.nextClearStencil_.reset();
			}
		}
		
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();
		
		return renderPassInfo;
	}
} // namespace KlayGE
