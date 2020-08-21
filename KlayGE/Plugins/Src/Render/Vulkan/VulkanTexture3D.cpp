/**
 * @file VulkanTexture3D.cpp
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
	VulkanTexture3D::VulkanTexture3D(VulkanDevicePtr device, uint32_t width, uint32_t height, uint32_t depth, uint32_t numMipMaps, uint32_t array_size,
		ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint)
		: VulkanTexture(TT_3D, sample_count, sample_quality, access_hint)
	{
		device_ = device;
		width_ = width;
        depth_ = depth;
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
			uint32_t const d = this->Depth(level);

			uint32_t image_size;
			if (IsCompressedFormat(format_))
			{
				uint32_t const block_size = NumFormatBytes(format_) * 4;
				image_size = ((w + 3) / 4) * ((h + 3) / 4) * d * block_size;
			}
			else
			{
				uint32_t const texel_size = NumFormatBytes(format_);
				image_size = w * h * d * texel_size;
			}

			mipmap_start_offset_[level + 1] = mipmap_start_offset_[level] + image_size;
		}
	}

	void VulkanTexture3D::CreateHWResource(std::span<ElementInitData const> init_data, float4 const* clear_value_hint)
	{
		VkFormatFeatureFlags flag = 0;
		if (!device_->IsFormatSupported(vkFormat_, VK_IMAGE_TILING_OPTIMAL, flag))
		{
			TMSG("Not supported format");
		}

		VkImageCreateInfo iCI;
		iCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		iCI.imageType = VK_IMAGE_TYPE_3D;
		iCI.extent.width = width_;
		iCI.extent.height = height_;
		iCI.extent.depth = depth_;
		iCI.mipLevels = num_mip_maps_;
		iCI.arrayLayers = array_size_;

		iCI.format = vkFormat_;
		iCI.tiling = VK_IMAGE_TILING_OPTIMAL;
		iCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		//TODO ÄÚ´æ·ÃÎÊ¿ØÖÆ
		iCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		iCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		iCI.samples = VK_SAMPLE_COUNT_1_BIT;
		iCI.flags = 0;
		iCI.pNext = VK_NULL_HANDLE;

		this->DoCreateHwResource(iCI, init_data, clear_value_hint);
	}

	uint32_t VulkanTexture3D::Width(uint32_t level) const 
	{
		BOOST_ASSERT(level < num_mip_maps_);
		return std::max<uint32_t>(1U, width_ >> level);
	}
	uint32_t VulkanTexture3D::Height(uint32_t level) const
	{
		BOOST_ASSERT(level < num_mip_maps_);
		return std::max<uint32_t>(1U, height_ >> level);
	}

    uint32_t VulkanTexture3D::Depth(uint32_t level) const
	{
		BOOST_ASSERT(level < num_mip_maps_);
		return std::max<uint32_t>(1U, depth_ >> level);
	}

} // namespace KlayGE
