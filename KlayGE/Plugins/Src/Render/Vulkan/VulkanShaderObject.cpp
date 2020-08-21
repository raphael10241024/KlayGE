/**
 * @file VulkanShaderObject.cpp
 * @author Zhangyakun
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

#include <KlayGE/KlayGE.hpp>

#include <KFL/CXX17/string_view.hpp>
#include <KFL/CXX2a/format.hpp>
#include <KFL/CustomizedStreamBuf.hpp>
#include <KFL/ErrorHandling.hpp>
#include <KFL/Hash.hpp>
#include <KFL/Math.hpp>
#include <KFL/Matrix.hpp>
#include <KFL/ResIdentifier.hpp>
#include <KFL/Util.hpp>
#include <KlayGE/Context.hpp>
#include <KlayGE/RenderEffect.hpp>
#include <KlayGE/RenderEngine.hpp>
#include <KlayGE/RenderFactory.hpp>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>

#include <boost/assert.hpp>

#include <glloader/glloader.h>

#include <KlayGE/Vulkan/VulkanRenderEngine.hpp>
#include <KlayGE/Vulkan/VulkanRenderFactory.hpp>
// #include <KlayGE/Vulkan/VulkanRenderView.hpp>
// #include <KlayGE/Vulkan/VulkanMapping.hpp>
#include <KlayGE/Vulkan/VulkanGraphicsBuffer.hpp>
#include <KlayGE/Vulkan/VulkanRenderStateObject.hpp>
#include <KlayGE/Vulkan/VulkanShaderObject.hpp>
#include <KlayGE/Vulkan/VulkanTexture.hpp>


#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

namespace KlayGE
{

	char const* default_shader_profiles[] = {
		"vs_5_0",
		"ps_5_0",
		"gs_5_0",
		"cs_5_0",
		"hs_5_0",
		"ds_5_0",
	};

	VulkanShaderStageObject::VulkanShaderStageObject(ShaderStage stage):ShaderStageObject(stage)
	{

	}
	
	VulkanShaderStageObject::~VulkanShaderStageObject()
	{
		//if (shaderModule)
		//{
		//	auto const& re = dynamic_cast<VulkanRenderEngine&>(Context::Instance().RenderFactoryInstance().RenderEngineInstance());
		//	vkDestroyShaderModule(re.GetVulkanDevicePtr()->logicalDevice, shaderModule, nullptr);
		//}
	}

	void VulkanShaderStageObject::StreamIn(
		RenderEffect const& effect, std::array<uint32_t, NumShaderStages> const& shader_desc_ids, ResIdentifier& res)
	{
		uint32_t native_shader_block_len;
		res.read(&native_shader_block_len, sizeof(native_shader_block_len));
		native_shader_block_len = LE2Native(native_shader_block_len);

		auto const& sd = effect.GetShaderDesc(shader_desc_ids[static_cast<uint32_t>(stage_)]);

		shader_func_name_ = sd.func_name;

		is_validate_ = false;
	}
	void VulkanShaderStageObject::StreamOut(std::ostream& os)
	{
		std::vector<char> native_shader_block;

		if (!spirv_code_.empty())
		{
			VectorOutputStreamBuf native_shader_buff(native_shader_block);
			std::ostream oss(&native_shader_buff);

			uint32_t len32 = Native2LE(static_cast<uint32_t>(spirv_code_.size()));
			oss.write(reinterpret_cast<char const*>(&len32), sizeof(len32));
			//oss.write(&spriv_code_[0], spriv_code_.size());

			uint16_t num16 = Native2LE(static_cast<uint16_t>(pnames_.size()));
			oss.write(reinterpret_cast<char const*>(&num16), sizeof(num16));
			for (size_t i = 0; i < pnames_.size(); ++i)
			{
				uint8_t len8 = static_cast<uint8_t>(pnames_[i].size());
				oss.write(reinterpret_cast<char const*>(&len8), sizeof(len8));
				oss.write(&pnames_[i][0], pnames_[i].size());
			}

			num16 = Native2LE(static_cast<uint16_t>(glsl_res_names_.size()));
			oss.write(reinterpret_cast<char const*>(&num16), sizeof(num16));
			for (size_t i = 0; i < glsl_res_names_.size(); ++i)
			{
				uint8_t len8 = static_cast<uint8_t>(glsl_res_names_[i].size());
				oss.write(reinterpret_cast<char const*>(&len8), sizeof(len8));
				oss.write(&glsl_res_names_[i][0], glsl_res_names_[i].size());
			}

			num16 = Native2LE(static_cast<uint16_t>(tex_sampler_pairs_.size()));
			oss.write(reinterpret_cast<char const*>(&num16), sizeof(num16));
			for (size_t i = 0; i < num16; ++i)
			{
				uint8_t len8 = static_cast<uint8_t>(tex_sampler_pairs_[i].first.size());
				oss.write(reinterpret_cast<char const*>(&len8), sizeof(len8));
				oss.write(&tex_sampler_pairs_[i].first[0], len8);

				len8 = static_cast<uint8_t>(tex_sampler_pairs_[i].second.size());
				oss.write(reinterpret_cast<char const*>(&len8), sizeof(len8));
				oss.write(&tex_sampler_pairs_[i].second[0], len8);
			}

			this->StageSpecificStreamOut(oss);
		}

		uint32_t len = static_cast<uint32_t>(native_shader_block.size());
		{
			uint32_t tmp = Native2LE(len);
			os.write(reinterpret_cast<char const*>(&tmp), sizeof(tmp));
		}
		if (len > 0)
		{
			os.write(reinterpret_cast<char const*>(&native_shader_block[0]), len * sizeof(native_shader_block[0]));
		}
	}
	void VulkanShaderStageObject::CompileShader(RenderEffect const& effect, RenderTechnique const& tech, RenderPass const& pass,
		std::array<uint32_t, NumShaderStages> const& shader_desc_ids)
	{
		ShaderDesc const& sd = effect.GetShaderDesc(shader_desc_ids[static_cast<uint32_t>(stage_)]);

		shader_func_name_ = sd.func_name;

		is_validate_ = true;

		spirv_code_.clear();

		if (is_validate_)
		{
			auto const& re = Context::Instance().RenderFactoryInstance().RenderEngineInstance();
			auto const& caps = re.DeviceCaps();

			std::string_view const shader_profile = this->GetShaderProfile(effect, shader_desc_ids[static_cast<uint32_t>(stage_)]);
			is_validate_ = !shader_profile.empty();

			if (is_validate_)
			{
				std::string err_msg;
				std::vector<std::pair<char const*, char const*>> macros;
				macros.emplace_back("KLAYGE_VULKAN", "1");
				if (!caps.TextureFormatSupport(EF_BC5) || !caps.TextureFormatSupport(EF_BC5_SRGB))
				{
					macros.emplace_back("KLAYGE_BC5_AS_AG", "1");
				}
				if (!caps.TextureFormatSupport(EF_BC4) || !caps.TextureFormatSupport(EF_BC4_SRGB))
				{
					macros.emplace_back("KLAYGE_BC4_AS_G", "1");
				}
				macros.emplace_back("KLAYGE_FRAG_DEPTH", "1");


				spirv_code_ = ShaderStageObject::CompileToSPIRV(
					stage_, effect, tech, pass, macros, sd.func_name.c_str(), shader_profile.data(), 0);
				if (spirv_code_.empty())
				{
					is_validate_ = false;
				}
				else
				{
					shader_desc_ = ShaderStageObject::ReflectSPIRV(spirv_code_);
					this->FillCBufferIndices(effect);
				}
			}
		}
	}

	void VulkanShaderStageObject::FillCBufferIndices(RenderEffect const& effect)
	{
		if (!shader_desc_.cb_desc.empty())
		{
			cbuff_indices_.resize(shader_desc_.cb_desc.size());
		}
		for (size_t c = 0; c < shader_desc_.cb_desc.size(); ++c)
		{
			uint32_t i = 0;
			for (; i < effect.NumCBuffers(); ++i)
			{
				if (effect.CBufferByIndex(i)->NameHash() == shader_desc_.cb_desc[c].name_hash)
				{
					cbuff_indices_[c] = static_cast<uint8_t>(i);
					break;
				}
			}
			BOOST_ASSERT(i < effect.NumCBuffers());
		}
	}


	void VulkanShaderStageObject::CreateHwShader(RenderEffect const& effect, std::array<uint32_t, NumShaderStages> const& shader_desc_ids)
	{
		KFL_UNUSED(effect);
		KFL_UNUSED(shader_desc_ids);

		if (!spirv_code_.empty())
		{
			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = spirv_code_.size() * sizeof(uint32_t);
			createInfo.pCode = spirv_code_.data();

			auto const& re = dynamic_cast<VulkanRenderEngine&>(Context::Instance().RenderFactoryInstance().RenderEngineInstance());

			VK_CHECK_RESULT(vkCreateShaderModule(re.GetVulkanDevicePtr()->logicalDevice, &createInfo, nullptr, &shaderModule));
		}
		else
		{
			is_validate_ = false;
		}
		hw_res_ready_ = true;

	}
	std::string_view VulkanShaderStageObject::GetShaderProfile(RenderEffect const& effect, uint32_t shader_desc_id) const
	{
		std::string_view shader_profile = effect.GetShaderDesc(shader_desc_id).profile;
		if (is_available_)
		{
			if (shader_profile == "auto")
			{
				shader_profile = default_shader_profiles[static_cast<uint32_t>(stage_)];
			}
		}
		else
		{
			shader_profile = std::string_view();
		}

		return shader_profile;
	}


	VulkanVertexShaderStageObject::VulkanVertexShaderStageObject():VulkanShaderStageObject(ShaderStage::Vertex)
	{
	}

	VulkanPixelShaderStageObject::VulkanPixelShaderStageObject():VulkanShaderStageObject(ShaderStage::Pixel)
	{
	}

		VulkanComputeShaderStageObject::VulkanComputeShaderStageObject():VulkanShaderStageObject(ShaderStage::Compute)
	{
	}

		VulkanDomainShaderStageObject::VulkanDomainShaderStageObject():VulkanShaderStageObject(ShaderStage::Domain)
	{
	}

	VulkanGeometryShaderStageObject::VulkanGeometryShaderStageObject():VulkanShaderStageObject(ShaderStage::Geometry)
	{

	}

	VulkanHullShaderStageObject::VulkanHullShaderStageObject():VulkanShaderStageObject(ShaderStage::Hull)
	{

	}

	VulkanShaderObject::~VulkanShaderObject()
	{
	}
	ShaderObjectPtr VulkanShaderObject::Clone(RenderEffect const& effect)
	{
		VulkanShaderObjectPtr ret = MakeSharedPtr<VulkanShaderObject>(so_template_);
		ret->is_validate_ = is_validate_;
		ret->hw_res_ready_ = hw_res_ready_;
		// ret->uavsrcs_.resize(uavsrcs_.size(), nullptr);
		// ret->uavs_.resize(uavs_.size());
		// ret->uav_init_counts_.resize(uav_init_counts_.size());

		// for (size_t i = 0; i < NumShaderStages; ++ i)
		//{
			// ret->samplers_[i] = samplers_[i];
			// ret->srvsrcs_[i].resize(srvsrcs_[i].size(), std::make_tuple(static_cast<void*>(nullptr), 0, 0));
			// ret->srvs_[i].resize(srvs_[i].size());

			// ret->param_binds_[i].reserve(param_binds_[i].size());
			// for (auto const & pb : param_binds_[i])
			// {
				// ret->param_binds_[i].push_back(ret->GetBindFunc(static_cast<ShaderStage>(i), pb.offset,
					// effect.ParameterByName(pb.param->Name())));
			// }
		// }
		KFL_UNUSED(effect);
		return ret;
	}
	VulkanShaderObject::VulkanShaderObject()
	{
	}

	VulkanShaderObject::VulkanShaderObject(
		std::shared_ptr<ShaderObjectTemplate> so_template)
		: ShaderObject(std::move(so_template))
	{
	}

	void VulkanShaderObject::Bind(RenderEffect const& effect)
	{
		KFL_UNUSED(effect);
	}
	void VulkanShaderObject::Unbind()
	{
	}

	void VulkanShaderObject::CreateHwResources(ShaderStage stage, RenderEffect const& effect)
	{
		KFL_UNUSED(effect);
		KFL_UNUSED(stage);
	}

	static VkShaderStageFlagBits VulkanShaderStage(ShaderStage stage)
	{
		if (stage == ShaderStage::Vertex)
		{
			return VK_SHADER_STAGE_VERTEX_BIT;
		}
		else if (stage == ShaderStage::Pixel)
		{
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		return VK_SHADER_STAGE_VERTEX_BIT;
	}

	void VulkanShaderObject::DoLinkShaders(RenderEffect const& effect)
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		for (uint32_t stage = 0; stage < NumShaderStages; ++stage)
		{
			auto const* shader_stage = checked_cast<VulkanShaderStageObject*>(this->Stage(static_cast<ShaderStage>(stage)).get());
			if (shader_stage)
			{
				VkPipelineShaderStageCreateInfo info = {};
				info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				info.stage = VulkanShaderStage(static_cast<ShaderStage>(stage));
				info.module = shader_stage->GetVkShaderModule();
				info.pName = shader_stage->GetFunctionName();
				shaderStages.push_back(info);
			}
		}

		std::vector<uint32_t> all_cbuff_indices;
		for (uint32_t stage = 0; stage < NumShaderStages; ++stage)
		{
			auto const* shader_stage = checked_cast<VulkanShaderStageObject*>(this->Stage(static_cast<ShaderStage>(stage)).get());
			if (shader_stage)
			{
				if (!shader_stage->CBufferIndices().empty())
				{
					auto const& shader_desc = shader_stage->GetShaderDesc();
					auto const& cbuff_indices = shader_stage->CBufferIndices();

					all_cbuff_indices.insert(all_cbuff_indices.end(), cbuff_indices.begin(), cbuff_indices.end());
					for (size_t i = 0; i < cbuff_indices.size(); ++i)
					{
						auto cbuff = effect.CBufferByIndex(cbuff_indices[i]);
						cbuff->Resize(shader_desc.cb_desc[i].size);
						BOOST_ASSERT(cbuff->NumParameters() == shader_desc.cb_desc[i].var_desc.size());
						for (uint32_t j = 0; j < cbuff->NumParameters(); ++j)
						{
							RenderEffectParameter* param = effect.ParameterByIndex(cbuff->ParameterIndex(j));
							uint32_t stride;
							if (param->Type() == REDT_struct)
							{
								stride = 1;
							}
							else if (shader_desc.cb_desc[i].var_desc[j].elements > 0)
							{
								if (param->Type() != REDT_float4x4)
								{
									stride = 16;
								}
								else
								{
									stride = 64;
								}
							}
							else
							{
								if (param->Type() != REDT_float4x4)
								{
									stride = 4;
								}
								else
								{
									stride = 16;
								}
							}
							param->BindToCBuffer(effect, cbuff_indices[i], shader_desc.cb_desc[i].var_desc[j].start_offset, stride);
						}
					}
				}
			}
		}
	}
} // namespace KlayGE
