/**
 * @file VulkanMapping.cpp
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

#include <KlayGE/Vulkan/VulkanMapping.hpp>
#include <KFL/ErrorHandling.hpp>

namespace KlayGE
{
    VkFormat VulkanMapping::MappingFormat(ElementFormat format)
    {
        switch (format)
		{
		// case EF_A8:
		// 	return DXGI_FORMAT_A8_UNORM;

		case EF_R5G6B5:
			return VK_FORMAT_R5G6B5_UNORM_PACK16;

		case EF_A1RGB5:
			return VK_FORMAT_A1R5G5B5_UNORM_PACK16;

		case EF_ARGB4:
			return VK_FORMAT_R4G4B4A4_UNORM_PACK16;

		case EF_R8:
			return VK_FORMAT_R8_UNORM;

		case EF_SIGNED_R8:
			return VK_FORMAT_R8_SNORM;

		case EF_GR8:
			return VK_FORMAT_R8G8_UNORM;

		case EF_SIGNED_GR8:
			return VK_FORMAT_R8G8_SNORM;

		case EF_ARGB8:
			return VK_FORMAT_R8G8B8A8_UNORM;

		case EF_ABGR8:
			return VK_FORMAT_A8B8G8R8_UNORM_PACK32;

		// case EF_SIGNED_ABGR8:
		// 	return DXGI_FORMAT_R8G8B8A8_SNORM;

		// case EF_A2BGR10:
		// 	return DXGI_FORMAT_R10G10B10A2_UNORM;

		// case EF_SIGNED_A2BGR10:
		// 	return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

		// case EF_R8UI:
		// 	return DXGI_FORMAT_R8_UINT;

		// case EF_R8I:
		// 	return DXGI_FORMAT_R8_SINT;

		// case EF_GR8UI:
		// 	return DXGI_FORMAT_R8G8_UINT;

		// case EF_GR8I:
		// 	return DXGI_FORMAT_R8G8_SINT;

		// case EF_ABGR8UI:
		// 	return DXGI_FORMAT_R8G8B8A8_UINT;

		// case EF_ABGR8I:
		// 	return DXGI_FORMAT_R8G8B8A8_SINT;

		// case EF_A2BGR10UI:
		// 	return DXGI_FORMAT_R10G10B10A2_UINT;

		// case EF_R16:
		// 	return VK_FORMAT_R16_UNORM;

		// case EF_SIGNED_R16:
		// 	return VK_FORMAT_R16_SNORM;

		// case EF_GR16:
		// 	return DXGI_FORMAT_R16G16_UNORM;

		// case EF_SIGNED_GR16:
		// 	return DXGI_FORMAT_R16G16_SNORM;

		// case EF_ABGR16:
		// 	return DXGI_FORMAT_R16G16B16A16_UNORM;

		// case EF_SIGNED_ABGR16:
		// 	return DXGI_FORMAT_R16G16B16A16_SNORM;

		// case EF_R16UI:
		// 	return DXGI_FORMAT_R16_UINT;

		// case EF_R16I:
		// 	return DXGI_FORMAT_R16_SINT;

		// case EF_GR16UI:
		// 	return DXGI_FORMAT_R16G16_UINT;

		// case EF_GR16I:
		// 	return DXGI_FORMAT_R16G16_SINT;

		// case EF_ABGR16UI:
		// 	return DXGI_FORMAT_R16G16B16A16_UINT;

		// case EF_ABGR16I:
		// 	return DXGI_FORMAT_R16G16B16A16_SINT;

		// case EF_R32UI:
		// 	return DXGI_FORMAT_R32_UINT;

		// case EF_R32I:
		// 	return DXGI_FORMAT_R32_SINT;

		// case EF_GR32UI:
		// 	return DXGI_FORMAT_R32G32_UINT;

		// case EF_GR32I:
		// 	return DXGI_FORMAT_R32G32_SINT;

		// case EF_BGR32UI:
		// 	return DXGI_FORMAT_R32G32B32_UINT;

		// case EF_BGR32I:
		// 	return DXGI_FORMAT_R32G32B32_SINT;

		// case EF_ABGR32UI:
		// 	return DXGI_FORMAT_R32G32B32A32_UINT;

		// case EF_ABGR32I:
		// 	return DXGI_FORMAT_R32G32B32A32_SINT;

		case EF_R16F:
			return VK_FORMAT_R16_SFLOAT;

		// case EF_GR16F:
		// 	return DXGI_FORMAT_R16G16_FLOAT;

		case EF_B10G11R11F:
			return VK_FORMAT_B10G11R11_UFLOAT_PACK32;

		// case EF_ABGR16F:
		// 	return DXGI_FORMAT_R16G16B16A16_FLOAT;

		case EF_R32F:
			return VK_FORMAT_R32_SFLOAT;

		// case EF_GR32F:
		// 	return DXGI_FORMAT_R32G32_FLOAT;

		// case EF_BGR32F:
		// 	return DXGI_FORMAT_R32G32B32_FLOAT;

		// case EF_ABGR32F:
		// 	return DXGI_FORMAT_R32G32B32A32_FLOAT;

		// case EF_BC1:
		// 	return DXGI_FORMAT_BC1_UNORM;

		// case EF_BC2:
		// 	return DXGI_FORMAT_BC2_UNORM;

		// case EF_BC3:
		// 	return DXGI_FORMAT_BC3_UNORM;

		// case EF_BC4:
		// 	return DXGI_FORMAT_BC4_UNORM;

		// case EF_SIGNED_BC4:
		// 	return DXGI_FORMAT_BC4_SNORM;

		// case EF_BC5:
		// 	return DXGI_FORMAT_BC5_UNORM;

		// case EF_SIGNED_BC5:
		// 	return DXGI_FORMAT_BC5_SNORM;

		// case EF_BC6:
		// 	return DXGI_FORMAT_BC6H_UF16;

		// case EF_SIGNED_BC6:
		// 	return DXGI_FORMAT_BC6H_SF16;

		// case EF_BC7:
		// 	return DXGI_FORMAT_BC7_UNORM;

		// case EF_D16:
		// 	return DXGI_FORMAT_D16_UNORM;

		case EF_D24S8:
			return VK_FORMAT_D24_UNORM_S8_UINT;

		// case EF_X24G8:
		// 	return DXGI_FORMAT_X24_TYPELESS_G8_UINT;

		case EF_D32F:
			return VK_FORMAT_D32_SFLOAT;
			
		// case EF_ARGB8_SRGB:
		// 	return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

		case EF_ABGR8_SRGB:
			return VK_FORMAT_A8B8G8R8_SRGB_PACK32;

		// case EF_BC1_SRGB:
		// 	return DXGI_FORMAT_BC1_UNORM_SRGB;

		// case EF_BC2_SRGB:
		// 	return DXGI_FORMAT_BC2_UNORM_SRGB;

		// case EF_BC3_SRGB:
		// 	return DXGI_FORMAT_BC3_UNORM_SRGB;

		// case EF_BC7_SRGB:
		// 	return DXGI_FORMAT_BC7_UNORM_SRGB;

		default:
			KFL_UNREACHABLE("Invalid format");
		}
    }
}
