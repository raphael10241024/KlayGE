/**
 * @file VulkanGraphicsBuffer.cpp
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
#include <KFL/Log.hpp>
#include <KlayGE/KlayGE.hpp>
#include <KlayGE/Vulkan/VulkanGraphicsBuffer.hpp>
#include <fstream>
#include <system_error>



namespace KlayGE
{
	VulkanGraphicsBuffer& VulkanGraphicsBuffer::GetStagingBuffer(VulkanDevicePtr d)
	{
		// 128M»º³å
		static VulkanGraphicsBuffer stagingBuffer(d, BU_Dynamic, EAH_CPU_Read | EAH_CPU_Write | EAH_GPU_Read, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		128 * 1024 * 1024, 0);
		if (!stagingBuffer.ready_) {
			stagingBuffer.CreateHWResource(nullptr);
		}
		return stagingBuffer;
	}

	VulkanGraphicsBuffer::VulkanGraphicsBuffer(VulkanDevicePtr device, BufferUsage usage, uint32_t access_hint, VkBufferUsageFlags type,
		uint32_t size_in_byte, uint32_t structure_byte_stride)
		: GraphicsBuffer(usage, access_hint, size_in_byte, structure_byte_stride), device_(device), type_(type), ready_(false)
	{

	}

	VulkanGraphicsBuffer::~VulkanGraphicsBuffer()
	{
		this->DeleteHWResource();
	}

	void VulkanGraphicsBuffer::CreateHWResource(void const* init_data)
	{
		bool deviceOnly = usage_ == BU_Static;
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = this->size_in_byte_;
		bufferInfo.usage = this->type_;
		if (deviceOnly)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VK_CHECK_RESULT(vkCreateBuffer(this->device_->logicalDevice, &bufferInfo, nullptr, &buffer_));

		VkMemoryRequirements memeRequirements;
		vkGetBufferMemoryRequirements(this->device_->logicalDevice, buffer_, &memeRequirements);
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memeRequirements.size;

		VkMemoryPropertyFlagBits flags;
		if (deviceOnly)
		{
			flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}
		else
		{
			flags = VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
		allocInfo.memoryTypeIndex = device_->FindMemoryType(
			memeRequirements.memoryTypeBits, flags);
		VK_CHECK_RESULT(vkAllocateMemory(device_->logicalDevice, &allocInfo, nullptr, &bufferMemory_));
		vkBindBufferMemory(device_->logicalDevice, buffer_, bufferMemory_, 0);

		if (init_data != nullptr)
		{
			if (deviceOnly)
			{
				auto& stagingBuffer = GetStagingBuffer(device_);
				void* stagingData;
				vkMapMemory(device_->logicalDevice, stagingBuffer.bufferMemory_, 0, size_in_byte_, 0, &stagingData);
				memcpy(stagingData, init_data, (size_t)size_in_byte_);
				vkUnmapMemory(device_->logicalDevice, stagingBuffer.bufferMemory_);
				stagingBuffer.CopyToSubBuffer(*this, 0, 0, size_in_byte_);
			}
			else
			{
				void* data;
				vkMapMemory(device_->logicalDevice, bufferMemory_, 0, size_in_byte_, 0, &data);
				memcpy(data, init_data, (size_t)size_in_byte_);
				vkUnmapMemory(device_->logicalDevice, bufferMemory_);
			}	
		}
		ready_ = true;
	}

	bool VulkanGraphicsBuffer::HWResourceReady() const
	{
		return ready_;
	}
	void VulkanGraphicsBuffer::DeleteHWResource()
	{
		vkDestroyBuffer(device_->logicalDevice, buffer_, nullptr);
		vkFreeMemory(device_->logicalDevice, bufferMemory_, nullptr);
		ready_ = false;
	}

    void* VulkanGraphicsBuffer::Map(BufferAccess ba)
    {
		KFL_UNUSED(ba);
		void* p;
		vkMapMemory(device_->logicalDevice, bufferMemory_, 0, size_in_byte_, 0, &p);
		return p;
    }
    void VulkanGraphicsBuffer::Unmap()
    {
		vkUnmapMemory(device_->logicalDevice, bufferMemory_);
    }
    void VulkanGraphicsBuffer::CopyToBuffer(GraphicsBuffer& target)
    {
		CopyToSubBuffer(target, 0, 0, size_in_byte_);
    }
    void VulkanGraphicsBuffer::CopyToSubBuffer(GraphicsBuffer& target, uint32_t dst_offset, uint32_t src_offset, uint32_t size)
    {
		VulkanGraphicsBuffer& tgb = checked_cast<VulkanGraphicsBuffer&>(target);

		VkCommandBufferAllocateInfo cbainfo ={};
		cbainfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cbainfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cbainfo.commandPool = device_->commandPools.transfer;
		cbainfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device_->logicalDevice, &cbainfo, &commandBuffer);
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = src_offset;
		copyRegion.dstOffset = dst_offset;
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, buffer_, tgb.buffer_, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);
		
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer	;

		vkQueueSubmit(device_->deviceQueues.transfer, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(device_->deviceQueues.transfer);
    }

    void VulkanGraphicsBuffer::UpdateSubresource(uint32_t offset, uint32_t size, void const* data)
    {
        KFL_UNUSED(offset);
        KFL_UNUSED(data);
        KFL_UNUSED(size);
    }
} // namespace KlayGE
