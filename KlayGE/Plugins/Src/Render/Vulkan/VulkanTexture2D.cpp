/**
 * @file VulkanTexture2D.cpp
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
#include <KlayGE/Vulkan/VulkanMapping.hpp>
#include <KlayGE/Vulkan/VulkanTexture.hpp>

namespace KlayGE
{
	VulkanTexture2D::VulkanTexture2D(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t numMipMaps, uint32_t array_size,
		ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint)
		: VulkanTexture(TT_2D, sample_count, sample_quality, access_hint)
	{
		device_ = device;
		width_ = width;
		height_ = height;
		num_mip_maps_ = numMipMaps;
		format_ = format;
		vkFormat_ = VulkanMapping::MappingFormat(format_);
		array_size_ = array_size;

		mipmap_start_offset_.resize(num_mip_maps_ + 1);
		mipmap_start_offset_[0] = 0;
		for (uint32_t level = 0; level < num_mip_maps_; ++ level)
		{
			uint32_t const w = this->Width(level);
			uint32_t const h = this->Height(level);

			uint32_t image_size;
			if (IsCompressedFormat(format_))
			{
				uint32_t const block_size = NumFormatBytes(format_) * 4;
				image_size = ((w + 3) / 4) * ((h + 3) / 4) * block_size;
			}
			else
			{
				uint32_t const texel_size = NumFormatBytes(format_);
				image_size = w * h * texel_size;
			}

			mipmap_start_offset_[level + 1] = mipmap_start_offset_[level] + image_size;
		}
	}

	void VulkanTexture2D::CreateHWResource(std::span<ElementInitData const> init_data, float4 const* clear_value_hint)
	{
		bool isDepth = IsDepthFormat(format_);
		VkFormatFeatureFlags flag = isDepth ? VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT : 0;
		if (!device_->IsFormatSupported(vkFormat_, VK_IMAGE_TILING_OPTIMAL, flag))
		{
			TMSG("Not supported format");
		}

		VkImageCreateInfo iCI;
		iCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		iCI.imageType = VK_IMAGE_TYPE_2D;
		iCI.extent.width = width_;
		iCI.extent.height = height_;
		iCI.extent.depth = 1;
		iCI.mipLevels = num_mip_maps_;
		iCI.arrayLayers = array_size_;

		iCI.format = vkFormat_;
		iCI.tiling = VK_IMAGE_TILING_OPTIMAL;
		iCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		iCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		iCI.samples = VK_SAMPLE_COUNT_1_BIT;
		iCI.flags = 0;
		iCI.pNext = VK_NULL_HANDLE;

		this->DoCreateHwResource(iCI, init_data, clear_value_hint);
	}

	uint32_t VulkanTexture2D::Width(uint32_t level) const 
	{
		BOOST_ASSERT(level < num_mip_maps_);
		return std::max<uint32_t>(1U, width_ >> level);
	}
	uint32_t VulkanTexture2D::Height(uint32_t level) const
	{
		BOOST_ASSERT(level < num_mip_maps_);
		return std::max<uint32_t>(1U, height_ >> level);
	}

} // namespace KlayGE
