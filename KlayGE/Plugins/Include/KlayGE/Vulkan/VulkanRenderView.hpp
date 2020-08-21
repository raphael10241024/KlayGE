/**
 * @file VulkanTexture.hpp
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

#ifndef KLAYGE_PLUGINS_VULKAN_RENDER_VIEW_HPP
#define KLAYGE_PLUGINS_VULKAN_RENDER_VIEW_HPP

#pragma once

#include <optional>
#include <KlayGE/KlayGE.hpp>
#include <KlayGE/RenderView.hpp>
#include <KlayGE/Texture.hpp>
#include <KlayGE/Vulkan/VulkanDevice.hpp>
#include <KlayGE/Vulkan/VulkanGraphicsBuffer.hpp>
#include <KlayGE/Vulkan/VulkanTexture.hpp>

namespace KlayGE
{
	class VulkanShaderResourceView : public ShaderResourceView
	{
	public:
		VulkanShaderResourceView(TexturePtr const& texture, ElementFormat pf, uint32_t first_array_index, uint32_t array_size,
			uint32_t first_level, uint32_t num_levels);

	private:
		VkImageView imageView_;
		VkSampler sampler_;
	};

	class VulkanDepthStencilView : public DepthStencilView
	{
		friend class VulkanCommonFrameBuffer;
	public:
		VulkanDepthStencilView(TexturePtr const& texture, ElementFormat pf, int first_array_index, int array_size, int level);

		void ClearDepth(float depth);
		void ClearStencil(int32_t stencil);
		void ClearDepthStencil(float depth, int32_t stencil);

		void Discard();

		void OnAttached(FrameBuffer& fb);
		void OnDetached(FrameBuffer& fb);

		inline VkImageView GetVkImageView()
		{
			return imageView_;
		}
	private:
		VkImageView imageView_;
		std::optional<float> nextClearDepth_;
		std::optional<uint32_t> nextClearStencil_;
	};

	class VulkanRenderTargetView : public RenderTargetView
	{
		friend class VulkanCommonFrameBuffer;
	public:
		VulkanRenderTargetView(TexturePtr const& texture, ElementFormat pf, int first_array_index, int array_size, int level);
		void ClearColor(Color const& clr);

		void Discard();

		void OnAttached(FrameBuffer& fb, FrameBuffer::Attachment att);
		void OnDetached(FrameBuffer& fb, FrameBuffer::Attachment att);

		inline VkImageView GetVkImageView() const { return imageView_; }

	private:
		VkImageView imageView_;
		std::optional<Color> nextClearColor_;
	};
} // namespace KlayGE

#endif // KLAYGE_PLUGINS_VULKAN_RENDER_VIEW_HPP
