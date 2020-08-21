/**
 * @file VulkanFrameBuffer.hpp
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

#ifndef KLAYGE_PLUGINS_VULKAN_FRAME_BUFFER_HPP
#define KLAYGE_PLUGINS_VULKAN_FRAME_BUFFER_HPP

#pragma once

#include <KlayGE/FrameBuffer.hpp>
#include <KlayGE/Vulkan/VulkanTypedefs.hpp>
#include <KlayGE/Vulkan/VulkanDevice.hpp>

namespace KlayGE
{
	class VulkanFrameBuffer : public FrameBuffer
	{
	public:
		virtual VkFramebuffer CurFrameBuffer() = 0 ;
		virtual VkRenderPassBeginInfo RenderPassInfo() = 0;
	protected:
		VulkanDevicePtr device_;
	};

	class VulkanCommonFrameBuffer : public VulkanFrameBuffer
	{
	public:
		void Clear(uint32_t flags, Color const & clr, float depth, int32_t stencil) override;
		VulkanCommonFrameBuffer(VulkanDevicePtr device);
		std::wstring const & Description() const override;
		void OnBind() override;
		void OnUnbind() override;
		void Discard(uint32_t flags) override;
		VkFramebuffer CurFrameBuffer() override;
		VkRenderPassBeginInfo RenderPassInfo() override;
	private:
		void InitFrameBuffer();
		VkFramebuffer frameBuffer_ = VK_NULL_HANDLE;
		VkRenderPass renderPass_;
	};
} // namespace KlayGE

#endif // KLAYGE_PLUGINS_VULKAN_FRAME_BUFFER_HPP
