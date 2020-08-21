find_package(Vulkan REQUIRED FATAL_ERROR)
SET(LIB_NAME KlayGE_RenderEngine_Vulkan)

SET(VULKAN_RE_SOURCE_FILES
	${KLAYGE_PROJECT_DIR}/Plugins/Src/Render/Vulkan/VulkanRenderEngine.cpp
	${KLAYGE_PROJECT_DIR}/Plugins/Src/Render/Vulkan/VulkanRenderFactory.cpp
	${KLAYGE_PROJECT_DIR}/Plugins/Src/Render/Vulkan/VulkanRenderStateObject.cpp
	${KLAYGE_PROJECT_DIR}/Plugins/Src/Render/Vulkan/VulkanShaderObject.cpp
	${KLAYGE_PROJECT_DIR}/Plugins/Src/Render/Vulkan/VulkanTexture.cpp
	${KLAYGE_PROJECT_DIR}/Plugins/Src/Render/Vulkan/VulkanTexture2D.cpp
	${KLAYGE_PROJECT_DIR}/Plugins/Src/Render/Vulkan/VulkanTexture3D.cpp
	${KLAYGE_PROJECT_DIR}/Plugins/Src/Render/Vulkan/VulkanInstance.cpp
	${KLAYGE_PROJECT_DIR}/Plugins/Src/Render/Vulkan/VulkanDevice.cpp
	${KLAYGE_PROJECT_DIR}/Plugins/Src/Render/Vulkan/VulkanRenderWindow.cpp
	${KLAYGE_PROJECT_DIR}/Plugins/Src/Render/Vulkan/VulkanGraphicsBuffer.cpp
	${KLAYGE_PROJECT_DIR}/Plugins/Src/Render/Vulkan/VulkanRenderLayout.cpp
	${KLAYGE_PROJECT_DIR}/Plugins/Src/Render/Vulkan/VulkanFrameBuffer.cpp
	${KLAYGE_PROJECT_DIR}/Plugins/Src/Render/Vulkan/VulkanMapping.cpp
	${KLAYGE_PROJECT_DIR}/Plugins/Src/Render/Vulkan/VulkanRenderView.cpp
)

SET(VULKAN_RE_HEADER_FILES
	${KLAYGE_PROJECT_DIR}/Plugins/Include/KlayGE/Vulkan/VulkanTypedefs.hpp
	${KLAYGE_PROJECT_DIR}/Plugins/Include/KlayGE/Vulkan/VulkanInstance.hpp
	${KLAYGE_PROJECT_DIR}/Plugins/Include/KlayGE/Vulkan/VulkanDevice.hpp
	${KLAYGE_PROJECT_DIR}/Plugins/Include/KlayGE/Vulkan/VulkanRenderEngine.hpp
	${KLAYGE_PROJECT_DIR}/Plugins/Include/KlayGE/Vulkan/VulkanRenderFactory.hpp
	${KLAYGE_PROJECT_DIR}/Plugins/Include/KlayGE/Vulkan/VulkanRenderStateObject.hpp
	${KLAYGE_PROJECT_DIR}/Plugins/Include/KlayGE/Vulkan/VulkanShaderObject.hpp
	${KLAYGE_PROJECT_DIR}/Plugins/Include/KlayGE/Vulkan/VulkanTexture.hpp
	${KLAYGE_PROJECT_DIR}/Plugins/Include/KlayGE/Vulkan/VulkanRenderWindow.hpp
	${KLAYGE_PROJECT_DIR}/Plugins/Include/KlayGE/Vulkan/VulkanGraphicsBuffer.hpp
	${KLAYGE_PROJECT_DIR}/Plugins/Include/KlayGE/Vulkan/VulkanRenderLayout.hpp
	${KLAYGE_PROJECT_DIR}/Plugins/Include/KlayGE/Vulkan/VulkanFrameBuffer.hpp
	${KLAYGE_PROJECT_DIR}/Plugins/Include/KlayGE/Vulkan/VulkanMapping.hpp
	${KLAYGE_PROJECT_DIR}/Plugins/Include/KlayGE/Vulkan/VulkanRenderView.hpp
)

SOURCE_GROUP("Source Files" FILES ${VULKAN_RE_SOURCE_FILES})
SOURCE_GROUP("Header Files" FILES ${VULKAN_RE_HEADER_FILES})

ADD_LIBRARY(${LIB_NAME} SHARED
	${VULKAN_RE_SOURCE_FILES} ${VULKAN_RE_HEADER_FILES}
)

if(KLAYGE_PLATFORM_WINDOWS AND KLAYGE_COMPILER_GCC)
	# GCC warnings emitted by preprocessor can't be suppressed with pragma GCC diagnostic ignored
	target_compile_options(${LIB_NAME}
		PRIVATE
			-Wno-unknown-pragmas
	)
endif()

target_include_directories(${LIB_NAME}
	PRIVATE
		${KLAYGE_PROJECT_DIR}/Plugins/Include
		${Vulkan_INCLUDE_DIRS}
)


ADD_DEPENDENCIES(${LIB_NAME} ${KLAYGE_CORELIB_NAME})
if(KLAYGE_IS_DEV_PLATFORM)
	add_dependencies(${LIB_NAME} glloader DXBC2GLSLLib)
endif()

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${KLAYGE_OUTPUT_DIR}
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${KLAYGE_OUTPUT_DIR}
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${KLAYGE_OUTPUT_DIR}
	ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${KLAYGE_OUTPUT_DIR}
	ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${KLAYGE_OUTPUT_DIR}
	RUNTIME_OUTPUT_DIRECTORY ${KLAYGE_BIN_DIR}/Render
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${KLAYGE_BIN_DIR}/Render
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${KLAYGE_BIN_DIR}/Render
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${KLAYGE_BIN_DIR}/Render
	RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${KLAYGE_BIN_DIR}/Render
	LIBRARY_OUTPUT_DIRECTORY ${KLAYGE_BIN_DIR}/Render
	LIBRARY_OUTPUT_DIRECTORY_DEBUG ${KLAYGE_BIN_DIR}/Render
	LIBRARY_OUTPUT_DIRECTORY_RELEASE ${KLAYGE_BIN_DIR}/Render
	LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${KLAYGE_BIN_DIR}/Render
	LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${KLAYGE_BIN_DIR}/Render
	PROJECT_LABEL ${LIB_NAME}
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
	OUTPUT_NAME ${LIB_NAME}${KLAYGE_OUTPUT_SUFFIX}
	FOLDER "KlayGE/Engine/Plugins/Render"
)

KLAYGE_ADD_PRECOMPILED_HEADER(${LIB_NAME} "${KLAYGE_PROJECT_DIR}/Core/Include/KlayGE/KlayGE.hpp")

target_link_libraries(${LIB_NAME}
	PRIVATE
		${KLAYGE_CORELIB_NAME}
		glloader
		${Vulkan_LIBRARIES}
)
if(KLAYGE_IS_DEV_PLATFORM)
	target_link_libraries(${LIB_NAME}
		PRIVATE
			DXBC2GLSLLib
	)
endif()

ADD_DEPENDENCIES(AllInEngine ${LIB_NAME})
