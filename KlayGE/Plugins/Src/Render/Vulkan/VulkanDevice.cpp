/**
 * @file VulkanTexture.hpp
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
#include <KlayGE/Vulkan/VulkanDevice.hpp>
#include <set>
#include <system_error>

namespace KlayGE
{

	VulkanDevice::VulkanDevice(VulkanInstance* ins) : instance(ins)
	{
		physicalDevice = instance->GetFirstPhysicalDevice();
		assert(physicalDevice);

		//检查PhysicalDevice的属性并记录
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);
		vkGetPhysicalDeviceFeatures(physicalDevice, &features);
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		assert(queueFamilyCount > 0);
		queueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

		// Get list of supported extensions
		uint32_t extCount = 0;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr);
		if (extCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extCount);
			if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
			{
				for (auto ext : extensions)
				{
					supportedExtensions.push_back(ext.extensionName);
				}
			}
		}
	}

	bool VulkanDevice::IsDeviceSuitable(const std::vector<const char*>& requirededExtensions)
	{
		std::set<std::string> re(requirededExtensions.begin(), requirededExtensions.end());
		for (const auto& extension : supportedExtensions)
		{
			re.erase(extension);
		}
		return re.empty();
	}

	uint32_t VulkanDevice::GetQueueFamilyIndex(VkQueueFlagBits queueFlags)
	{
		if (queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
			{
				if ((queueFamilyProperties[i].queueFlags & queueFlags) &&
					((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
				{
					return i;
					break;
				}
			}
		}
		// TODO ADD vkGetPhysicalDeviceSurfaceSupportKHR

		if (queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
			{
				if ((queueFamilyProperties[i].queueFlags & queueFlags) &&
					((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
					((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
				{
					return i;
					break;
				}
			}
		}

		for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
		{
			if (queueFamilyProperties[i].queueFlags & queueFlags)
			{
				return i;
				break;
			}
		}

		TMSG("Could not find a matching queue family index");
	}

	void VulkanDevice::CreateLogicalDevice()
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
		const float defaultQueuePriority(0.0f);

		queueFamilyIndices.graphics = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
		{
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = queueFamilyIndices.graphics;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.push_back(queueInfo);
		}

		queueFamilyIndices.compute = GetQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
		if (queueFamilyIndices.compute != queueFamilyIndices.graphics)
		{
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = queueFamilyIndices.compute;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.push_back(queueInfo);
		}
		else
		{
			queueFamilyIndices.compute = queueFamilyIndices.graphics;
		}

		queueFamilyIndices.transfer = GetQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);
		if ((queueFamilyIndices.transfer != queueFamilyIndices.graphics) && (queueFamilyIndices.transfer != queueFamilyIndices.compute))
		{
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = queueFamilyIndices.transfer;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			queueCreateInfos.push_back(queueInfo);
		}
		else
		{
			queueFamilyIndices.transfer = queueFamilyIndices.graphics;
		}

		VkPhysicalDeviceFeatures deviceFratures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<int32_t>(queueCreateInfos.size());
		createInfo.pEnabledFeatures = &deviceFratures;


		std::vector<const char*> requireedExtesnsion = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		if (!IsDeviceSuitable(requireedExtesnsion))
		{
			TMSG("Not support!");
		}
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requireedExtesnsion.size());
		createInfo.ppEnabledExtensionNames = requireedExtesnsion.data();


		const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
		createInfo.ppEnabledLayerNames = &validationLayerName;
		createInfo.enabledLayerCount = 1;


		VK_CHECK_RESULT(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice));


		vkGetDeviceQueue(logicalDevice, queueFamilyIndices.graphics, 0, &deviceQueues.graphics);
		deviceQueues.transfer = deviceQueues.compute = deviceQueues.graphics;
		if (queueFamilyIndices.compute != queueFamilyIndices.graphics)
		{
			vkGetDeviceQueue(logicalDevice, queueFamilyIndices.compute, 0, &deviceQueues.compute);
		}
		if ((queueFamilyIndices.transfer != queueFamilyIndices.graphics) && (queueFamilyIndices.transfer != queueFamilyIndices.compute))
		{
			vkGetDeviceQueue(logicalDevice, queueFamilyIndices.transfer, 0, &deviceQueues.transfer);
		}

		{
			VkCommandPoolCreateInfo cmdPoolInfo = {};
			cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmdPoolInfo.queueFamilyIndex = queueFamilyIndices.graphics;
			cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			VK_CHECK_RESULT(vkCreateCommandPool(logicalDevice, &cmdPoolInfo, nullptr, &commandPools.graphics));
		}
		{
			VkCommandPoolCreateInfo cmdPoolInfo = {};
			cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmdPoolInfo.queueFamilyIndex = queueFamilyIndices.compute;
			cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			VK_CHECK_RESULT(vkCreateCommandPool(logicalDevice, &cmdPoolInfo, nullptr, &commandPools.compute));
		}
		{
			VkCommandPoolCreateInfo cmdPoolInfo = {};
			cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmdPoolInfo.queueFamilyIndex = queueFamilyIndices.transfer;
			cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			VK_CHECK_RESULT(vkCreateCommandPool(logicalDevice, &cmdPoolInfo, nullptr, &commandPools.transfer));
		}
	}

	uint32_t VulkanDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags porp)
	{
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & porp) == porp)
			{
				return i;
			}
		}
		TMSG("Error");
	}

	bool VulkanDevice::IsFormatSupported(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags f)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(this->physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & f) == f)
		{
			return true;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & f) == f)
		{
			return true;
		}
		return false;
	}

	
	VkCommandBuffer VulkanDevice::BeginSingleTimeCommand(VkCommandPool commandPool)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;
        
        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(this->logicalDevice, &allocInfo, &commandBuffer);
        
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        
        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        
        return commandBuffer;
	}

	void VulkanDevice::EndSingleTimeCommand(VkCommandPool commandPool, VkQueue queue, VkCommandBuffer buffer)
	{
		vkEndCommandBuffer(buffer);
        
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &buffer;
        
        vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue);
        
        vkFreeCommandBuffers(this->logicalDevice, commandPool, 1, &buffer);
	}

	// VulkanDevice::~VulkanDevice()
	//{
	//    if (commandPool)
	//    {
	//        vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
	//    }
	//    if (logicalDevice)
	//    {
	//        vkDestroyDevice(logicalDevice, nullptr);
	//    }
	//}
} // namespace KlayGE
