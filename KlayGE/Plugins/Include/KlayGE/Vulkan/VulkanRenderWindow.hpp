/**
 * @file VulkanRenderWindow.hpp
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

#ifndef KLAYGE_PLUGINS_VULKAN_RENDER_WINDOW_HPP
#define KLAYGE_PLUGINS_VULKAN_RENDER_WINDOW_HPP

#pragma once

#include <KlayGE/FrameBuffer.hpp>
#include <KlayGE/Vulkan/VulkanDevice.hpp>
#include <KlayGE/Vulkan/VulkanRenderView.hpp>
#include <KlayGE/Vulkan/VulkanFrameBuffer.hpp>
#include <KlayGE/FrameBuffer.hpp>
#include <KlayGE/RenderView.hpp>

namespace KlayGE
{
	class VulkanRenderWindow final : public VulkanFrameBuffer
	{
		friend class VulkanRenderEngine;
	public:
		std::wstring const & Description() const override;
		void OnBind() override;
		void OnUnbind() override;
		void Discard(uint32_t flags) override;

		void BeginFrame();

		void SwapBuffers() override;
		void WaitOnSwapBuffers() override;
		VulkanRenderWindow(VulkanDevicePtr device, std::string const name, RenderSettings const & settings);
		~VulkanRenderWindow() override;
		void InitSwapChainDetials(VulkanDevicePtr device);

		void CreateRenderPass();

		inline const TexturePtr& GetVulkanDepthStencilTexture()
		{
			return depth_stencil_->TextureResource();
		};

		inline size_t CacheSize()
		{
			return frameBuffers_.size();
		};

		inline uint32_t CurImageIndex()
		{
			return cur_image_index;
		};

		inline VkFramebuffer CurFrameBuffer() override
		{
			return frameBuffers_[cur_image_index];
		}
		void Clear(uint32_t flags, Color const & clr, float depth, int32_t stencil) override;
		VkRenderPassBeginInfo RenderPassInfo() override;
	private:
		std::string	name_;
		HWND	hWnd_;
		bool	isFullScreen_;
		uint32_t color_bits_;

		//Signal::Connection on_exit_size_move_connect_;
		//Signal::Connection on_size_connect_;
		
		VkSurfaceKHR surface_;
		PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;

		VkSwapchainKHR swapChain_ = VK_NULL_HANDLE;
		std::vector<VkImage> swapChainImages_;
		std::vector<VkImageView> swapChainImageViews_;
		VkExtent2D swapChainExtent_;
		VkSurfaceFormatKHR back_buffer_format_;
		VkPresentModeKHR back_present_mode;
		DepthStencilViewPtr depth_stencil_;

		uint32_t cur_image_index;
		std::vector<VkFence> fences_;

		VkRenderPass renderPass_;
		std::vector<VkFramebuffer> frameBuffers_;

		VkSemaphore image_avaliable_semaphore_;
		VkSemaphore signal_semaphore;

		struct  {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		} swap_chain_support_details_;

		struct
		{
			uint32_t flags;
			Color color;
			float depth;
			int32_t stencil;
		} clearValues_;

	};
} // namespace KlayGE

#endif // KLAYGE_PLUGINS_VULKAN_RENDER_WINDOW_HPP
