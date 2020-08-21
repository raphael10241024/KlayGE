////////////////////////////////////////////////////////////////
#ifndef KLAYGE_PLUGINS_VULKAN_TYPE_DEFINES_HPP
#define KLAYGE_PLUGINS_VULKAN_TYPE_DEFINES_HPP

#pragma once

#pragma warning(disable : 26812)
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>


namespace KlayGE
{
#define VK_CHECK_RESULT(f)																				\
{																										\
	VkResult res = (f);																					\
	if (res != VK_SUCCESS)																				\
	{																									\
		TMSG(KlayGE::CombineFileLine(__FILE__, __LINE__));												\
	}																									\
}   

} // namespace KlayGE

#endif // KLAYGE_PLUGINS_VULKAN_TYPE_DEFINES_HPP
