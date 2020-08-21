/**
 * @file VulkanShaderObject.hpp
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

#ifndef KLAYGE_PLUGINS_VULKAN_SHADER_OBJECT_HPP
#define KLAYGE_PLUGINS_VULKAN_SHADER_OBJECT_HPP

#pragma once

#include <KlayGE/PreDeclare.hpp>
#include <KlayGE/RenderLayout.hpp>
#include <KlayGE/ShaderObject.hpp>

namespace KlayGE
{
	class VulkanShaderStageObject : public ShaderStageObject
	{
	public:
		explicit VulkanShaderStageObject(ShaderStage stage);
		~VulkanShaderStageObject() override;

		void StreamIn(
			RenderEffect const& effect, std::array<uint32_t, NumShaderStages> const& shader_desc_ids, ResIdentifier& res) override;
		void StreamOut(std::ostream& os) override;
		void CompileShader(RenderEffect const& effect, RenderTechnique const& tech, RenderPass const& pass,
			std::array<uint32_t, NumShaderStages> const& shader_desc_ids) override;
		void CreateHwShader(RenderEffect const& effect, std::array<uint32_t, NumShaderStages> const& shader_desc_ids) override;

		void FillCBufferIndices(RenderEffect const& effect);

		VkShaderModule GetVkShaderModule() const
		{
			return shaderModule;
		}
		const char* GetFunctionName() const
		{
			return shader_func_name_.c_str();
		}

		SPIRVShaderDesc const& GetShaderDesc() const
		{
			return shader_desc_;
		}

		std::vector<uint8_t> const& CBufferIndices() const
		{
			return cbuff_indices_;
		}

	private:
		std::string_view GetShaderProfile(RenderEffect const& effect, uint32_t shader_desc_id) const override;

	private:
		bool is_available_;

		std::vector<uint32_t> spirv_code_; 
		VkShaderModule shaderModule;

		std::string shader_func_name_;
		SPIRVShaderDesc shader_desc_;
		std::vector<uint8_t> cbuff_indices_;

		std::vector<std::string> pnames_;
		std::vector<std::string> glsl_res_names_;

		std::vector<std::pair<std::string, std::string>> tex_sampler_pairs_;
	};

	class VulkanVertexShaderStageObject final : public VulkanShaderStageObject
	{
	public:
		VulkanVertexShaderStageObject();
	};

	class VulkanPixelShaderStageObject final : public VulkanShaderStageObject
	{
	public:
		VulkanPixelShaderStageObject();
	};

	class VulkanGeometryShaderStageObject final : public VulkanShaderStageObject
	{
	public:
		VulkanGeometryShaderStageObject();
	};

	class VulkanComputeShaderStageObject final : public VulkanShaderStageObject
	{
	public:
		VulkanComputeShaderStageObject();
	};

	class VulkanHullShaderStageObject final : public VulkanShaderStageObject
	{
	public:
		VulkanHullShaderStageObject();
	};

	class VulkanDomainShaderStageObject final : public VulkanShaderStageObject
	{
	public:
		VulkanDomainShaderStageObject();
	};

	class VulkanShaderObject final : public ShaderObject
	{
	private:
		struct VulkanShaderTemplate
		{
			int id;
		};
		const std::shared_ptr<VulkanShaderTemplate> vulkan_so_template_;
	public:
		VulkanShaderObject();
		explicit VulkanShaderObject(std::shared_ptr<ShaderObjectTemplate> so_template);
		~VulkanShaderObject() override;
		ShaderObjectPtr Clone(RenderEffect const& effect) override;
		void Bind(RenderEffect const& effect) override;
		void Unbind() override;

	private:
		void CreateHwResources(ShaderStage stage, RenderEffect const& effect) override;
		void DoLinkShaders(RenderEffect const& effect) override;
	};

	typedef std::shared_ptr<VulkanShaderObject> VulkanShaderObjectPtr;
} // namespace KlayGE

#endif // KLAYGE_PLUGINS_Vulkan_SHADER_OBJECT_HPP
