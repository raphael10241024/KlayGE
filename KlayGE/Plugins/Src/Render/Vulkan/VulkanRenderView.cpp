/**
 * @file VulkanRenderView.cpp
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

#pragma once

#include <KFL/ErrorHandling.hpp>
#include <KlayGE/Vulkan/VulkanRenderView.hpp>
namespace KlayGE
{
	VulkanShaderResourceView::VulkanShaderResourceView(TexturePtr const& texture, ElementFormat pf, uint32_t first_array_index,
		uint32_t array_size, uint32_t first_level, uint32_t num_levels)
	{
		BOOST_ASSERT(texture);

		tex_ = texture;
		pf_ = pf == EF_Unknown ? texture->Format() : pf;
		first_array_index_ = first_array_index;
		array_size_ = array_size;
		first_level_ = first_level;
		num_levels_ = num_levels;
		first_elem_ = 0;
		num_elems_ = 0;

		VulkanTexture& t = checked_cast<VulkanTexture&>(*texture);
		if (!t.hw_ready_)
		{
			TMSG("ERROR");
		}

		VkImageViewCreateInfo viewCI = {};
		viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

		viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
		if (t.type_ == Texture::TextureType::TT_3D)
			viewCI.viewType = VK_IMAGE_VIEW_TYPE_3D;
		else if (t.type_ == Texture::TextureType::TT_1D)
			viewCI.viewType = VK_IMAGE_VIEW_TYPE_1D;

		viewCI.format = t.vkFormat_;
		viewCI.subresourceRange = {};
		viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		if(IsDepthFormat(t.format_)) viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		viewCI.subresourceRange.baseMipLevel = first_level_;
		viewCI.subresourceRange.levelCount = num_levels_;
		viewCI.subresourceRange.baseArrayLayer = first_array_index_;
		viewCI.subresourceRange.layerCount = array_size_;
		viewCI.image = t.image_;

		VK_CHECK_RESULT(vkCreateImageView(t.device_->logicalDevice, &viewCI, nullptr, &imageView_));

		VkSamplerCreateInfo samplerCI = {};
		samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCI.magFilter = VK_FILTER_LINEAR;
		samplerCI.minFilter = VK_FILTER_LINEAR;
		samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCI.minLod = 0.0f;
		samplerCI.maxLod = 0.0f;
		samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCI.mipLodBias = 0.0f;

		samplerCI.anisotropyEnable = VK_FALSE;
		samplerCI.maxAnisotropy = 1;
		samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		samplerCI.unnormalizedCoordinates = VK_FALSE;
		samplerCI.compareEnable = VK_FALSE;
		samplerCI.compareOp = VK_COMPARE_OP_ALWAYS;

		VK_CHECK_RESULT(vkCreateSampler(t.device_->logicalDevice, &samplerCI, nullptr, &sampler_));
	}


	VulkanDepthStencilView::VulkanDepthStencilView(
		TexturePtr const& texture, ElementFormat pf, int first_array_index, int array_size, int level)
	{
		BOOST_ASSERT(texture);

		tex_ = texture;
		width_ = texture->Width(level);
		height_ = texture->Height(level);
		pf_ = pf == EF_Unknown ? texture->Format() : pf;
		sample_count_ = texture->SampleCount();
		sample_quality_ = texture->SampleQuality();

		first_array_index_ = first_array_index;
		array_size_ = array_size;
		level_ = level;
		first_slice_ = 0;
		num_slices_ = texture->Depth(0);
		first_face_ = Texture::CF_Positive_X;
		num_faces_ = 1;

		VulkanTexture& t = checked_cast<VulkanTexture&>(*texture);
		if (!t.hw_ready_)
		{
			TMSG("ERROR");
		}

		VkImageViewCreateInfo viewCI = {};
		viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

		viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
		if (t.type_ == Texture::TextureType::TT_3D)
			viewCI.viewType = VK_IMAGE_VIEW_TYPE_3D;
		else if (t.type_ == Texture::TextureType::TT_1D)
			viewCI.viewType = VK_IMAGE_VIEW_TYPE_1D;

		viewCI.format = t.vkFormat_;
		viewCI.subresourceRange = {};
		viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		viewCI.subresourceRange.baseMipLevel = level;
		viewCI.subresourceRange.levelCount = 1;
		viewCI.subresourceRange.baseArrayLayer = first_array_index_;
		viewCI.subresourceRange.layerCount = array_size_;
		viewCI.image = t.image_;
		VK_CHECK_RESULT(vkCreateImageView(t.device_->logicalDevice, &viewCI, nullptr, &imageView_));
	}

	void VulkanDepthStencilView::ClearDepth(float depth)
	{
		nextClearDepth_ = depth;
	}

	void VulkanDepthStencilView::ClearStencil(int32_t stencil)
	{
		nextClearStencil_ = stencil;
	}
	void VulkanDepthStencilView::ClearDepthStencil(float depth, int32_t stencil)
	{
		ClearDepth(depth);
		ClearStencil(stencil);
	}

	void VulkanDepthStencilView::Discard()
	{
	}

	void VulkanDepthStencilView::OnAttached(FrameBuffer& fb)
	{
		KFL_UNUSED(fb);
	}
	void VulkanDepthStencilView::OnDetached(FrameBuffer& fb)
	{
		KFL_UNUSED(fb);
	}


	VulkanRenderTargetView::VulkanRenderTargetView(
		TexturePtr const& texture, ElementFormat pf, int first_array_index, int array_size, int level)
	{
		BOOST_ASSERT(texture);

		tex_ = texture;
		width_ = texture->Width(level);
		height_ = texture->Height(level);
		pf_ = pf == EF_Unknown ? texture->Format() : pf;
		sample_count_ = texture->SampleCount();
		sample_quality_ = texture->SampleQuality();

		first_array_index_ = first_array_index;
		array_size_ = array_size;
		level_ = level;
		first_slice_ = 0;
		num_slices_ = texture->Depth(0);
		first_face_ = Texture::CF_Positive_X;
		num_faces_ = 1;
		first_elem_ = 0;
		num_elems_ = 0;

		VulkanTexture& t = checked_cast<VulkanTexture&>(*texture);
		if (!t.hw_ready_)
		{
			TMSG("ERROR");
		}

		VkImageViewCreateInfo viewCI = {};
		viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

		viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
		if (t.type_ == Texture::TextureType::TT_3D)
			viewCI.viewType = VK_IMAGE_VIEW_TYPE_3D;
		else if (t.type_ == Texture::TextureType::TT_1D)
			viewCI.viewType = VK_IMAGE_VIEW_TYPE_1D;

		viewCI.format = t.vkFormat_;
		viewCI.subresourceRange = {};
		viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewCI.subresourceRange.baseMipLevel = level_;
		viewCI.subresourceRange.levelCount = 1;
		viewCI.subresourceRange.baseArrayLayer = first_array_index_;
		viewCI.subresourceRange.layerCount = array_size_;
		viewCI.image = t.image_;

		VK_CHECK_RESULT(vkCreateImageView(t.device_->logicalDevice, &viewCI, nullptr, &imageView_));
	}

	void VulkanRenderTargetView::ClearColor(Color const& clr)
	{
		nextClearColor_ = clr;
	}

	void VulkanRenderTargetView::Discard()
	{
	}

	void VulkanRenderTargetView::OnAttached(FrameBuffer& fb, FrameBuffer::Attachment att)
	{
		KFL_UNUSED(fb);
		KFL_UNUSED(att);
	}

	void VulkanRenderTargetView::OnDetached(FrameBuffer& fb, FrameBuffer::Attachment att)
	{
		KFL_UNUSED(fb);
		KFL_UNUSED(att);
	}
} // namespace KlayGE
