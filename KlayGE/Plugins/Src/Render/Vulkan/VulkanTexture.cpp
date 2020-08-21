/**
 * @file VulkanRenderEngine.cpp
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
#include <KlayGE/KlayGE.hpp>
#include <KlayGE/Vulkan/VulkanTexture.hpp>

namespace KlayGE
{
	VulkanTexture::VulkanTexture(TextureType type, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint)
		: Texture(type, sample_count, sample_quality, access_hint)
	{
	}

	VulkanTexture::~VulkanTexture()
	{
	}

	std::wstring const& VulkanTexture::Name() const
	{
		static const std::wstring name(L"Vulkan Texture");
		return name;
	}

	uint32_t VulkanTexture::Width(uint32_t level) const
	{
		KFL_UNUSED(level);
		BOOST_ASSERT(level < num_mip_maps_);
		return 1;
	}

	uint32_t VulkanTexture::Height(uint32_t level) const
	{
		KFL_UNUSED(level);
		BOOST_ASSERT(level < num_mip_maps_);
		return 1;
	}

	uint32_t VulkanTexture::Depth(uint32_t level) const
	{
		KFL_UNUSED(level);
		BOOST_ASSERT(level < num_mip_maps_);
		return 1;
	}

	void VulkanTexture::CopyToTexture(Texture& target, TextureFilter filter)
	{
		KFL_UNUSED(target);
		KFL_UNUSED(filter);
	}

	void VulkanTexture::CopyToSubTexture1D(Texture& target, uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset,
		uint32_t dst_width, uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset, uint32_t src_width, TextureFilter filter)
	{
		KFL_UNUSED(target);
		KFL_UNUSED(dst_array_index);
		KFL_UNUSED(dst_level);
		KFL_UNUSED(dst_x_offset);
		KFL_UNUSED(dst_width);
		KFL_UNUSED(src_array_index);
		KFL_UNUSED(src_level);
		KFL_UNUSED(src_x_offset);
		KFL_UNUSED(src_width);
		KFL_UNUSED(filter);
	}

	void VulkanTexture::CopyToSubTexture2D(Texture& target, uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset,
		uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height, uint32_t src_array_index, uint32_t src_level, uint32_t src_x_offset,
		uint32_t src_y_offset, uint32_t src_width, uint32_t src_height, TextureFilter filter)
	{
		KFL_UNUSED(target);
		KFL_UNUSED(dst_array_index);
		KFL_UNUSED(dst_level);
		KFL_UNUSED(dst_x_offset);
		KFL_UNUSED(dst_y_offset);
		KFL_UNUSED(dst_width);
		KFL_UNUSED(dst_height);
		KFL_UNUSED(src_array_index);
		KFL_UNUSED(src_level);
		KFL_UNUSED(src_x_offset);
		KFL_UNUSED(src_y_offset);
		KFL_UNUSED(src_width);
		KFL_UNUSED(src_height);
		KFL_UNUSED(filter);
	}

	void VulkanTexture::CopyToSubTexture3D(Texture& target, uint32_t dst_array_index, uint32_t dst_level, uint32_t dst_x_offset,
		uint32_t dst_y_offset, uint32_t dst_z_offset, uint32_t dst_width, uint32_t dst_height, uint32_t dst_depth, uint32_t src_array_index,
		uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_z_offset, uint32_t src_width, uint32_t src_height,
		uint32_t src_depth, TextureFilter filter)
	{
		KFL_UNUSED(target);
		KFL_UNUSED(dst_array_index);
		KFL_UNUSED(dst_level);
		KFL_UNUSED(dst_x_offset);
		KFL_UNUSED(dst_y_offset);
		KFL_UNUSED(dst_z_offset);
		KFL_UNUSED(dst_width);
		KFL_UNUSED(dst_height);
		KFL_UNUSED(dst_depth);
		KFL_UNUSED(src_array_index);
		KFL_UNUSED(src_level);
		KFL_UNUSED(src_x_offset);
		KFL_UNUSED(src_y_offset);
		KFL_UNUSED(src_z_offset);
		KFL_UNUSED(src_width);
		KFL_UNUSED(src_height);
		KFL_UNUSED(src_depth);
		KFL_UNUSED(filter);
	}

	void VulkanTexture::CopyToSubTextureCube(Texture& target, uint32_t dst_array_index, CubeFaces dst_face, uint32_t dst_level,
		uint32_t dst_x_offset, uint32_t dst_y_offset, uint32_t dst_width, uint32_t dst_height, uint32_t src_array_index, CubeFaces src_face,
		uint32_t src_level, uint32_t src_x_offset, uint32_t src_y_offset, uint32_t src_width, uint32_t src_height, TextureFilter filter)
	{
		KFL_UNUSED(target);
		KFL_UNUSED(dst_array_index);
		KFL_UNUSED(dst_face);
		KFL_UNUSED(dst_level);
		KFL_UNUSED(dst_x_offset);
		KFL_UNUSED(dst_y_offset);
		KFL_UNUSED(dst_width);
		KFL_UNUSED(dst_height);
		KFL_UNUSED(src_array_index);
		KFL_UNUSED(src_face);
		KFL_UNUSED(src_level);
		KFL_UNUSED(src_x_offset);
		KFL_UNUSED(src_y_offset);
		KFL_UNUSED(src_width);
		KFL_UNUSED(src_height);
		KFL_UNUSED(filter);
	}

	void VulkanTexture::Map1D(uint32_t array_index, uint32_t level, TextureMapAccess tma, uint32_t x_offset, uint32_t width, void*& data)
	{
		KFL_UNUSED(array_index);
		KFL_UNUSED(level);
		KFL_UNUSED(tma);
		KFL_UNUSED(x_offset);
		KFL_UNUSED(width);

		data = nullptr;
	}

	void VulkanTexture::Map2D(uint32_t array_index, uint32_t level, TextureMapAccess tma, uint32_t x_offset, uint32_t y_offset,
		uint32_t width, uint32_t height, void*& data, uint32_t& row_pitch)
	{
		KFL_UNUSED(array_index);
		KFL_UNUSED(level);
		KFL_UNUSED(tma);
		KFL_UNUSED(x_offset);
		KFL_UNUSED(y_offset);
		KFL_UNUSED(width);
		KFL_UNUSED(height);

		data = nullptr;
		row_pitch = 0;
	}

	void VulkanTexture::Map3D(uint32_t array_index, uint32_t level, TextureMapAccess tma, uint32_t x_offset, uint32_t y_offset,
		uint32_t z_offset, uint32_t width, uint32_t height, uint32_t depth, void*& data, uint32_t& row_pitch, uint32_t& slice_pitch)
	{
		KFL_UNUSED(array_index);
		KFL_UNUSED(level);
		KFL_UNUSED(tma);
		KFL_UNUSED(x_offset);
		KFL_UNUSED(y_offset);
		KFL_UNUSED(z_offset);
		KFL_UNUSED(width);
		KFL_UNUSED(height);
		KFL_UNUSED(depth);

		data = nullptr;
		row_pitch = 0;
		slice_pitch = 0;
	}

	void VulkanTexture::MapCube(uint32_t array_index, CubeFaces face, uint32_t level, TextureMapAccess tma, uint32_t x_offset,
		uint32_t y_offset, uint32_t width, uint32_t height, void*& data, uint32_t& row_pitch)
	{
		KFL_UNUSED(array_index);
		KFL_UNUSED(face);
		KFL_UNUSED(level);
		KFL_UNUSED(tma);
		KFL_UNUSED(x_offset);
		KFL_UNUSED(y_offset);
		KFL_UNUSED(width);
		KFL_UNUSED(height);

		data = nullptr;
		row_pitch = 0;
	}

	void VulkanTexture::Unmap1D(uint32_t array_index, uint32_t level)
	{
		KFL_UNUSED(array_index);
		KFL_UNUSED(level);
	}

	void VulkanTexture::Unmap2D(uint32_t array_index, uint32_t level)
	{
		KFL_UNUSED(array_index);
		KFL_UNUSED(level);
	}

	void VulkanTexture::Unmap3D(uint32_t array_index, uint32_t level)
	{
		KFL_UNUSED(array_index);
		KFL_UNUSED(level);
	}

	void VulkanTexture::UnmapCube(uint32_t array_index, CubeFaces face, uint32_t level)
	{
		KFL_UNUSED(array_index);
		KFL_UNUSED(face);
		KFL_UNUSED(level);
	}

	void VulkanTexture::DoCreateHwResource(
		VkImageCreateInfo& createInfo, std::span<ElementInitData const> init_data, float4 const* clear_value_hint)
	{
		bool isDepth = IsDepthFormat(format_);
		createInfo.usage = 0;
		// TODO 内存访问控制
		if (access_hint_ & EAH_GPU_Read)
		{
			createInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
			if (!init_data.empty())
			{
				createInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			}
			if (!(access_hint_ & EAH_Immutable))
			{
				createInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			}
		}
		if (access_hint_ & EAH_GPU_Write)
		{
			if (isDepth)
			{
				createInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			}
			else
			{
				createInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			}
		}


		VK_CHECK_RESULT(vkCreateImage(device_->logicalDevice, &createInfo, nullptr, &image_));

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device_->logicalDevice, image_, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = device_->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vkAllocateMemory(device_->logicalDevice, &allocInfo, nullptr, &memory_);

		vkBindImageMemory(device_->logicalDevice, image_, memory_, 0);

		hw_ready_ = true;

		VkImageSubresourceRange range;
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		range.levelCount = 1;
		range.baseMipLevel = 0;
		range.layerCount = 1;
		range.baseArrayLayer = 0;
		//布局转换
		if (IsDepthFormat(format_))
		{
			range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL, range,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT);
		}
		else
		{
			if (!init_data.empty())
			{
				TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, range,
					VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
				uint32_t total_size = mipmap_start_offset_.back() * array_size_;

				VulkanGraphicsBuffer& buffer = VulkanGraphicsBuffer::GetStagingBuffer(device_);
				// char* staging;
				void* staging;
				vkMapMemory(device_->logicalDevice, buffer.GetVkDeviceMemory(), 0, total_size, 0, &staging);

				char* stagingChar = static_cast<char*>(staging);

				std::vector<VkBufferImageCopy> bufferCopyRegions;

				uint32_t offset = 0;
				uint32_t const texel_size = NumFormatBytes(format_);
				for (uint32_t array_index = 0; array_index < array_size_; ++array_index)
				{
					for (uint32_t level = 0; level < num_mip_maps_; ++level)
					{
						uint32_t const w = this->Width(level);
						uint32_t const h = this->Height(level);
						uint32_t const d = this->Depth(level);
						uint32_t index = array_index * num_mip_maps_ + level;

						uint32_t image_size = 0;
						if (IsCompressedFormat(format_))
						{
							image_size = ((w + 3) / 4) * ((h + 3) / 4) * d * texel_size * 4;
						}
						else
						{
							image_size = w * h * d * texel_size;
						}
						memcpy(stagingChar + offset, init_data[index].data, image_size);

						VkBufferImageCopy bufferCopyRegion = {};
						bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
						bufferCopyRegion.imageSubresource.mipLevel = level;
						bufferCopyRegion.imageSubresource.baseArrayLayer = array_index;
						bufferCopyRegion.imageSubresource.layerCount = array_size_;
						bufferCopyRegion.imageOffset = {0, 0, 0};
						bufferCopyRegion.imageExtent = {Width(level), Height(level), Depth(level)};
						bufferCopyRegion.bufferOffset = offset;
						bufferCopyRegions.push_back(bufferCopyRegion);

						offset += image_size;
					}
				}


				vkUnmapMemory(device_->logicalDevice, buffer.GetVkDeviceMemory());
				VkCommandBuffer commandBuffer = device_->BeginSingleTimeCommand(device_->commandPools.transfer);

				vkCmdCopyBufferToImage(commandBuffer, buffer.GetVkBuffer(), image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					static_cast<uint32_t>(bufferCopyRegions.size()), bufferCopyRegions.data());

				device_->EndSingleTimeCommand(device_->commandPools.transfer, device_->deviceQueues.transfer, commandBuffer);
			}
		}


		KFL_UNUSED(init_data);
		KFL_UNUSED(clear_value_hint);
	}

	void VulkanTexture::DeleteHWResource()
	{
		if (image_ != VK_NULL_HANDLE)
		{
			vkDestroyImage(device_->logicalDevice, image_, nullptr);
			vkFreeMemory(device_->logicalDevice, memory_, nullptr);
		}
	}

	bool VulkanTexture::HWResourceReady() const
	{
		return hw_ready_;
	}

	void VulkanTexture::UpdateSubresource1D(uint32_t array_index, uint32_t level, uint32_t x_offset, uint32_t width, void const* data)
	{
		KFL_UNUSED(array_index);
		KFL_UNUSED(level);
		KFL_UNUSED(x_offset);
		KFL_UNUSED(width);
		KFL_UNUSED(data);
	}

	void VulkanTexture::UpdateSubresource2D(uint32_t array_index, uint32_t level, uint32_t x_offset, uint32_t y_offset, uint32_t width,
		uint32_t height, void const* data, uint32_t row_pitch)
	{
		KFL_UNUSED(array_index);
		KFL_UNUSED(level);
		KFL_UNUSED(x_offset);
		KFL_UNUSED(y_offset);
		KFL_UNUSED(width);
		KFL_UNUSED(height);
		KFL_UNUSED(data);
		KFL_UNUSED(row_pitch);
	}

	void VulkanTexture::UpdateSubresource3D(uint32_t array_index, uint32_t level, uint32_t x_offset, uint32_t y_offset, uint32_t z_offset,
		uint32_t width, uint32_t height, uint32_t depth, void const* data, uint32_t row_pitch, uint32_t slice_pitch)
	{
		KFL_UNUSED(array_index);
		KFL_UNUSED(level);
		KFL_UNUSED(x_offset);
		KFL_UNUSED(y_offset);
		KFL_UNUSED(z_offset);
		KFL_UNUSED(width);
		KFL_UNUSED(height);
		KFL_UNUSED(depth);
		KFL_UNUSED(data);
		KFL_UNUSED(row_pitch);
		KFL_UNUSED(slice_pitch);
	}

	void VulkanTexture::UpdateSubresourceCube(uint32_t array_index, CubeFaces face, uint32_t level, uint32_t x_offset, uint32_t y_offset,
		uint32_t width, uint32_t height, void const* data, uint32_t row_pitch)
	{
		KFL_UNUSED(array_index);
		KFL_UNUSED(face);
		KFL_UNUSED(level);
		KFL_UNUSED(x_offset);
		KFL_UNUSED(y_offset);
		KFL_UNUSED(width);
		KFL_UNUSED(height);
		KFL_UNUSED(data);
		KFL_UNUSED(row_pitch);
	}

	void VulkanTexture::TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, VkImageSubresourceRange range,
		VkPipelineStageFlags oldStage, VkPipelineStageFlags newStage)
	{
		VkCommandBuffer commandBuffer = device_->BeginSingleTimeCommand(device_->commandPools.graphics);
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image_;
		barrier.subresourceRange = range;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = 0;
		switch (oldLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			barrier.srcAccessMask = 0;
			break;

		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		}

		switch (newLayout)
		{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			barrier.dstAccessMask = barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			if (barrier.srcAccessMask == 0)
			{
				barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			}
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		}

		vkCmdPipelineBarrier(commandBuffer, oldStage, newStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		device_->EndSingleTimeCommand(device_->commandPools.graphics, device_->deviceQueues.graphics, commandBuffer);
	}

	/*void VulkanTexture::FromBuffer(VulkanGraphicsBuffer buffer)
	{
		VkCommandBuffer commandBuffer = device_->BeginSingleTimeCommand(device_->commandPools.transfer);

		VkBufferImageCopy bufferCopyRegion = {};
		bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferCopyRegion.imageSubresource.mipLevel = 0;
		bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
		bufferCopyRegion.imageSubresource.layerCount = 1;
		bufferCopyRegion.imageOffset = {0, 0, 0};
		bufferCopyRegion.imageExtent = {Width(0), Height(0), 1};
		bufferCopyRegion.bufferOffset = 0;

		vkCmdCopyBufferToImage(commandBuffer, buffer.GetVkBuffer(), image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);

		device_->EndSingleTimeCommand(device_->commandPools.transfer, device_->deviceQueues.transfer, commandBuffer);
	}*/
} // namespace KlayGE
