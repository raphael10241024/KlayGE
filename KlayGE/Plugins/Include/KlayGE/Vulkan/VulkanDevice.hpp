/**
 * @file VulkanDevice.hpp
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

#ifndef KLAYGE_PLUGINS_VULKAN_DEVICE_HPP
#define KLAYGE_PLUGINS_VULKAN_DEVICE_HPP

#pragma once

#include <KlayGE/Vulkan/VulkanInstance.hpp>
#include <vector>
#include <string>

namespace KlayGE
{
	class VulkanDevice
	{
	public:
		VulkanDevice() = default;
		VulkanDevice(VulkanInstance *instance);
		//~VulkanDevice();

		//uint32_t GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound = nullptr);
		bool IsDeviceSuitable(const std::vector<const char*>&  requirededExtensions);

		uint32_t GetQueueFamilyIndex(VkQueueFlagBits queueFlags);

		void CreateLogicalDevice();

		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		bool IsFormatSupported(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features);

		VkCommandBuffer BeginSingleTimeCommand(VkCommandPool);

		void EndSingleTimeCommand(VkCommandPool, VkQueue queue, VkCommandBuffer buffer);

	public:
		VulkanInstance * instance;

		VkPhysicalDevice physicalDevice;

		VkPhysicalDeviceProperties properties;

		VkPhysicalDeviceFeatures features;

		VkPhysicalDeviceFeatures enabledFeatures;

		VkPhysicalDeviceMemoryProperties memoryProperties;

		std::vector<VkQueueFamilyProperties> queueFamilyProperties;

		std::vector<std::string> supportedExtensions;

		VkDevice logicalDevice;


		bool enableDebugMarkers = false;

		struct
		{
			uint32_t graphics;
			uint32_t compute;
			uint32_t transfer;
		} queueFamilyIndices;

		struct
		{
			VkQueue graphics;
			VkQueue compute;
			VkQueue transfer;
		} deviceQueues;
		
		struct
		{
			VkCommandPool graphics;
			VkCommandPool compute;
			VkCommandPool transfer;
		} commandPools;
		

		operator VkDevice() { return logicalDevice; };
	};
	typedef std::shared_ptr<VulkanDevice> VulkanDevicePtr;
} // namespace KlayGE

#endif // KLAYGE_PLUGINS_VULKAN_DEVICE_HPP
