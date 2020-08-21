/**
 * @file VulkanRenderFactory.cpp
 * @author Minmin Gong
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

#include <KlayGE/Vulkan/VulkanRenderEngine.hpp>
#include <KlayGE/Vulkan/VulkanRenderStateObject.hpp>
#include <KlayGE/Vulkan/VulkanShaderObject.hpp>
#include <KlayGE/Vulkan/VulkanTexture.hpp>
#include <KlayGE/Vulkan/VulkanRenderLayout.hpp>
#include <KlayGE/Vulkan/VulkanGraphicsBuffer.hpp>
#include <KlayGE/Vulkan/VulkanRenderFactory.hpp>
#include <KlayGE/Vulkan/VulkanFrameBuffer.hpp>
#include <KlayGE/Vulkan/VulkanRenderView.hpp>

namespace KlayGE
{
	VulkanRenderFactory::VulkanRenderFactory() = default;

	std::wstring const& VulkanRenderFactory::Name() const
	{
		static std::wstring const name(L"Vulkan Render Factory");
		return name;
	}

	TexturePtr VulkanRenderFactory::MakeDelayCreationTexture1D(uint32_t width, uint32_t num_mip_maps, uint32_t array_size,
		ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint)
	{
		KFL_UNUSED(width);
		KFL_UNUSED(num_mip_maps);
		KFL_UNUSED(array_size);
		KFL_UNUSED(format);
		KFL_UNUSED(sample_count);
		KFL_UNUSED(sample_quality);
		KFL_UNUSED(access_hint);
		return nullptr;
	}
	TexturePtr VulkanRenderFactory::MakeDelayCreationTexture2D(uint32_t width, uint32_t height, uint32_t num_mip_maps, uint32_t array_size,
		ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint)
	{
		return MakeSharedPtr<VulkanTexture2D>(GetVulkanDevicePtr(), width, height, num_mip_maps, array_size,
		format, sample_count, sample_quality, access_hint);
	}
	TexturePtr VulkanRenderFactory::MakeDelayCreationTexture3D(uint32_t width, uint32_t height, uint32_t depth, uint32_t num_mip_maps,
		uint32_t array_size, ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint)
	{
		return MakeSharedPtr<VulkanTexture3D>(GetVulkanDevicePtr(), width, height, depth, num_mip_maps, array_size,
			format, sample_count, sample_quality, access_hint);
	}
	TexturePtr VulkanRenderFactory::MakeDelayCreationTextureCube(uint32_t size, uint32_t num_mip_maps, uint32_t array_size,
		ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint)
	{
		KFL_UNUSED(size);
		KFL_UNUSED(num_mip_maps);
		KFL_UNUSED(array_size);
		KFL_UNUSED(format);
		KFL_UNUSED(sample_count);
		KFL_UNUSED(sample_quality);
		KFL_UNUSED(access_hint);
		return nullptr;
	}

	FrameBufferPtr VulkanRenderFactory::MakeFrameBuffer()
	{
		return MakeSharedPtr<VulkanCommonFrameBuffer>(GetVulkanDevicePtr());
	}

	RenderLayoutPtr VulkanRenderFactory::MakeRenderLayout()
	{
		return MakeSharedPtr<VulkanRenderLayout>();
	}

	GraphicsBufferPtr VulkanRenderFactory::MakeDelayCreationVertexBuffer(
		BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, uint32_t structure_byte_stride)
	{
		auto const& re = dynamic_cast<VulkanRenderEngine&>(Context::Instance().RenderFactoryInstance().RenderEngineInstance());

		return MakeSharedPtr<VulkanGraphicsBuffer>(
			re.GetVulkanDevicePtr(), usage, access_hint, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, size_in_byte, structure_byte_stride);
	}

	GraphicsBufferPtr VulkanRenderFactory::MakeDelayCreationIndexBuffer(
		BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, uint32_t structure_byte_stride)
	{
		auto const& re = dynamic_cast<VulkanRenderEngine&>(Context::Instance().RenderFactoryInstance().RenderEngineInstance());

		return MakeSharedPtr<VulkanGraphicsBuffer>(
			re.GetVulkanDevicePtr(), usage, access_hint, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, size_in_byte, structure_byte_stride);
	}

	GraphicsBufferPtr VulkanRenderFactory::MakeDelayCreationConstantBuffer(
		BufferUsage usage, uint32_t access_hint, uint32_t size_in_byte, uint32_t structure_byte_stride)
	{
		auto const& re = dynamic_cast<VulkanRenderEngine&>(Context::Instance().RenderFactoryInstance().RenderEngineInstance());
		return MakeSharedPtr<VulkanGraphicsBuffer>(
			re.GetVulkanDevicePtr(), usage, access_hint, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, size_in_byte, structure_byte_stride);
	}

	QueryPtr VulkanRenderFactory::MakeOcclusionQuery()
	{
		return QueryPtr();
	}

	QueryPtr VulkanRenderFactory::MakeConditionalRender()
	{
		return QueryPtr();
	}

	QueryPtr VulkanRenderFactory::MakeTimerQuery()
	{
		return QueryPtr();
	}

	QueryPtr VulkanRenderFactory::MakeSOStatisticsQuery()
	{
		return QueryPtr();
	}

	FencePtr VulkanRenderFactory::MakeFence()
	{
		return FencePtr();
	}

	ShaderResourceViewPtr VulkanRenderFactory::MakeTextureSrv(TexturePtr const& texture, ElementFormat pf, uint32_t first_array_index,
		uint32_t array_size, uint32_t first_level, uint32_t num_levels)
	{
		return MakeSharedPtr<VulkanShaderResourceView>(texture, pf, first_array_index, array_size, first_level, num_levels);
	}

	ShaderResourceViewPtr VulkanRenderFactory::MakeTexture2DSrv(
		TexturePtr const& texture, ElementFormat pf, int array_index, Texture::CubeFaces face, uint32_t first_level, uint32_t num_levels)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(array_index);
		KFL_UNUSED(face);
		KFL_UNUSED(first_level);
		KFL_UNUSED(num_levels);
		return MakeSharedPtr<VulkanShaderResourceView>(texture, pf, 0, 1, first_level, num_levels);
	}

	ShaderResourceViewPtr VulkanRenderFactory::MakeBufferSrv(
		GraphicsBufferPtr const& gbuffer, ElementFormat pf, uint32_t first_elem, uint32_t num_elems)
	{
		KFL_UNUSED(gbuffer);
		KFL_UNUSED(pf);
		KFL_UNUSED(first_elem);
		KFL_UNUSED(num_elems);
		return ShaderResourceViewPtr();
	}

	RenderTargetViewPtr VulkanRenderFactory::Make1DRtv(
		TexturePtr const& texture, ElementFormat pf, int first_array_index, int array_size, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(first_array_index);
		KFL_UNUSED(array_size);
		KFL_UNUSED(level);
		return RenderTargetViewPtr();
	}

	RenderTargetViewPtr VulkanRenderFactory::Make2DRtv(
		TexturePtr const& texture, ElementFormat pf, int first_array_index, int array_size, int level)
	{
		return MakeSharedPtr<VulkanRenderTargetView>(texture, pf, first_array_index, array_size, level);
	}

	RenderTargetViewPtr VulkanRenderFactory::Make2DRtv(
		TexturePtr const& texture, ElementFormat pf, int array_index, Texture::CubeFaces face, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(array_index);
		KFL_UNUSED(face);
		KFL_UNUSED(level);
		return RenderTargetViewPtr();
	}

	RenderTargetViewPtr VulkanRenderFactory::Make2DRtv(
		TexturePtr const& texture, ElementFormat pf, int array_index, uint32_t slice, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(array_index);
		KFL_UNUSED(slice);
		KFL_UNUSED(level);
		return RenderTargetViewPtr();
	}

	RenderTargetViewPtr VulkanRenderFactory::Make3DRtv(
		TexturePtr const& texture, ElementFormat pf, int array_index, uint32_t first_slice, uint32_t num_slices, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(array_index);
		KFL_UNUSED(first_slice);
		KFL_UNUSED(num_slices);
		KFL_UNUSED(level);
		return RenderTargetViewPtr();
	}

	RenderTargetViewPtr VulkanRenderFactory::MakeCubeRtv(TexturePtr const& texture, ElementFormat pf, int array_index, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(array_index);
		KFL_UNUSED(level);
		return RenderTargetViewPtr();
	}

	RenderTargetViewPtr VulkanRenderFactory::MakeBufferRtv(
		GraphicsBufferPtr const& gbuffer, ElementFormat pf, uint32_t first_elem, uint32_t num_elems)
	{
		KFL_UNUSED(gbuffer);
		KFL_UNUSED(pf);
		KFL_UNUSED(first_elem);
		KFL_UNUSED(num_elems);
		return RenderTargetViewPtr();
	}

	DepthStencilViewPtr VulkanRenderFactory::Make2DDsv(
		uint32_t width, uint32_t height, ElementFormat pf, uint32_t sample_count, uint32_t sample_quality)
	{
		KFL_UNUSED(width);
		KFL_UNUSED(height);
		KFL_UNUSED(pf);
		KFL_UNUSED(sample_count);
		KFL_UNUSED(sample_quality);
		return DepthStencilViewPtr();
	}

	DepthStencilViewPtr VulkanRenderFactory::Make1DDsv(
		TexturePtr const& texture, ElementFormat pf, int first_array_index, int array_size, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(first_array_index);
		KFL_UNUSED(array_size);
		KFL_UNUSED(level);
		return DepthStencilViewPtr();
	}

	DepthStencilViewPtr VulkanRenderFactory::Make2DDsv(
		TexturePtr const& texture, ElementFormat pf, int first_array_index, int array_size, int level)
	{
		return MakeSharedPtr<VulkanDepthStencilView>(texture, pf, first_array_index, array_size, level);
	}

	DepthStencilViewPtr VulkanRenderFactory::Make2DDsv(
		TexturePtr const& texture, ElementFormat pf, int array_index, Texture::CubeFaces face, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(array_index);
		KFL_UNUSED(face);
		KFL_UNUSED(level);
		return DepthStencilViewPtr();
	}

	DepthStencilViewPtr VulkanRenderFactory::Make2DDsv(
		TexturePtr const& texture, ElementFormat pf, int array_index, uint32_t slice, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(array_index);
		KFL_UNUSED(slice);
		KFL_UNUSED(level);
		return DepthStencilViewPtr();
	}

	DepthStencilViewPtr VulkanRenderFactory::Make3DDsv(
		TexturePtr const& texture, ElementFormat pf, int array_index, uint32_t first_slice, uint32_t num_slices, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(array_index);
		KFL_UNUSED(first_slice);
		KFL_UNUSED(num_slices);
		KFL_UNUSED(level);
		return DepthStencilViewPtr();
	}

	DepthStencilViewPtr VulkanRenderFactory::MakeCubeDsv(TexturePtr const& texture, ElementFormat pf, int array_index, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(array_index);
		KFL_UNUSED(level);
		return DepthStencilViewPtr();
	}

	UnorderedAccessViewPtr VulkanRenderFactory::Make1DUav(
		TexturePtr const& texture, ElementFormat pf, int first_array_index, int array_size, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(first_array_index);
		KFL_UNUSED(array_size);
		KFL_UNUSED(level);
		return UnorderedAccessViewPtr();
	}

	UnorderedAccessViewPtr VulkanRenderFactory::Make2DUav(
		TexturePtr const& texture, ElementFormat pf, int first_array_index, int array_size, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(first_array_index);
		KFL_UNUSED(array_size);
		KFL_UNUSED(level);
		return UnorderedAccessViewPtr();
	}

	UnorderedAccessViewPtr VulkanRenderFactory::Make2DUav(
		TexturePtr const& texture, ElementFormat pf, int array_index, Texture::CubeFaces face, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(array_index);
		KFL_UNUSED(face);
		KFL_UNUSED(level);
		return UnorderedAccessViewPtr();
	}

	UnorderedAccessViewPtr VulkanRenderFactory::Make2DUav(
		TexturePtr const& texture, ElementFormat pf, int array_index, uint32_t slice, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(array_index);
		KFL_UNUSED(slice);
		KFL_UNUSED(level);
		return UnorderedAccessViewPtr();
	}

	UnorderedAccessViewPtr VulkanRenderFactory::MakeCubeUav(TexturePtr const& texture, ElementFormat pf, int array_index, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(array_index);
		KFL_UNUSED(level);
		return UnorderedAccessViewPtr();
	}

	UnorderedAccessViewPtr VulkanRenderFactory::Make3DUav(
		TexturePtr const& texture, ElementFormat pf, int array_index, uint32_t first_slice, uint32_t num_slices, int level)
	{
		KFL_UNUSED(texture);
		KFL_UNUSED(pf);
		KFL_UNUSED(array_index);
		KFL_UNUSED(first_slice);
		KFL_UNUSED(num_slices);
		KFL_UNUSED(level);
		return UnorderedAccessViewPtr();
	}

	UnorderedAccessViewPtr VulkanRenderFactory::MakeBufferUav(
		GraphicsBufferPtr const& gbuffer, ElementFormat pf, uint32_t first_elem, uint32_t num_elems)
	{
		KFL_UNUSED(gbuffer);
		KFL_UNUSED(pf);
		KFL_UNUSED(first_elem);
		KFL_UNUSED(num_elems);
		return UnorderedAccessViewPtr();
	}

	ShaderObjectPtr VulkanRenderFactory::MakeShaderObject()
	{
		return MakeSharedPtr<VulkanShaderObject>();
	}

	ShaderStageObjectPtr VulkanRenderFactory::MakeShaderStageObject(ShaderStage stage)
	{
		std::shared_ptr<VulkanShaderStageObject> ret;
		switch (stage)
		{
		case ShaderStage::Vertex:
			ret = MakeSharedPtr<VulkanVertexShaderStageObject>();
			break;

		case ShaderStage::Pixel:
			ret = MakeSharedPtr<VulkanPixelShaderStageObject>();
			break;

		case ShaderStage::Geometry:
			ret = MakeSharedPtr<VulkanGeometryShaderStageObject>();
			break;

		case ShaderStage::Compute:
			ret = MakeSharedPtr<VulkanComputeShaderStageObject>();
			break;

		case ShaderStage::Hull:
			ret = MakeSharedPtr<VulkanHullShaderStageObject>();
			break;

		case ShaderStage::Domain:
			ret = MakeSharedPtr<VulkanDomainShaderStageObject>();
			break;

		default:
			KFL_UNREACHABLE("Invalid shader stage");
		}
		return ret;
	}

	std::unique_ptr<RenderEngine> VulkanRenderFactory::DoMakeRenderEngine()
	{
		return MakeUniquePtr<VulkanRenderEngine>();
	}

	RenderStateObjectPtr VulkanRenderFactory::DoMakeRenderStateObject(
		RasterizerStateDesc const& rs_desc, DepthStencilStateDesc const& dss_desc, BlendStateDesc const& bs_desc)
	{
		return MakeSharedPtr<VulkanRenderStateObject>(rs_desc, dss_desc, bs_desc);
	}

	SamplerStateObjectPtr VulkanRenderFactory::DoMakeSamplerStateObject(SamplerStateDesc const& desc)
	{
		return MakeSharedPtr<VulkanSamplerStateObject>(desc);
	}

	void VulkanRenderFactory::DoSuspend()
	{
	}

	void VulkanRenderFactory::DoResume()
	{
	}

	VulkanDevicePtr VulkanRenderFactory::GetVulkanDevicePtr()
	{
		static VulkanDevicePtr ptr = nullptr;
		if (ptr == nullptr)
		{
			auto const& re = dynamic_cast<VulkanRenderEngine&>(Context::Instance().RenderFactoryInstance().RenderEngineInstance());
			ptr = re.GetVulkanDevicePtr();
		}
		return ptr;
	}
} // namespace KlayGE

extern "C"
{
	KLAYGE_SYMBOL_EXPORT void MakeRenderFactory(std::unique_ptr<KlayGE::RenderFactory>& ptr)
	{
		ptr = KlayGE::MakeUniquePtr<KlayGE::VulkanRenderFactory>();
	}
}
