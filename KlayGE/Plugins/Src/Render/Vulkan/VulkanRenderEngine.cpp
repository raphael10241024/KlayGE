/**
 * @file VulkanRenderEngine.cpp
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
#include <KFL/Hash.hpp>
#include <KlayGE/KlayGE.hpp>
#include <KlayGE/Vulkan/VulkanRenderEngine.hpp>
#include <KlayGE/Vulkan/VulkanRenderWindow.hpp>
#include <KlayGE/Vulkan/VulkanFrameBuffer.hpp>
#include <system_error>

namespace KlayGE
{
	VulkanRenderEngine::VulkanRenderEngine() : RenderEngine()
	{
#ifdef KLAYGE_DEBUG
		bool const debug_context = true;
#else
		bool const debug_context = Context::Instance().Config().graphics_cfg.debug_context;
#endif
		vulkanInstance = VulkanInstance("Vulkan", debug_context);
		vulkanDevice = MakeSharedPtr<VulkanDevice>(&vulkanInstance);
		vulkanDevice->CreateLogicalDevice();
	};
	VulkanRenderEngine::~VulkanRenderEngine()
	{
		this->Destroy();
	}

	std::wstring const& VulkanRenderEngine::Name() const
	{
		static std::wstring const name(L"Vulkan Render Engine");
		return name;
	}

	void VulkanRenderEngine::DoCreateRenderWindow(std::string const& name, RenderSettings const& settings)
	{
		// TODO»ñÈ¡°æ±¾ºÅ
		// int32_t version = vulkanDevice->properties.apiVersion;
		// VK_VERSION_MAJOR(version);
		native_shader_platform_name_ = "vulkan_1_0";
		this->FillRenderDeviceCaps();

		std::shared_ptr<VulkanRenderWindow> win = MakeSharedPtr<VulkanRenderWindow>(vulkanDevice, name, settings);
		this->BindFrameBuffer(win);

		drawCommandBuffers_.resize(win->CacheSize());
		VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
		cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufAllocateInfo.commandPool = vulkanDevice->commandPools.graphics;
		cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufAllocateInfo.commandBufferCount = static_cast<uint32_t>(win->CacheSize());
		VK_CHECK_RESULT(vkAllocateCommandBuffers(vulkanDevice->logicalDevice, &cmdBufAllocateInfo, drawCommandBuffers_.data()));
	}

	void VulkanRenderEngine::BeginFrame()
	{
		auto& v = checked_cast<VulkanRenderWindow&>(*screen_frame_buffer_);
		v.BeginFrame();
		cur_image_index_ =  v.cur_image_index;

		vkResetCommandBuffer(drawCommandBuffers_[cur_image_index_], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		VK_CHECK_RESULT(vkBeginCommandBuffer(drawCommandBuffers_[cur_image_index_], &beginInfo));

		VulkanRenderWindow& p = checked_cast<VulkanRenderWindow&>(*screen_frame_buffer_);
		p.Clear(FrameBuffer::CBM_Color | FrameBuffer::CBM_Depth, Color(0.5f, 0.5f, 0.5f, 1.0f), 1.0f, 0);
		VkRenderPassBeginInfo info = p.RenderPassInfo();
		vkCmdBeginRenderPass(drawCommandBuffers_[cur_image_index_], &info, VK_SUBPASS_CONTENTS_INLINE);
		
		vkCmdEndRenderPass(drawCommandBuffers_[cur_image_index_]);
	}

	void VulkanRenderEngine::EndFrame()
	{

	}

	void VulkanRenderEngine::ForceFlush()
	{
	}

	TexturePtr const& VulkanRenderEngine::ScreenDepthStencilTexture() const
	{
		return checked_cast<VulkanRenderWindow&>(*screen_frame_buffer_).GetVulkanDepthStencilTexture();
	}

	void VulkanRenderEngine::ScissorRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		VkRect2D rect2D {};
		rect2D.extent.width = width;
		rect2D.extent.height = height;
		rect2D.offset.x = x;
		rect2D.offset.y = y;
		vkCmdSetScissor(drawCommandBuffers_[cur_image_index_], 0, 1, &rect2D);
	}

	void VulkanRenderEngine::GetCustomAttrib(std::string_view name, void* value) const
	{
		size_t const name_hash = HashRange(name.begin(), name.end());
		if (CT_HASH("MAJOR_VERSION") == name_hash)
		{
			*static_cast<uint32_t*>(value) = major_version_;
		}
		else if (CT_HASH("MINOR_VERSION") == name_hash)
		{
			*static_cast<uint32_t*>(value) = minor_version_;
		}
		else if (CT_HASH("FRAG_DEPTH_SUPPORT") == name_hash)
		{
			*static_cast<bool*>(value) = frag_depth_support_;
		}
	}

	void VulkanRenderEngine::SetCustomAttrib(std::string_view name, void* value)
	{
		size_t const name_hash = HashRange(name.begin(), name.end());
		if (CT_HASH("PLATFORM") == name_hash)
		{
			native_shader_platform_name_ = *static_cast<std::string*>(value);

			if (native_shader_platform_name_.find("d3d_12") == 0)
			{
				shader_profiles_[static_cast<uint32_t>(ShaderStage::Vertex)] = "vs_5_1";
				shader_profiles_[static_cast<uint32_t>(ShaderStage::Pixel)] = "ps_5_1";
				shader_profiles_[static_cast<uint32_t>(ShaderStage::Geometry)] = "gs_5_1";
				shader_profiles_[static_cast<uint32_t>(ShaderStage::Compute)] = "cs_5_1";
				shader_profiles_[static_cast<uint32_t>(ShaderStage::Hull)] = "hs_5_1";
				shader_profiles_[static_cast<uint32_t>(ShaderStage::Domain)] = "ds_5_1";
			}
			else
			{
				shader_profiles_[static_cast<uint32_t>(ShaderStage::Vertex)] = "vs_5_0";
				shader_profiles_[static_cast<uint32_t>(ShaderStage::Pixel)] = "ps_5_0";
				shader_profiles_[static_cast<uint32_t>(ShaderStage::Geometry)] = "gs_5_0";
				shader_profiles_[static_cast<uint32_t>(ShaderStage::Compute)] = "cs_5_0";
				shader_profiles_[static_cast<uint32_t>(ShaderStage::Hull)] = "hs_5_0";
				shader_profiles_[static_cast<uint32_t>(ShaderStage::Domain)] = "ds_5_0";
			}
		}
		else if (CT_HASH("MAJOR_VERSION") == name_hash)
		{
			major_version_ = static_cast<uint8_t>(*static_cast<uint32_t*>(value));
		}
		else if (CT_HASH("MINOR_VERSION") == name_hash)
		{
			minor_version_ = static_cast<uint8_t>(*static_cast<uint32_t*>(value));
		}
		else if (CT_HASH("REQUIRES_FLIPPING") == name_hash)
		{
			requires_flipping_ = *static_cast<bool*>(value);
		}
		else if (CT_HASH("NATIVE_SHADER_FOURCC") == name_hash)
		{
			native_shader_fourcc_ = *static_cast<uint32_t*>(value);
		}
		else if (CT_HASH("NATIVE_SHADER_VERSION") == name_hash)
		{
			native_shader_version_ = *static_cast<uint32_t*>(value);
		}
		else if (CT_HASH("DEVICE_CAPS") == name_hash)
		{
			caps_ = *static_cast<RenderDeviceCaps*>(value);
		}
		else if (CT_HASH("FRAG_DEPTH_SUPPORT") == name_hash)
		{
			frag_depth_support_ = *static_cast<bool*>(value);
		}
	}

	void VulkanRenderEngine::DoBindFrameBuffer(FrameBufferPtr const& fb)
	{
		KFL_UNUSED(fb);
	}

	void VulkanRenderEngine::DoBindSOBuffers(RenderLayoutPtr const& rl)
	{
		KFL_UNUSED(rl);
	}

	void VulkanRenderEngine::DoRender(RenderEffect const& effect, RenderTechnique const& tech, RenderLayout const& rl)
	{
		//auto& fb = checked_cast<VulkanFrameBuffer&>(*this->CurFrameBuffer());
		KFL_UNUSED(effect);
		KFL_UNUSED(tech);
		KFL_UNUSED(rl);
	}

	void VulkanRenderEngine::DoDispatch(RenderEffect const& effect, RenderTechnique const& tech, uint32_t tgx, uint32_t tgy, uint32_t tgz)
	{
		KFL_UNUSED(effect);
		KFL_UNUSED(tech);
		KFL_UNUSED(tgx);
		KFL_UNUSED(tgy);
		KFL_UNUSED(tgz);
	}

	void VulkanRenderEngine::DoDispatchIndirect(
		RenderEffect const& effect, RenderTechnique const& tech, GraphicsBufferPtr const& buff_args, uint32_t offset)
	{
		KFL_UNUSED(effect);
		KFL_UNUSED(tech);
		KFL_UNUSED(buff_args);
		KFL_UNUSED(offset);
	}

	void VulkanRenderEngine::DoResize(uint32_t width, uint32_t height)
	{
		KFL_UNUSED(width);
		KFL_UNUSED(height);
	}

	void VulkanRenderEngine::DoDestroy()
	{
	}

	void VulkanRenderEngine::DoSuspend()
	{
	}

	void VulkanRenderEngine::DoResume()
	{
	}

	void VulkanRenderEngine::FillRenderDeviceCaps()
	{
		VkPhysicalDeviceLimits* limits = &vulkanDevice->properties.limits;
		caps_.max_texture_width = limits->maxImageDimension2D;
		caps_.max_texture_height = limits->maxImageDimension2D;
		caps_.max_texture_array_length = limits->maxImageArrayLayers;
		caps_.cs_support = false;
		std::vector<ElementFormat> vertex_formats = {EF_A8, EF_R8, EF_GR8, EF_BGR8, EF_ARGB8, EF_ABGR8, EF_R8UI, EF_GR8UI, EF_BGR8UI,
			EF_ABGR8UI, EF_SIGNED_R8, EF_SIGNED_GR8, EF_SIGNED_BGR8, EF_SIGNED_ABGR8, EF_R8I, EF_GR8I, EF_BGR8I, EF_ABGR8I, EF_A2BGR10,
			EF_R16, EF_GR16, EF_BGR16, EF_ABGR16, EF_R16UI, EF_GR16UI, EF_BGR16UI, EF_ABGR16UI, EF_SIGNED_R16, EF_SIGNED_GR16,
			EF_SIGNED_BGR16, EF_SIGNED_ABGR16, EF_R16I, EF_GR16I, EF_BGR16I, EF_ABGR16I, EF_R32UI, EF_GR32UI, EF_BGR32UI, EF_ABGR32UI,
			EF_R32I, EF_GR32I, EF_BGR32I, EF_ABGR32I, EF_R32F, EF_GR32F, EF_BGR32F, EF_ABGR32F, EF_SIGNED_A2BGR10, EF_R16F, EF_GR16F,
			EF_BGR16F, EF_ABGR16F};
		caps_.AssignVertexFormats(std::move(vertex_formats));

		std::vector<ElementFormat> texture_formats = {EF_A8, EF_ARGB4, EF_R8, EF_SIGNED_R8, EF_GR8, EF_SIGNED_GR8, EF_GR16, EF_SIGNED_GR16,
			EF_BGR8, EF_SIGNED_BGR8, EF_SIGNED_ABGR8, EF_ARGB8, EF_ABGR8, EF_A2BGR10, EF_SIGNED_A2BGR10, EF_R16, EF_SIGNED_R16, EF_R8UI,
			EF_R8I, EF_GR8UI, EF_GR8I, EF_BGR8UI, EF_BGR8I, EF_ABGR8UI, EF_ABGR8I, EF_R16UI, EF_R16I, EF_GR16UI, EF_GR16I, EF_BGR16UI,
			EF_BGR16I, EF_ABGR16UI, EF_ABGR16I, EF_R32UI, EF_R32I, EF_GR32UI, EF_GR32I, EF_BGR32UI, EF_BGR32I, EF_ABGR32UI, EF_ABGR32I,
			EF_BGR16, EF_SIGNED_BGR16, EF_ABGR16, EF_SIGNED_ABGR16, EF_R16F, EF_GR16F, EF_B10G11R11F, EF_BGR16F, EF_ABGR16F, EF_R32F,
			EF_GR32F, EF_BGR32F, EF_ABGR32F, EF_BC4, EF_BC5, EF_SIGNED_BC4, EF_SIGNED_BC5, EF_D16, EF_D24S8, EF_D32F, EF_ARGB8_SRGB,
			EF_ABGR8_SRGB, EF_BC4_SRGB, EF_BC5_SRGB};

		caps_.AssignTextureFormats(std::move(texture_formats));

		int max_samples = caps_.max_simultaneous_rts;
		std::map<ElementFormat, std::vector<uint32_t>> render_target_formats;
			auto add_render_target_format = [&render_target_formats, &max_samples](std::span<ElementFormat const> fmts)
			{
				for (auto fmt : fmts)
				{
					for (int i = 1; i <= max_samples; i *= 2)
					{
						render_target_formats[fmt].push_back(RenderDeviceCaps::EncodeSampleCountQuality(i, 1));
					}
				}
			};
		add_render_target_format(MakeSpan({EF_R8, EF_GR8, EF_ARGB8, EF_ABGR8, EF_SIGNED_ABGR8, EF_A2BGR10, EF_SIGNED_A2BGR10, EF_ABGR8UI,
			EF_ABGR8I, EF_R16, EF_SIGNED_R16, EF_GR16, EF_SIGNED_GR16, EF_ABGR16, EF_SIGNED_ABGR16, EF_R16UI, EF_R16I, EF_GR16UI, EF_GR16I,
			EF_ABGR16UI, EF_ABGR16I, EF_R32UI, EF_R32I, EF_GR32UI, EF_GR32I, EF_ABGR32UI, EF_ABGR32I, EF_R16F, EF_GR16F, EF_R32F, EF_GR32F,
			EF_ABGR16F, EF_B10G11R11F, EF_ABGR32F, EF_D16, EF_D24S8, EF_D32F, EF_ARGB8_SRGB, EF_ABGR8_SRGB}));

		caps_.AssignRenderTargetFormats(std::move(render_target_formats));
	}

	bool VulkanRenderEngine::FullScreen() const
	{
		return false;
	}

	void VulkanRenderEngine::FullScreen(bool fs)
	{
		KFL_UNUSED(fs);
	}

	VulkanDevicePtr VulkanRenderEngine::GetVulkanDevicePtr() const
	{
		return vulkanDevice;
	}
} // namespace KlayGE
