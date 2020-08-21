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

#ifndef KLAYGE_PLUGINS_VULKAN_TEXTURE_HPP
#define KLAYGE_PLUGINS_VULKAN_TEXTURE_HPP

#pragma once

#include <KlayGE/Texture.hpp>
#include <KlayGE/Vulkan/VulkanDevice.hpp>
#include <KlayGE/Vulkan/VulkanGraphicsBuffer.hpp>


namespace KlayGE
{
	class VulkanTexture : public Texture
	{
		friend class VulkanShaderResourceView;
		friend class VulkanDepthStencilView;
		friend class VulkanRenderTargetView;
		friend class VulkanRenderWindow;

	protected:
		bool hw_ready_ = false;
		VulkanDevicePtr device_;
		VkFormat vkFormat_;
		VkImage image_ = VK_NULL_HANDLE;
		VkDeviceMemory memory_ = VK_NULL_HANDLE;
		std::vector<uint32_t> mipmap_start_offset_;

	public:
		VulkanTexture(TextureType type, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint);
		~VulkanTexture() override;

		std::wstring const& Name() const override;

		uint32_t Width(uint32_t level) const override;
		uint32_t Height(uint32_t level) const override;
		uint32_t Depth(uint32_t level) const override;
		VkFormat GetVkFormat() const
		{
			return vkFormat_;
		};

		void CopyToTexture(Texture& target, TextureFilter filter) override;
		void CopyToSubTexture1D(Texture& target, uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_width,
			uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_width, TextureFilter filter) override;
		void CopyToSubTexture2D(Texture& target, uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset,
			uint32_t dst_width, uint32_t dst_height, uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset,
			uint32_t src_y_offset, uint32_t src_width, uint32_t src_height, TextureFilter filter) override;
		void CopyToSubTexture3D(Texture& target, uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset, uint32_t dst_y_offset,
			uint32_t dst_z_offset, uint32_t dst_width, uint32_t dst_height, uint32_t dst_depth, uint32_t src_array_index,
			uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_z_offset, uint32_t src_width,
			uint32_t src_height, uint32_t src_depth, TextureFilter filter) override;
		void CopyToSubTextureCube(Texture& target, uint32_t dst_array_index, CubeFaces dst_face, uint32_t dst_level, uint32_t dst_x_offset,
			uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height, uint32_t src_array_index, CubeFaces src_face,
			uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height,
			TextureFilter filter) override;

		void Map1D(uint32_t array_index, uint32_t level, TextureMapAccess tma, uint32_t x_offset, uint32_t width, void*& data) override;
		void Map2D(uint32_t array_index, uint32_t level, TextureMapAccess tma, uint32_t x_offset, uint32_t y_offset, uint32_t width,
			uint32_t height, void*& data, uint32_t& row_pitch) override;
		void Map3D(uint32_t array_index, uint32_t level, TextureMapAccess tma, uint32_t x_offset, uint32_t y_offset, uint32_t z_offset,
			uint32_t width, uint32_t height, uint32_t depth, void*& data, uint32_t& row_pitch, uint32_t& slice_pitch) override;
		void MapCube(uint32_t array_index, CubeFaces face, uint32_t level, TextureMapAccess tma, uint32_t x_offset, uint32_t y_offset,
			uint32_t width, uint32_t height, void*& data, uint32_t& row_pitch) override;

		void Unmap1D(uint32_t array_index, uint32_t level) override;
		void Unmap2D(uint32_t array_index, uint32_t level) override;
		void Unmap3D(uint32_t array_index, uint32_t level) override;
		void UnmapCube(uint32_t array_index, CubeFaces face, uint32_t level) override;

		void DoCreateHwResource(VkImageCreateInfo& createInfo, std::span<ElementInitData const> init_data, float4 const* clear_value_hint);
		void DeleteHWResource() override;
		bool HWResourceReady() const override;

		void UpdateSubresource1D(uint32_t array_index, uint32_t level, uint32_t x_offset, uint32_t width, void const* data) override;
		void UpdateSubresource2D(uint32_t array_index, uint32_t level, uint32_t x_offset, uint32_t y_offset, uint32_t width,
			uint32_t height, void const* data, uint32_t row_pitch) override;
		void UpdateSubresource3D(uint32_t array_index, uint32_t level, uint32_t x_offset, uint32_t y_offset, uint32_t z_offset,
			uint32_t width, uint32_t height, uint32_t depth, void const* data, uint32_t row_pitch, uint32_t slice_pitch) override;
		void UpdateSubresourceCube(uint32_t array_index, CubeFaces face, uint32_t level, uint32_t x_offset, uint32_t y_offset,
			uint32_t width, uint32_t height, void const* data, uint32_t row_pitch) override;

		void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, VkImageSubresourceRange range,
			VkPipelineStageFlags oldStage, VkPipelineStageFlags newStage);
	};

	class VulkanTexture2D : public VulkanTexture
	{
	public:
		VulkanTexture2D(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t numMipMaps, uint32_t array_size,
			ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint);

		void CreateHWResource(std::span<ElementInitData const> init_data, float4 const* clear_value_hint) override;
		uint32_t Width(uint32_t level) const override;
		uint32_t Height(uint32_t level) const override;

	private:
		uint32_t width_;
		uint32_t height_;
	};


	class VulkanTexture3D : public VulkanTexture
	{
	public:
		VulkanTexture3D(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t depth, uint32_t numMipMaps, uint32_t array_size,
			ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint);

		void CreateHWResource(std::span<ElementInitData const> init_data, float4 const* clear_value_hint) override;
		uint32_t Width(uint32_t level) const override;
		uint32_t Height(uint32_t level) const override;
		uint32_t Depth(uint32_t level) const override;

	private:
		uint32_t width_;
		uint32_t height_;
		uint32_t depth_;
	};
} // namespace KlayGE

#endif // KLAYGE_PLUGINS_VULKAN_TEXTURE_HPP
