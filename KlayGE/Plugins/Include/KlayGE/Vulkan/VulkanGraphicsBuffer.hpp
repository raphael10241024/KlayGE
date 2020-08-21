//////////////////////////////////////////////////////////////////////////////////

#ifndef KLAYGE_PLUGINS_VULKAN_GRAPHICS_BUFFER_HPP
#define KLAYGE_PLUGINS_VULKAN_GRAPHICS_BUFFER_HPP

#pragma once

#include <KlayGE/ElementFormat.hpp>
#include <KlayGE/GraphicsBuffer.hpp>
#include <KlayGE/Vulkan/VulkanDevice.hpp>
#include <KlayGE/Vulkan/VulkanTexture.hpp>
#include <unordered_map>

namespace KlayGE
{
	class VulkanGraphicsBuffer final : public GraphicsBuffer
	{
	public:
		explicit VulkanGraphicsBuffer(VulkanDevicePtr device, BufferUsage usage, uint32_t access_hint, VkBufferUsageFlags type,
			uint32_t size_in_byte, uint32_t structure_byte_stride);
		~VulkanGraphicsBuffer() override;

		void CreateHWResource(void const* init_data) override;
		bool HWResourceReady() const override;
		void DeleteHWResource() override;
		void* Map(BufferAccess ba) override;
		void Unmap() override;
		void CopyToBuffer(GraphicsBuffer& target) override;
		void CopyToSubBuffer(GraphicsBuffer& target, uint32_t dst_offset, uint32_t src_offset, uint32_t size) override;
		void UpdateSubresource(uint32_t offset, uint32_t size, void const* data) override;

		inline VkBuffer GetVkBuffer() const {return buffer_;}
		inline VkDeviceMemory GetVkDeviceMemory() const {return bufferMemory_;}
		static VulkanGraphicsBuffer& GetStagingBuffer(VulkanDevicePtr d);

	private:

		VulkanDevicePtr device_;
		VkBuffer buffer_;
		VkBufferUsageFlags type_;
		VkDeviceMemory bufferMemory_;
		bool ready_;
		void * mapped_ = nullptr;
	};

} // namespace KlayGE

#endif // KLAYGE_PLUGINS_VULKAN_GRAPHICS_BUFFER_HPP
