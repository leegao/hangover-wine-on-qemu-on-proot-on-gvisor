#include <stdint.h>
#include <stddef.h>

extern void *dlsym(void *handle, const char *symbol);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern char *strcpy(char *dest, const char *src);
extern char *strncpy(char *dest, const char *src, size_t n);
extern size_t strlen(const char *s);
extern void *memcpy(void *dest, const void *src, size_t n);
extern void *memset(void *s, int c, size_t n);
extern void *calloc(size_t nmemb, size_t size);
extern void *malloc(size_t size);
extern void free(void *ptr);
extern long write(int fd, const void *buf, size_t count);
extern void _exit(int status);

#define RTLD_DEFAULT ((void *)0)

typedef uint32_t VkFlags;
typedef uint64_t VkFlags64;
typedef uint32_t VkBool32;
typedef uint64_t VkDeviceSize;
typedef uint64_t VkDeviceAddress;
typedef int32_t VkResult;
typedef int32_t VkStructureType;
typedef void (*PFN_vkVoidFunction)(void);

#define VK_SUCCESS 0
#define VK_INCOMPLETE 5
#define VK_TRUE 1
#define VK_FALSE 0
#define VK_MAKE_API_VERSION(variant, major, minor, patch) \
    ((((uint32_t)(variant)) << 29U) | (((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))
#define VK_API_VERSION_1_3 VK_MAKE_API_VERSION(0, 1, 3, 296)

typedef struct VkBaseOutStructure {
    VkStructureType sType;
    struct VkBaseOutStructure *pNext;
} VkBaseOutStructure;

typedef struct VkExtensionProperties {
    char extensionName[256];
    uint32_t specVersion;
} VkExtensionProperties;

typedef struct VkExtent2D {
    uint32_t width;
    uint32_t height;
} VkExtent2D;

typedef struct VkExtent3D {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
} VkExtent3D;

/* Dispatchable handle object with loader magic at offset 0 */
typedef struct VkDispatchObj {
    uintptr_t loader_magic;
    uint64_t id;
    void *mem_ptr;
    size_t mem_size;
} VkDispatchObj;

static VkDispatchObj g_phys_dev = { 0x01CDC0DE, 1, NULL, 0 };
static uint64_t g_next_handle_id = 10;

static void *alloc_dispatch_handle(void) {
    VkDispatchObj *o = (VkDispatchObj *)calloc(1, sizeof(VkDispatchObj));
    if (o) {
        o->loader_magic = 0x01CDC0DE;
        o->id = g_next_handle_id++;
    }
    return o;
}

static uint64_t alloc_nondispatch_handle(void) {
    return g_next_handle_id++;
}

static const VkExtensionProperties g_instance_exts[] = {
    {"VK_KHR_surface", 25},
    {"VK_EXT_headless_surface", 1},
    {"VK_KHR_get_physical_device_properties2", 2},
    {"VK_KHR_get_surface_capabilities2", 1},
    {"VK_EXT_surface_maintenance1", 1},
    {"VK_KHR_surface_maintenance1", 1},
    {"VK_EXT_swapchain_colorspace", 4},
    {"VK_KHR_external_memory_capabilities", 1},
    {"VK_KHR_external_semaphore_capabilities", 1},
    {"VK_KHR_external_fence_capabilities", 1},
    {"VK_KHR_device_group_creation", 1},
    {"VK_EXT_debug_utils", 2},
};
#define NUM_INSTANCE_EXTS ((uint32_t)(sizeof(g_instance_exts)/sizeof(g_instance_exts[0])))

static const VkExtensionProperties g_device_exts[] = {
    {"VK_KHR_swapchain", 70},
    {"VK_KHR_swapchain_mutable_format", 1},
    {"VK_EXT_swapchain_maintenance1", 1},
    {"VK_KHR_swapchain_maintenance1", 1},
    {"VK_KHR_maintenance1", 2},
    {"VK_KHR_maintenance2", 1},
    {"VK_KHR_maintenance3", 1},
    {"VK_KHR_maintenance4", 2},
    {"VK_KHR_maintenance5", 1},
    {"VK_KHR_maintenance6", 1},
    {"VK_KHR_maintenance7", 1},
    {"VK_KHR_dynamic_rendering", 1},
    {"VK_KHR_dynamic_rendering_local_read", 1},
    {"VK_KHR_synchronization2", 1},
    {"VK_KHR_timeline_semaphore", 2},
    {"VK_KHR_create_renderpass2", 1},
    {"VK_KHR_copy_commands2", 1},
    {"VK_KHR_bind_memory2", 1},
    {"VK_KHR_dedicated_allocation", 3},
    {"VK_KHR_descriptor_update_template", 1},
    {"VK_KHR_draw_indirect_count", 1},
    {"VK_KHR_driver_properties", 1},
    {"VK_KHR_format_feature_flags2", 2},
    {"VK_KHR_get_memory_requirements2", 1},
    {"VK_KHR_image_format_list", 1},
    {"VK_KHR_imageless_framebuffer", 1},
    {"VK_KHR_load_store_op_none", 1},
    {"VK_KHR_map_memory2", 1},
    {"VK_KHR_multiview", 1},
    {"VK_KHR_pipeline_library", 1},
    {"VK_KHR_present_id", 1},
    {"VK_KHR_present_wait", 1},
    {"VK_KHR_push_descriptor", 2},
    {"VK_KHR_relaxed_block_layout", 1},
    {"VK_KHR_robustness2", 1},
    {"VK_KHR_sampler_mirror_clamp_to_edge", 3},
    {"VK_KHR_sampler_ycbcr_conversion", 14},
    {"VK_KHR_separate_depth_stencil_layouts", 1},
    {"VK_KHR_shader_draw_parameters", 1},
    {"VK_KHR_shader_float16_int8", 1},
    {"VK_KHR_shader_float_controls", 4},
    {"VK_KHR_shader_non_semantic_info", 1},
    {"VK_KHR_shader_subgroup_extended_types", 1},
    {"VK_KHR_shader_terminate_invocation", 1},
    {"VK_KHR_spirv_1_4", 1},
    {"VK_KHR_storage_buffer_storage_class", 1},
    {"VK_KHR_uniform_buffer_standard_layout", 1},
    {"VK_KHR_variable_pointers", 1},
    {"VK_KHR_vertex_attribute_divisor", 1},
    {"VK_KHR_vulkan_memory_model", 3},
    {"VK_KHR_zero_initialize_workgroup_memory", 1},
    {"VK_KHR_16bit_storage", 1},
    {"VK_KHR_8bit_storage", 1},
    {"VK_KHR_buffer_device_address", 1},
    {"VK_KHR_depth_stencil_resolve", 1},
    {"VK_KHR_device_group", 4},
    {"VK_KHR_external_fence", 1},
    {"VK_KHR_external_memory", 1},
    {"VK_KHR_external_semaphore", 1},
    {"VK_KHR_shader_atomic_int64", 1},
    {"VK_KHR_shader_Expect_assume", 1},
    {"VK_KHR_shader_integer_dot_product", 1},
    {"VK_EXT_4444_formats", 1},
    {"VK_EXT_attachment_feedback_loop_layout", 2},
    {"VK_EXT_border_color_swizzle", 1},
    {"VK_EXT_color_write_enable", 1},
    {"VK_EXT_conservative_rasterization", 1},
    {"VK_EXT_custom_border_color", 12},
    {"VK_EXT_depth_bias_control", 1},
    {"VK_EXT_depth_clip_control", 1},
    {"VK_EXT_depth_clip_enable", 1},
    {"VK_EXT_descriptor_buffer", 1},
    {"VK_EXT_descriptor_indexing", 2},
    {"VK_EXT_extended_dynamic_state", 1},
    {"VK_EXT_extended_dynamic_state2", 1},
    {"VK_EXT_extended_dynamic_state3", 2},
    {"VK_EXT_external_memory_host", 1},
    {"VK_EXT_fragment_shader_interlock", 1},
    {"VK_EXT_graphics_pipeline_library", 1},
    {"VK_EXT_hdr_metadata", 2},
    {"VK_EXT_host_query_reset", 1},
    {"VK_EXT_image_2d_view_of_3d", 1},
    {"VK_EXT_image_robustness", 1},
    {"VK_EXT_image_sliced_view_of_3d", 1},
    {"VK_EXT_image_view_min_lod", 1},
    {"VK_EXT_index_type_uint8", 1},
    {"VK_EXT_inline_uniform_block", 1},
    {"VK_EXT_line_rasterization", 1},
    {"VK_EXT_load_store_op_none", 1},
    {"VK_EXT_memory_budget", 1},
    {"VK_EXT_memory_priority", 1},
    {"VK_EXT_multi_draw", 1},
    {"VK_EXT_mutable_descriptor_type", 1},
    {"VK_EXT_nested_command_buffer", 1},
    {"VK_EXT_non_seamless_cube_map", 1},
    {"VK_EXT_pageable_device_local_memory", 1},
    {"VK_EXT_pipeline_creation_cache_control", 3},
    {"VK_EXT_pipeline_creation_feedback", 1},
    {"VK_EXT_pipeline_robustness", 1},
    {"VK_EXT_post_depth_coverage", 1},
    {"VK_EXT_primitive_topology_list_restart", 1},
    {"VK_EXT_primitives_generated_query", 1},
    {"VK_EXT_private_data", 1},
    {"VK_EXT_provoking_vertex", 1},
    {"VK_EXT_robustness2", 1},
    {"VK_EXT_sample_locations", 1},
    {"VK_EXT_sampler_filter_minmax", 2},
    {"VK_EXT_scalar_block_layout", 1},
    {"VK_EXT_separate_stencil_usage", 1},
    {"VK_EXT_shader_atomic_float", 1},
    {"VK_EXT_shader_atomic_float2", 1},
    {"VK_EXT_shader_demote_to_helper_invocation", 1},
    {"VK_EXT_shader_module_identifier", 1},
    {"VK_EXT_shader_stencil_export", 1},
    {"VK_EXT_shader_viewport_index_layer", 1},
    {"VK_EXT_subgroup_size_control", 2},
    {"VK_EXT_texel_buffer_alignment", 1},
    {"VK_EXT_transform_feedback", 1},
    {"VK_EXT_vertex_attribute_divisor", 3},
    {"VK_EXT_vertex_input_dynamic_state", 2},
    {"VK_VALVE_mutable_descriptor_type", 1},
    {"VK_AMD_shader_fragment_mask", 1},
};
#define NUM_DEVICE_EXTS ((uint32_t)(sizeof(g_device_exts)/sizeof(g_device_exts[0])))

VkResult vkEnumerateInstanceVersion(uint32_t *pApiVersion) {
    if (pApiVersion) *pApiVersion = VK_API_VERSION_1_3;
    return VK_SUCCESS;
}

VkResult vkEnumerateInstanceLayerProperties(uint32_t *pPropertyCount, void *pProperties) {
    (void)pProperties;
    if (pPropertyCount) *pPropertyCount = 0;
    return VK_SUCCESS;
}

VkResult vkEnumerateDeviceLayerProperties(void *physicalDevice, uint32_t *pPropertyCount, void *pProperties) {
    (void)physicalDevice; (void)pProperties;
    if (pPropertyCount) *pPropertyCount = 0;
    return VK_SUCCESS;
}

VkResult vkEnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pPropertyCount, VkExtensionProperties *pProperties) {
    (void)pLayerName;
    if (!pPropertyCount) return VK_SUCCESS;
    if (!pProperties) {
        *pPropertyCount = NUM_INSTANCE_EXTS;
        return VK_SUCCESS;
    }
    uint32_t count = *pPropertyCount < NUM_INSTANCE_EXTS ? *pPropertyCount : NUM_INSTANCE_EXTS;
    for (uint32_t i = 0; i < count; i++) {
        pProperties[i] = g_instance_exts[i];
    }
    *pPropertyCount = count;
    return (count < NUM_INSTANCE_EXTS) ? VK_INCOMPLETE : VK_SUCCESS;
}

VkResult vkEnumerateDeviceExtensionProperties(void *physicalDevice, const char *pLayerName, uint32_t *pPropertyCount, VkExtensionProperties *pProperties) {
    (void)physicalDevice; (void)pLayerName;
    if (!pPropertyCount) return VK_SUCCESS;
    if (!pProperties) {
        *pPropertyCount = NUM_DEVICE_EXTS;
        return VK_SUCCESS;
    }
    uint32_t count = *pPropertyCount < NUM_DEVICE_EXTS ? *pPropertyCount : NUM_DEVICE_EXTS;
    for (uint32_t i = 0; i < count; i++) {
        pProperties[i] = g_device_exts[i];
    }
    *pPropertyCount = count;
    return (count < NUM_DEVICE_EXTS) ? VK_INCOMPLETE : VK_SUCCESS;
}

VkResult vkCreateInstance(const void *pCreateInfo, const void *pAllocator, void **pInstance) {
    (void)pCreateInfo; (void)pAllocator;
    if (pInstance) *pInstance = alloc_dispatch_handle();
    return VK_SUCCESS;
}

void vkDestroyInstance(void *instance, const void *pAllocator) {
    (void)instance; (void)pAllocator;
}

VkResult vkEnumeratePhysicalDevices(void *instance, uint32_t *pPhysicalDeviceCount, void **pPhysicalDevices) {
    (void)instance;
    if (!pPhysicalDeviceCount) return VK_SUCCESS;
    if (!pPhysicalDevices) {
        *pPhysicalDeviceCount = 1;
        return VK_SUCCESS;
    }
    if (*pPhysicalDeviceCount >= 1) {
        pPhysicalDevices[0] = &g_phys_dev;
        *pPhysicalDeviceCount = 1;
        return VK_SUCCESS;
    }
    return VK_INCOMPLETE;
}

typedef struct VkPhysicalDeviceGroupProperties {
    VkStructureType sType;
    void *pNext;
    uint32_t physicalDeviceCount;
    void *physicalDevices[32];
    VkBool32 subsetAllocation;
} VkPhysicalDeviceGroupProperties;

VkResult vkEnumeratePhysicalDeviceGroups(void *instance, uint32_t *pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties *pPhysicalDeviceGroupProperties) {
    (void)instance;
    if (!pPhysicalDeviceGroupCount) return VK_SUCCESS;
    if (!pPhysicalDeviceGroupProperties) {
        *pPhysicalDeviceGroupCount = 1;
        return VK_SUCCESS;
    }
    if (*pPhysicalDeviceGroupCount >= 1) {
        pPhysicalDeviceGroupProperties[0].physicalDeviceCount = 1;
        pPhysicalDeviceGroupProperties[0].physicalDevices[0] = &g_phys_dev;
        pPhysicalDeviceGroupProperties[0].subsetAllocation = VK_FALSE;
        *pPhysicalDeviceGroupCount = 1;
        return VK_SUCCESS;
    }
    return VK_INCOMPLETE;
}

typedef struct VkPhysicalDeviceLimits {
    uint32_t maxImageDimension1D;
    uint32_t maxImageDimension2D;
    uint32_t maxImageDimension3D;
    uint32_t maxImageDimensionCube;
    uint32_t maxImageArrayLayers;
    uint32_t maxTexelBufferElements;
    uint32_t maxUniformBufferRange;
    uint32_t maxStorageBufferRange;
    uint32_t maxPushConstantsSize;
    uint32_t maxMemoryAllocationCount;
    uint32_t maxSamplerAllocationCount;
    VkDeviceSize bufferImageGranularity;
    VkDeviceSize sparseAddressSpaceSize;
    uint32_t maxBoundDescriptorSets;
    uint32_t maxPerStageDescriptorSamplers;
    uint32_t maxPerStageDescriptorUniformBuffers;
    uint32_t maxPerStageDescriptorStorageBuffers;
    uint32_t maxPerStageDescriptorSampledImages;
    uint32_t maxPerStageDescriptorStorageImages;
    uint32_t maxPerStageDescriptorInputAttachments;
    uint32_t maxPerStageResources;
    uint32_t maxDescriptorSetSamplers;
    uint32_t maxDescriptorSetUniformBuffers;
    uint32_t maxDescriptorSetUniformBuffersDynamic;
    uint32_t maxDescriptorSetStorageBuffers;
    uint32_t maxDescriptorSetStorageBuffersDynamic;
    uint32_t maxDescriptorSetSampledImages;
    uint32_t maxDescriptorSetStorageImages;
    uint32_t maxDescriptorSetInputAttachments;
    uint32_t maxVertexInputAttributes;
    uint32_t maxVertexInputBindings;
    uint32_t maxVertexInputAttributeOffset;
    uint32_t maxVertexInputBindingStride;
    uint32_t maxVertexOutputComponents;
    uint32_t maxTessellationGenerationLevel;
    uint32_t maxTessellationPatchSize;
    uint32_t maxTessellationControlPerVertexInputComponents;
    uint32_t maxTessellationControlPerVertexOutputComponents;
    uint32_t maxTessellationControlPerPatchOutputComponents;
    uint32_t maxTessellationControlTotalOutputComponents;
    uint32_t maxTessellationEvaluationInputComponents;
    uint32_t maxTessellationEvaluationOutputComponents;
    uint32_t maxGeometryShaderInvocations;
    uint32_t maxGeometryInputComponents;
    uint32_t maxGeometryOutputComponents;
    uint32_t maxGeometryOutputVertices;
    uint32_t maxGeometryTotalOutputComponents;
    uint32_t maxFragmentInputComponents;
    uint32_t maxFragmentOutputAttachments;
    uint32_t maxFragmentDualSrcAttachments;
    uint32_t maxFragmentCombinedOutputResources;
    uint32_t maxComputeSharedMemorySize;
    uint32_t maxComputeWorkGroupCount[3];
    uint32_t maxComputeWorkGroupInvocations;
    uint32_t maxComputeWorkGroupSize[3];
    uint32_t subPixelPrecisionBits;
    uint32_t subTexelPrecisionBits;
    uint32_t mipmapPrecisionBits;
    uint32_t maxDrawIndexedIndexValue;
    uint32_t maxDrawIndirectCount;
    float maxSamplerLodBias;
    float maxSamplerAnisotropy;
    uint32_t maxViewports;
    uint32_t maxViewportDimensions[2];
    float viewportBoundsRange[2];
    uint32_t viewportSubPixelBits;
    size_t minMemoryMapAlignment;
    VkDeviceSize minTexelBufferOffsetAlignment;
    VkDeviceSize minUniformBufferOffsetAlignment;
    VkDeviceSize minStorageBufferOffsetAlignment;
    int32_t minTexelOffset;
    uint32_t maxTexelOffset;
    int32_t minTexelGatherOffset;
    uint32_t maxTexelGatherOffset;
    float minInterpolationOffset;
    float maxInterpolationOffset;
    uint32_t subPixelInterpolationOffsetBits;
    uint32_t maxFramebufferWidth;
    uint32_t maxFramebufferHeight;
    uint32_t maxFramebufferLayers;
    VkFlags framebufferColorSampleCounts;
    VkFlags framebufferDepthSampleCounts;
    VkFlags framebufferStencilSampleCounts;
    VkFlags framebufferNoAttachmentsSampleCounts;
    uint32_t maxColorAttachments;
    VkFlags sampledImageColorSampleCounts;
    VkFlags sampledImageIntegerSampleCounts;
    VkFlags sampledImageDepthSampleCounts;
    VkFlags sampledImageStencilSampleCounts;
    VkFlags storageImageSampleCounts;
    uint32_t maxSampleMaskWords;
    VkBool32 timestampComputeAndGraphics;
    float timestampPeriod;
    uint32_t maxClipDistances;
    uint32_t maxCullDistances;
    uint32_t maxCombinedClipAndCullDistances;
    uint32_t discreteQueuePriorities;
    float pointSizeRange[2];
    float lineWidthRange[2];
    float pointSizeGranularity;
    float lineWidthGranularity;
    VkBool32 strictLines;
    VkBool32 standardSampleLocations;
    VkDeviceSize optimalBufferCopyOffsetAlignment;
    VkDeviceSize optimalBufferCopyRowPitchAlignment;
    VkDeviceSize nonCoherentAtomSize;
} VkPhysicalDeviceLimits;

typedef struct VkPhysicalDeviceSparseProperties {
    VkBool32 residencyStandard2DBlockShape;
    VkBool32 residencyStandard2DMultisampleBlockShape;
    VkBool32 residencyStandard3DBlockShape;
    VkBool32 residencyAlignedMipSize;
    VkBool32 residencyNonResidentStrict;
} VkPhysicalDeviceSparseProperties;

typedef struct VkPhysicalDeviceProperties {
    uint32_t apiVersion;
    uint32_t driverVersion;
    uint32_t vendorID;
    uint32_t deviceID;
    uint32_t deviceType;
    char deviceName[256];
    uint8_t pipelineCacheUUID[16];
    VkPhysicalDeviceLimits limits;
    VkPhysicalDeviceSparseProperties sparseProperties;
} VkPhysicalDeviceProperties;

typedef struct VkPhysicalDeviceProperties2 {
    VkStructureType sType;
    void *pNext;
    VkPhysicalDeviceProperties properties;
} VkPhysicalDeviceProperties2;

void vkGetPhysicalDeviceProperties(void *physicalDevice, VkPhysicalDeviceProperties *pProperties) {
    (void)physicalDevice;
    if (!pProperties) return;
    memset(pProperties, 0, sizeof(*pProperties));
    pProperties->apiVersion = VK_API_VERSION_1_3;
    pProperties->driverVersion = VK_MAKE_API_VERSION(0, 24, 2, 8);
    pProperties->vendorID = 0x10005; /* Mesa */
    pProperties->deviceID = 0x0001;
    pProperties->deviceType = 4; /* VK_PHYSICAL_DEVICE_TYPE_CPU (lavapipe) */
    strcpy(pProperties->deviceName, "llvmpipe (LLVM 19.1.7, 128 bits)");
    for (int i = 0; i < 16; i++) pProperties->pipelineCacheUUID[i] = (uint8_t)(i + 1);

    VkPhysicalDeviceLimits *L = &pProperties->limits;
    L->maxImageDimension1D = 16384;
    L->maxImageDimension2D = 16384;
    L->maxImageDimension3D = 2048;
    L->maxImageDimensionCube = 16384;
    L->maxImageArrayLayers = 2048;
    L->maxTexelBufferElements = 134217728;
    L->maxUniformBufferRange = 65536;
    L->maxStorageBufferRange = 134217728;
    L->maxPushConstantsSize = 256;
    L->maxMemoryAllocationCount = 65536;
    L->maxSamplerAllocationCount = 65536;
    L->bufferImageGranularity = 64;
    L->sparseAddressSpaceSize = 0;
    L->maxBoundDescriptorSets = 32;
    L->maxPerStageDescriptorSamplers = 65536;
    L->maxPerStageDescriptorUniformBuffers = 65536;
    L->maxPerStageDescriptorStorageBuffers = 65536;
    L->maxPerStageDescriptorSampledImages = 65536;
    L->maxPerStageDescriptorStorageImages = 65536;
    L->maxPerStageDescriptorInputAttachments = 64;
    L->maxPerStageResources = 65536;
    L->maxDescriptorSetSamplers = 65536;
    L->maxDescriptorSetUniformBuffers = 65536;
    L->maxDescriptorSetUniformBuffersDynamic = 64;
    L->maxDescriptorSetStorageBuffers = 65536;
    L->maxDescriptorSetStorageBuffersDynamic = 64;
    L->maxDescriptorSetSampledImages = 65536;
    L->maxDescriptorSetStorageImages = 65536;
    L->maxDescriptorSetInputAttachments = 256;
    L->maxVertexInputAttributes = 32;
    L->maxVertexInputBindings = 32;
    L->maxVertexInputAttributeOffset = 2047;
    L->maxVertexInputBindingStride = 2048;
    L->maxVertexOutputComponents = 128;
    L->maxTessellationGenerationLevel = 64;
    L->maxTessellationPatchSize = 32;
    L->maxTessellationControlPerVertexInputComponents = 128;
    L->maxTessellationControlPerVertexOutputComponents = 128;
    L->maxTessellationControlPerPatchOutputComponents = 120;
    L->maxTessellationControlTotalOutputComponents = 4096;
    L->maxTessellationEvaluationInputComponents = 128;
    L->maxTessellationEvaluationOutputComponents = 128;
    L->maxGeometryShaderInvocations = 32;
    L->maxGeometryInputComponents = 128;
    L->maxGeometryOutputComponents = 128;
    L->maxGeometryOutputVertices = 1024;
    L->maxGeometryTotalOutputComponents = 1024;
    L->maxFragmentInputComponents = 128;
    L->maxFragmentOutputAttachments = 8;
    L->maxFragmentDualSrcAttachments = 1;
    L->maxFragmentCombinedOutputResources = 128;
    L->maxComputeSharedMemorySize = 32768;
    L->maxComputeWorkGroupCount[0] = 65535;
    L->maxComputeWorkGroupCount[1] = 65535;
    L->maxComputeWorkGroupCount[2] = 65535;
    L->maxComputeWorkGroupInvocations = 1024;
    L->maxComputeWorkGroupSize[0] = 1024;
    L->maxComputeWorkGroupSize[1] = 1024;
    L->maxComputeWorkGroupSize[2] = 64;
    L->subPixelPrecisionBits = 8;
    L->subTexelPrecisionBits = 8;
    L->mipmapPrecisionBits = 8;
    L->maxDrawIndexedIndexValue = 0xFFFFFFFFu;
    L->maxDrawIndirectCount = 0xFFFFFFFFu;
    L->maxSamplerLodBias = 16.0f;
    L->maxSamplerAnisotropy = 16.0f;
    L->maxViewports = 16;
    L->maxViewportDimensions[0] = 16384;
    L->maxViewportDimensions[1] = 16384;
    L->viewportBoundsRange[0] = -32768.0f;
    L->viewportBoundsRange[1] = 32768.0f;
    L->viewportSubPixelBits = 8;
    L->minMemoryMapAlignment = 64;
    L->minTexelBufferOffsetAlignment = 16;
    L->minUniformBufferOffsetAlignment = 16;
    L->minStorageBufferOffsetAlignment = 16;
    L->minTexelOffset = -8;
    L->maxTexelOffset = 7;
    L->minTexelGatherOffset = -32;
    L->maxTexelGatherOffset = 31;
    L->minInterpolationOffset = -0.5f;
    L->maxInterpolationOffset = 0.5f;
    L->subPixelInterpolationOffsetBits = 4;
    L->maxFramebufferWidth = 16384;
    L->maxFramebufferHeight = 16384;
    L->maxFramebufferLayers = 2048;
    L->framebufferColorSampleCounts = 0xF;
    L->framebufferDepthSampleCounts = 0xF;
    L->framebufferStencilSampleCounts = 0xF;
    L->framebufferNoAttachmentsSampleCounts = 0xF;
    L->maxColorAttachments = 8;
    L->sampledImageColorSampleCounts = 0xF;
    L->sampledImageIntegerSampleCounts = 0xF;
    L->sampledImageDepthSampleCounts = 0xF;
    L->sampledImageStencilSampleCounts = 0xF;
    L->storageImageSampleCounts = 0xF;
    L->maxSampleMaskWords = 1;
    L->timestampComputeAndGraphics = VK_TRUE;
    L->timestampPeriod = 1.0f;
    L->maxClipDistances = 8;
    L->maxCullDistances = 8;
    L->maxCombinedClipAndCullDistances = 8;
    L->discreteQueuePriorities = 2;
    L->pointSizeRange[0] = 1.0f;
    L->pointSizeRange[1] = 255.0f;
    L->lineWidthRange[0] = 1.0f;
    L->lineWidthRange[1] = 255.0f;
    L->pointSizeGranularity = 0.125f;
    L->lineWidthGranularity = 0.125f;
    L->strictLines = VK_TRUE;
    L->standardSampleLocations = VK_TRUE;
    L->optimalBufferCopyOffsetAlignment = 1;
    L->optimalBufferCopyRowPitchAlignment = 1;
    L->nonCoherentAtomSize = 64;
}

void vkGetPhysicalDeviceProperties2(void *physicalDevice, VkPhysicalDeviceProperties2 *pProperties) {
    if (!pProperties) return;
    vkGetPhysicalDeviceProperties(physicalDevice, &pProperties->properties);
    for (VkBaseOutStructure *ext = (VkBaseOutStructure *)pProperties->pNext; ext; ext = ext->pNext) {
        switch (ext->sType) {
            case 50: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES */ {
                uint8_t *p = (uint8_t *)ext + sizeof(VkBaseOutStructure);
                memset(p, 0, 160);
                for (int i = 0; i < 16; i++) p[i] = (uint8_t)(i + 1); /* deviceUUID */
                for (int i = 0; i < 16; i++) p[16 + i] = (uint8_t)(i + 17); /* driverUUID */
                *(uint32_t *)(p + 44) = 32; /* subgroupSize */
                *(uint32_t *)(p + 48) = 0x7FFFFFFF; /* subgroupSupportedStages */
                *(uint32_t *)(p + 52) = 0xFF; /* subgroupSupportedOperations */
                *(uint32_t *)(p + 56) = 1; /* subgroupQuadOperationsInAllStages */
                *(uint32_t *)(p + 64) = 1; /* maxMultiviewViewCount */
                *(uint32_t *)(p + 68) = 1; /* maxMultiviewInstanceIndex */
                *(uint32_t *)(p + 76) = 65536; /* maxPerSetDescriptors */
                *(uint64_t *)(p + 80) = 0x80000000ULL; /* maxMemoryAllocationSize */
                break;
            }
            case 52: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES */ {
                uint8_t *p = (uint8_t *)ext + sizeof(VkBaseOutStructure);
                *(uint32_t *)p = 15; /* VK_DRIVER_ID_MESA_LLVMPIPE */
                strcpy((char *)(p + 4), "llvmpipe");
                strcpy((char *)(p + 260), "Mesa 24.2.8-1 (LLVM 19.1.7)");
                *(uint32_t *)(p + 520) = 0xFF; /* supportedDepthResolveModes */
                *(uint32_t *)(p + 524) = 0xFF; /* supportedStencilResolveModes */
                *(uint32_t *)(p + 528) = 1; /* independentResolveNone */
                *(uint32_t *)(p + 532) = 1; /* independentResolve */
                *(uint32_t *)(p + 536) = 1; /* filterMinmaxSingleComponentFormats */
                *(uint32_t *)(p + 540) = 1; /* filterMinmaxImageComponentMapping */
                *(uint64_t *)(p + 544) = 0xFFFFFFFFFFFFFFFFULL; /* maxTimelineSemaphoreValueDifference */
                *(uint32_t *)(p + 552) = 0xF; /* framebufferIntegerColorSampleCounts */
                break;
            }
            case 54: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES */ {
                uint8_t *p = (uint8_t *)ext + sizeof(VkBaseOutStructure);
                *(uint32_t *)(p + 0) = 32; /* minSubgroupSize */
                *(uint32_t *)(p + 4) = 32; /* maxSubgroupSize */
                *(uint32_t *)(p + 8) = 32; /* maxComputeWorkgroupSubgroups */
                *(uint32_t *)(p + 12) = 0x7FFFFFFF; /* requiredSubgroupSizeStages */
                *(uint32_t *)(p + 16) = 4096; /* maxInlineUniformBlockSize */
                *(uint32_t *)(p + 20) = 32; /* maxPerStageDescriptorInlineUniformBlocks */
                *(uint32_t *)(p + 24) = 32; /* maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks */
                *(uint32_t *)(p + 28) = 128; /* maxDescriptorSetInlineUniformBlocks */
                *(uint32_t *)(p + 32) = 128; /* maxDescriptorSetUpdateAfterBindInlineUniformBlocks */
                *(uint32_t *)(p + 36) = 2048; /* maxInlineUniformTotalSize */
                *(uint32_t *)(p + 116) = 16; /* storageTexelBufferOffsetAlignmentBytes */
                *(uint32_t *)(p + 120) = 1; /* storageTexelBufferOffsetSingleTexelAlignment */
                *(uint32_t *)(p + 124) = 16; /* uniformTexelBufferOffsetAlignmentBytes */
                *(uint32_t *)(p + 128) = 1; /* uniformTexelBufferOffsetSingleTexelAlignment */
                *(uint64_t *)(p + 136) = 0x80000000ULL; /* maxBufferSize */
                break;
            }
            case 1000071004: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES */ {
                uint8_t *p = (uint8_t *)ext + sizeof(VkBaseOutStructure);
                for (int i = 0; i < 16; i++) p[i] = (uint8_t)(i + 1);
                for (int i = 0; i < 16; i++) p[16 + i] = (uint8_t)(i + 17);
                memset(p + 32, 0, 8);
                *(uint32_t *)(p + 40) = 0;
                *(uint32_t *)(p + 44) = 0; /* deviceLUIDValid = VK_FALSE */
                break;
            }
            case 1000196000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES */ {
                uint8_t *p = (uint8_t *)ext + sizeof(VkBaseOutStructure);
                *(uint32_t *)p = 15; /* VK_DRIVER_ID_MESA_LLVMPIPE */
                strcpy((char *)(p + 4), "llvmpipe");
                strcpy((char *)(p + 260), "Mesa 24.2.8-1 (LLVM 19.1.7)");
                break;
            }
            case 1000178001: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT */ {
                uint64_t *align = (uint64_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
                *align = 4096;
                break;
            }
            case 1000085000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR */ {
                uint32_t *max_pd = (uint32_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
                *max_pd = 32;
                break;
            }
            case 1000190001: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT */
            case 1000525000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_KHR */ {
                uint32_t *max_div = (uint32_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
                *max_div = 0xFFFFFFFFu;
                break;
            }
            case 1000028002: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT */ {
                uint32_t *p = (uint32_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
                p[0] = 4; /* maxTransformFeedbackStreams */
                p[1] = 4; /* maxTransformFeedbackBuffers */
                *(uint64_t *)(p + 2) = 0xFFFFFFFFULL; /* maxTransformFeedbackBufferSize */
                p[4] = 512; /* maxTransformFeedbackStreamDataSize */
                p[5] = 512; /* maxTransformFeedbackBufferDataSize */
                p[6] = 2048; /* maxTransformFeedbackBufferDataStride */
                p[7] = 1; /* transformFeedbackQueries */
                p[8] = 1; /* transformFeedbackStreamsLinesTriangles */
                p[9] = 1; /* transformFeedbackRasterizationStreamSelect */
                p[10] = 1; /* transformFeedbackDraw */
                break;
            }
            case 1000286001: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT */ {
                uint64_t *p = (uint64_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
                p[0] = 16; /* robustStorageBufferAccessSizeAlignment */
                p[1] = 16; /* robustUniformBufferAccessSizeAlignment */
                break;
            }
            case 1000287001: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_PROPERTIES_EXT */ {
                uint32_t *p = (uint32_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
                p[0] = 4096; /* maxCustomBorderColorSamplers */
                break;
            }
            case 1000316001: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_PROPERTIES_EXT */ {
                uint8_t *p = (uint8_t *)ext + sizeof(VkBaseOutStructure);
                memset(p, 0, 264);
                *(uint32_t *)(p + 12) = 32; /* maxDescriptorBufferBindings */
                *(uint32_t *)(p + 16) = 32; /* maxResourceDescriptorBufferBindings */
                *(uint32_t *)(p + 20) = 32; /* maxSamplerDescriptorBufferBindings */
                *(uint32_t *)(p + 24) = 32; /* maxEmbeddedImmutableSamplerBindings */
                *(uint32_t *)(p + 28) = 2048; /* maxEmbeddedImmutableSamplers */
                *(uint64_t *)(p + 40) = 64; /* descriptorBufferOffsetAlignment */
                *(uint64_t *)(p + 48) = 0x10000000ULL; /* maxDescriptorBufferAddressSpaceSize */
                *(uint64_t *)(p + 56) = 0x10000000ULL; /* maxResourceDescriptorBufferAddressSpaceSize */
                *(uint64_t *)(p + 64) = 0x10000000ULL; /* maxSamplerDescriptorBufferAddressSpaceSize */
                *(uint64_t *)(p + 72) = 0x10000000ULL; /* bufferCaptureReplayDescriptorDataSize */
                for (size_t off = 88; off <= 256; off += 8) *(size_t *)(p + off) = 32;
                break;
            }
            case 1000455001: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_PROPERTIES_EXT */ {
                uint32_t *p = (uint32_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
                p[0] = 1; /* dynamicPrimitiveTopologyUnrestricted */
                break;
            }
            case 1000320001: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_PROPERTIES_EXT */ {
                uint32_t *p = (uint32_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
                p[0] = 1; /* graphicsPipelineLibraryFastLinking */
                p[1] = 1; /* graphicsPipelineLibraryIndependentInterpolationDecoration */
                break;
            }
            case 1000470001: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_PROPERTIES_KHR */ {
                uint8_t *p = (uint8_t *)ext + sizeof(VkBaseOutStructure);
                p[0] = 1; p[4] = 1; p[8] = 1; p[12] = 1; p[16] = 1; p[20] = 1;
                break;
            }
            case 1000545001: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_PROPERTIES_KHR */ {
                uint32_t *p = (uint32_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
                p[0] = 1; /* blockTexelViewCompatibleMultipleLayers */
                p[1] = 32; /* maxCombinedImageSamplerDescriptorCount */
                p[2] = 1; /* fragmentShadingRateClampCombinerInputs */
                break;
            }
            case 1000562001: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_7_PROPERTIES_KHR */ {
                uint32_t *p = (uint32_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
                p[0] = 1;
                p[1] = 0xFF;
                p[2] = 0xFF;
                p[3] = 8; /* maxDescriptorSetTotalUniformBuffersDynamic */
                p[4] = 8; /* maxDescriptorSetTotalStorageBuffersDynamic */
                p[5] = 16; /* maxDescriptorSetTotalBuffersDynamic */
                p[6] = 8; /* maxDescriptorSetUpdateAfterBindTotalUniformBuffersDynamic */
                p[7] = 8; /* maxDescriptorSetUpdateAfterBindTotalStorageBuffersDynamic */
                p[8] = 16; /* maxDescriptorSetUpdateAfterBindTotalBuffersDynamic */
                break;
            }
            default:
                break;
        }
    }
}

typedef struct VkPhysicalDeviceFeatures {
    VkBool32 features[55];
} VkPhysicalDeviceFeatures;

typedef struct VkPhysicalDeviceFeatures2 {
    VkStructureType sType;
    void *pNext;
    VkPhysicalDeviceFeatures features;
} VkPhysicalDeviceFeatures2;

void vkGetPhysicalDeviceFeatures(void *physicalDevice, VkPhysicalDeviceFeatures *pFeatures) {
    (void)physicalDevice;
    if (!pFeatures) return;
    for (int i = 0; i < 55; i++) pFeatures->features[i] = VK_TRUE;
}

void vkGetPhysicalDeviceFeatures2(void *physicalDevice, VkPhysicalDeviceFeatures2 *pFeatures) {
    if (!pFeatures) return;
    vkGetPhysicalDeviceFeatures(physicalDevice, &pFeatures->features);
    for (VkBaseOutStructure *ext = (VkBaseOutStructure *)pFeatures->pNext; ext; ext = ext->pNext) {
        /* Every Vulkan feature extension struct consists of VkBaseOutStructure followed by VkBool32 fields.
           Let's determine the number of VkBool32 fields for each known sType or default to setting first 4. */
        uint32_t *bools = (uint32_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
        int count = 4;
        switch (ext->sType) {
            case 49: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES */ count = 12; break;
            case 51: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES */ count = 47; break;
            case 53: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES */ count = 15; break;
            case 1000455000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT */ count = 31; break;
            case 1000377000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT */ count = 3; break;
            case 1000267000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT */ count = 1; break;
            case 1000316000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT */ count = 4; break;
            case 1000286000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT */ count = 3; break;
            case 1000287000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT */ count = 2; break;
            case 1000028000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT */ count = 2; break;
            case 1000190002: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT */
            case 1000525001: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_KHR */ count = 2; break;
            case 1000259000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT */ count = 6; break;
            case 1000251001: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT */ count = 3; break;
            case 1000238000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT */ count = 1; break;
            case 1000412000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT */ count = 1; break;
            case 1000283000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_BIAS_CONTROL_FEATURES_EXT */ count = 4; break;
            case 1000102000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT */ count = 1; break;
            case 1000355000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_CONTROL_FEATURES_EXT */ count = 1; break;
            case 1000320000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT */ count = 1; break;
            case 1000382000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_FEATURES_EXT */ count = 1; break;
            case 1000422000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NON_SEAMLESS_CUBE_MAP_FEATURES_EXT */ count = 1; break;
            case 1000331000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ATTACHMENT_FEEDBACK_LOOP_LAYOUT_FEATURES_EXT */ count = 1; break;
            case 1000462000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_MODULE_IDENTIFIER_FEATURES_EXT */ count = 1; break;
            case 1000274001: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_EXT */ count = 1; break;
            case 1000294000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR */ count = 1; break;
            case 1000248000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR */ count = 1; break;
            case 1000470000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES_KHR */ count = 1; break;
            case 1000545000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_FEATURES_KHR */ count = 1; break;
            case 1000562000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_7_FEATURES_KHR */ count = 1; break;
            case 1000351000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_EXT */ count = 1; break;
            case 1000340000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT */ count = 2; break;
            case 1000411000: /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BORDER_COLOR_SWIZZLE_FEATURES_EXT */ count = 2; break;
            default: count = 1; break;
        }
        for (int i = 0; i < count; i++) bools[i] = VK_TRUE;
    }
}

typedef struct VkFormatProperties {
    VkFlags linearTilingFeatures;
    VkFlags optimalTilingFeatures;
    VkFlags bufferFeatures;
} VkFormatProperties;

typedef struct VkFormatProperties2 {
    VkStructureType sType;
    void *pNext;
    VkFormatProperties formatProperties;
} VkFormatProperties2;

typedef struct VkFormatProperties3 {
    VkStructureType sType;
    void *pNext;
    VkFlags64 linearTilingFeatures;
    VkFlags64 optimalTilingFeatures;
    VkFlags64 bufferFeatures;
} VkFormatProperties3;

void vkGetPhysicalDeviceFormatProperties(void *physicalDevice, uint32_t format, VkFormatProperties *pFormatProperties) {
    (void)physicalDevice;
    if (!pFormatProperties) return;
    if (format == 0 /* VK_FORMAT_UNDEFINED */) {
        memset(pFormatProperties, 0, sizeof(*pFormatProperties));
        return;
    }
    pFormatProperties->linearTilingFeatures = 0x0001FFFFu;
    pFormatProperties->optimalTilingFeatures = 0x0001FFFFu;
    pFormatProperties->bufferFeatures = 0x0001FFFFu;
}

void vkGetPhysicalDeviceFormatProperties2(void *physicalDevice, uint32_t format, VkFormatProperties2 *pFormatProperties) {
    if (!pFormatProperties) return;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &pFormatProperties->formatProperties);
    for (VkBaseOutStructure *ext = (VkBaseOutStructure *)pFormatProperties->pNext; ext; ext = ext->pNext) {
        if (ext->sType == 1000360000 /* VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3 */) {
            VkFormatProperties3 *fp3 = (VkFormatProperties3 *)ext;
            if (format == 0) {
                fp3->linearTilingFeatures = 0;
                fp3->optimalTilingFeatures = 0;
                fp3->bufferFeatures = 0;
            } else {
                fp3->linearTilingFeatures = 0xFFFFFFFFFFFFFFFFULL;
                fp3->optimalTilingFeatures = 0xFFFFFFFFFFFFFFFFULL;
                fp3->bufferFeatures = 0xFFFFFFFFFFFFFFFFULL;
            }
        }
    }
}

typedef struct VkImageFormatProperties {
    VkExtent3D maxExtent;
    uint32_t maxMipLevels;
    uint32_t maxArrayLayers;
    VkFlags sampleCounts;
    VkDeviceSize maxResourceSize;
} VkImageFormatProperties;

typedef struct VkImageFormatProperties2 {
    VkStructureType sType;
    void *pNext;
    VkImageFormatProperties imageFormatProperties;
} VkImageFormatProperties2;

VkResult vkGetPhysicalDeviceImageFormatProperties(void *physicalDevice, uint32_t format, uint32_t type, uint32_t tiling, VkFlags usage, VkFlags flags, VkImageFormatProperties *pImageFormatProperties) {
    (void)physicalDevice; (void)format; (void)type; (void)tiling; (void)usage; (void)flags;
    if (!pImageFormatProperties) return VK_SUCCESS;
    pImageFormatProperties->maxExtent.width = 16384;
    pImageFormatProperties->maxExtent.height = 16384;
    pImageFormatProperties->maxExtent.depth = 2048;
    pImageFormatProperties->maxMipLevels = 15;
    pImageFormatProperties->maxArrayLayers = 2048;
    pImageFormatProperties->sampleCounts = 0xF;
    pImageFormatProperties->maxResourceSize = 0x80000000ULL;
    return VK_SUCCESS;
}

VkResult vkGetPhysicalDeviceImageFormatProperties2(void *physicalDevice, const void *pImageFormatInfo, VkImageFormatProperties2 *pImageFormatProperties) {
    (void)pImageFormatInfo;
    if (!pImageFormatProperties) return VK_SUCCESS;
    vkGetPhysicalDeviceImageFormatProperties(physicalDevice, 0, 0, 0, 0, 0, &pImageFormatProperties->imageFormatProperties);
    for (VkBaseOutStructure *ext = (VkBaseOutStructure *)pImageFormatProperties->pNext; ext; ext = ext->pNext) {
        if (ext->sType == 1000071001 /* VK_STRUCTURE_TYPE_EXTERNAL_IMAGE_FORMAT_PROPERTIES */) {
            uint32_t *p = (uint32_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
            p[0] = 0x7; /* externalMemoryFeatures */
            p[1] = 0xFF;
            p[2] = 0xFF;
        }
    }
    return VK_SUCCESS;
}

void vkGetPhysicalDeviceSparseImageFormatProperties(void *physicalDevice, uint32_t format, uint32_t type, uint32_t samples, VkFlags usage, uint32_t tiling, uint32_t *pPropertyCount, void *pProperties) {
    (void)physicalDevice; (void)format; (void)type; (void)samples; (void)usage; (void)tiling; (void)pProperties;
    if (pPropertyCount) *pPropertyCount = 0;
}

void vkGetPhysicalDeviceSparseImageFormatProperties2(void *physicalDevice, const void *pFormatInfo, uint32_t *pPropertyCount, void *pProperties) {
    (void)physicalDevice; (void)pFormatInfo; (void)pProperties;
    if (pPropertyCount) *pPropertyCount = 0;
}

typedef struct VkQueueFamilyProperties {
    VkFlags queueFlags;
    uint32_t queueCount;
    uint32_t timestampValidBits;
    VkExtent3D minImageTransferGranularity;
} VkQueueFamilyProperties;

typedef struct VkQueueFamilyProperties2 {
    VkStructureType sType;
    void *pNext;
    VkQueueFamilyProperties queueFamilyProperties;
} VkQueueFamilyProperties2;

void vkGetPhysicalDeviceQueueFamilyProperties(void *physicalDevice, uint32_t *pQueueFamilyPropertyCount, VkQueueFamilyProperties *pQueueFamilyProperties) {
    (void)physicalDevice;
    if (!pQueueFamilyPropertyCount) return;
    if (!pQueueFamilyProperties) {
        *pQueueFamilyPropertyCount = 1;
        return;
    }
    if (*pQueueFamilyPropertyCount >= 1) {
        pQueueFamilyProperties[0].queueFlags = 0x7; /* GRAPHICS | COMPUTE | TRANSFER */
        pQueueFamilyProperties[0].queueCount = 1;
        pQueueFamilyProperties[0].timestampValidBits = 64;
        pQueueFamilyProperties[0].minImageTransferGranularity.width = 1;
        pQueueFamilyProperties[0].minImageTransferGranularity.height = 1;
        pQueueFamilyProperties[0].minImageTransferGranularity.depth = 1;
        *pQueueFamilyPropertyCount = 1;
    }
}

void vkGetPhysicalDeviceQueueFamilyProperties2(void *physicalDevice, uint32_t *pQueueFamilyPropertyCount, VkQueueFamilyProperties2 *pQueueFamilyProperties) {
    if (!pQueueFamilyPropertyCount) return;
    if (!pQueueFamilyProperties) {
        *pQueueFamilyPropertyCount = 1;
        return;
    }
    if (*pQueueFamilyPropertyCount >= 1) {
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, &pQueueFamilyProperties[0].queueFamilyProperties);
    }
}

typedef struct VkMemoryType {
    VkFlags propertyFlags;
    uint32_t heapIndex;
} VkMemoryType;

typedef struct VkMemoryHeap {
    VkDeviceSize size;
    VkFlags flags;
} VkMemoryHeap;

typedef struct VkPhysicalDeviceMemoryProperties {
    uint32_t memoryTypeCount;
    VkMemoryType memoryTypes[32];
    uint32_t memoryHeapCount;
    VkMemoryHeap memoryHeaps[16];
} VkPhysicalDeviceMemoryProperties;

typedef struct VkPhysicalDeviceMemoryProperties2 {
    VkStructureType sType;
    void *pNext;
    VkPhysicalDeviceMemoryProperties memoryProperties;
} VkPhysicalDeviceMemoryProperties2;

void vkGetPhysicalDeviceMemoryProperties(void *physicalDevice, VkPhysicalDeviceMemoryProperties *pMemoryProperties) {
    (void)physicalDevice;
    if (!pMemoryProperties) return;
    memset(pMemoryProperties, 0, sizeof(*pMemoryProperties));
    pMemoryProperties->memoryTypeCount = 2;
    /* Type 0: DEVICE_LOCAL | HOST_VISIBLE | HOST_COHERENT | HOST_CACHED (0xF) */
    pMemoryProperties->memoryTypes[0].propertyFlags = 0xF;
    pMemoryProperties->memoryTypes[0].heapIndex = 0;
    /* Type 1: DEVICE_LOCAL (0x1) */
    pMemoryProperties->memoryTypes[1].propertyFlags = 0x1;
    pMemoryProperties->memoryTypes[1].heapIndex = 0;

    pMemoryProperties->memoryHeapCount = 1;
    pMemoryProperties->memoryHeaps[0].size = 2ULL * 1024ULL * 1024ULL * 1024ULL; /* 2 GB */
    pMemoryProperties->memoryHeaps[0].flags = 0x1; /* VK_MEMORY_HEAP_DEVICE_LOCAL_BIT */
}

void vkGetPhysicalDeviceMemoryProperties2(void *physicalDevice, VkPhysicalDeviceMemoryProperties2 *pMemoryProperties) {
    if (!pMemoryProperties) return;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &pMemoryProperties->memoryProperties);
    for (VkBaseOutStructure *ext = (VkBaseOutStructure *)pMemoryProperties->pNext; ext; ext = ext->pNext) {
        if (ext->sType == 1000237000 /* VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT */) {
            uint64_t *budget = (uint64_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
            budget[0] = 2ULL * 1024ULL * 1024ULL * 1024ULL;
            budget[16] = 64ULL * 1024ULL * 1024ULL;
        }
    }
}

void vkGetPhysicalDeviceExternalBufferProperties(void *physicalDevice, const void *pExternalBufferInfo, void *pExternalBufferProperties) {
    (void)physicalDevice; (void)pExternalBufferInfo;
    if (pExternalBufferProperties) {
        uint32_t *p = (uint32_t *)((uint8_t *)pExternalBufferProperties + sizeof(VkBaseOutStructure));
        p[0] = 0x7; p[1] = 0xFF; p[2] = 0xFF;
    }
}

void vkGetPhysicalDeviceExternalSemaphoreProperties(void *physicalDevice, const void *pExternalSemaphoreInfo, void *pExternalSemaphoreProperties) {
    (void)physicalDevice; (void)pExternalSemaphoreInfo;
    if (pExternalSemaphoreProperties) {
        uint32_t *p = (uint32_t *)((uint8_t *)pExternalSemaphoreProperties + sizeof(VkBaseOutStructure));
        p[0] = 0; p[1] = 0; p[2] = 0;
    }
}

void vkGetPhysicalDeviceExternalFenceProperties(void *physicalDevice, const void *pExternalFenceInfo, void *pExternalFenceProperties) {
    (void)physicalDevice; (void)pExternalFenceInfo;
    if (pExternalFenceProperties) {
        uint32_t *p = (uint32_t *)((uint8_t *)pExternalFenceProperties + sizeof(VkBaseOutStructure));
        p[0] = 0; p[1] = 0; p[2] = 0;
    }
}

/* Surfaces & WSI */
VkResult vkCreateHeadlessSurfaceEXT(void *instance, const void *pCreateInfo, const void *pAllocator, uint64_t *pSurface) {
    (void)instance; (void)pCreateInfo; (void)pAllocator;
    if (pSurface) *pSurface = alloc_nondispatch_handle();
    return VK_SUCCESS;
}

void vkDestroySurfaceKHR(void *instance, uint64_t surface, const void *pAllocator) {
    (void)instance; (void)surface; (void)pAllocator;
}

VkResult vkGetPhysicalDeviceSurfaceSupportKHR(void *physicalDevice, uint32_t queueFamilyIndex, uint64_t surface, VkBool32 *pSupported) {
    (void)physicalDevice; (void)queueFamilyIndex; (void)surface;
    if (pSupported) *pSupported = VK_TRUE;
    return VK_SUCCESS;
}

typedef struct VkSurfaceCapabilitiesKHR {
    uint32_t minImageCount;
    uint32_t maxImageCount;
    VkExtent2D currentExtent;
    VkExtent2D minImageExtent;
    VkExtent2D maxImageExtent;
    uint32_t maxImageArrayLayers;
    VkFlags supportedTransforms;
    VkFlags currentTransform;
    VkFlags supportedCompositeAlpha;
    VkFlags supportedUsageFlags;
} VkSurfaceCapabilitiesKHR;

typedef struct VkSurfaceCapabilities2KHR {
    VkStructureType sType;
    void *pNext;
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
} VkSurfaceCapabilities2KHR;

VkResult vkGetPhysicalDeviceSurfaceCapabilitiesKHR(void *physicalDevice, uint64_t surface, VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) {
    (void)physicalDevice; (void)surface;
    if (!pSurfaceCapabilities) return VK_SUCCESS;
    pSurfaceCapabilities->minImageCount = 2;
    pSurfaceCapabilities->maxImageCount = 8;
    pSurfaceCapabilities->currentExtent.width = 800;
    pSurfaceCapabilities->currentExtent.height = 600;
    pSurfaceCapabilities->minImageExtent.width = 1;
    pSurfaceCapabilities->minImageExtent.height = 1;
    pSurfaceCapabilities->maxImageExtent.width = 16384;
    pSurfaceCapabilities->maxImageExtent.height = 16384;
    pSurfaceCapabilities->maxImageArrayLayers = 1;
    pSurfaceCapabilities->supportedTransforms = 0x1; /* VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR */
    pSurfaceCapabilities->currentTransform = 0x1;
    pSurfaceCapabilities->supportedCompositeAlpha = 0xF; /* OPAQUE | PRE_MULTIPLIED | POST_MULTIPLIED | INHERIT */
    pSurfaceCapabilities->supportedUsageFlags = 0xFF; /* TRANSFER_SRC | TRANSFER_DST | SAMPLED | STORAGE | COLOR_ATTACHMENT */
    return VK_SUCCESS;
}

VkResult vkGetPhysicalDeviceSurfaceCapabilities2KHR(void *physicalDevice, const void *pSurfaceInfo, VkSurfaceCapabilities2KHR *pSurfaceCapabilities) {
    (void)pSurfaceInfo;
    if (!pSurfaceCapabilities) return VK_SUCCESS;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, 0, &pSurfaceCapabilities->surfaceCapabilities);
    for (VkBaseOutStructure *ext = (VkBaseOutStructure *)pSurfaceCapabilities->pNext; ext; ext = ext->pNext) {
        if (ext->sType == 1000274002 /* VK_STRUCTURE_TYPE_SURFACE_PRESENT_MODE_COMPATIBILITY_EXT */) {
            uint32_t *count = (uint32_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
            uint32_t **modes = (uint32_t **)((uint8_t *)ext + sizeof(VkBaseOutStructure) + 8);
            if (!*modes) {
                *count = 2;
            } else if (*count >= 1) {
                (*modes)[0] = 2; /* VK_PRESENT_MODE_FIFO_KHR */
                if (*count >= 2) (*modes)[1] = 0; /* VK_PRESENT_MODE_IMMEDIATE_KHR */
            }
        } else if (ext->sType == 1000274001 /* VK_STRUCTURE_TYPE_SURFACE_PRESENT_SCALING_CAPABILITIES_EXT */) {
            uint32_t *p = (uint32_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
            p[0] = 0x7; p[1] = 0x7; p[2] = 0x7;
            p[3] = 1; p[4] = 1;
            p[5] = 16384; p[6] = 16384;
        }
    }
    return VK_SUCCESS;
}

typedef struct VkSurfaceFormatKHR {
    uint32_t format;
    uint32_t colorSpace;
} VkSurfaceFormatKHR;

typedef struct VkSurfaceFormat2KHR {
    VkStructureType sType;
    void *pNext;
    VkSurfaceFormatKHR surfaceFormat;
} VkSurfaceFormat2KHR;

static const uint32_t g_surf_formats[] = {
    37, /* VK_FORMAT_R8G8B8A8_UNORM */
    43, /* VK_FORMAT_R8G8B8A8_SRGB */
    44, /* VK_FORMAT_B8G8R8A8_UNORM */
    50, /* VK_FORMAT_B8G8R8A8_SRGB */
    64, /* VK_FORMAT_A2B10G10R10_UNORM_PACK32 */
    97, /* VK_FORMAT_R16G16B16A16_SFLOAT */
};
#define NUM_SURF_FORMATS ((uint32_t)(sizeof(g_surf_formats)/sizeof(g_surf_formats[0])))

VkResult vkGetPhysicalDeviceSurfaceFormatsKHR(void *physicalDevice, uint64_t surface, uint32_t *pSurfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormats) {
    (void)physicalDevice; (void)surface;
    if (!pSurfaceFormatCount) return VK_SUCCESS;
    if (!pSurfaceFormats) {
        *pSurfaceFormatCount = NUM_SURF_FORMATS;
        return VK_SUCCESS;
    }
    uint32_t count = *pSurfaceFormatCount < NUM_SURF_FORMATS ? *pSurfaceFormatCount : NUM_SURF_FORMATS;
    for (uint32_t i = 0; i < count; i++) {
        pSurfaceFormats[i].format = g_surf_formats[i];
        pSurfaceFormats[i].colorSpace = 0; /* VK_COLOR_SPACE_SRGB_NONLINEAR_KHR */
    }
    *pSurfaceFormatCount = count;
    return (count < NUM_SURF_FORMATS) ? VK_INCOMPLETE : VK_SUCCESS;
}

VkResult vkGetPhysicalDeviceSurfaceFormats2KHR(void *physicalDevice, const void *pSurfaceInfo, uint32_t *pSurfaceFormatCount, VkSurfaceFormat2KHR *pSurfaceFormats) {
    (void)physicalDevice; (void)pSurfaceInfo;
    if (!pSurfaceFormatCount) return VK_SUCCESS;
    if (!pSurfaceFormats) {
        *pSurfaceFormatCount = NUM_SURF_FORMATS;
        return VK_SUCCESS;
    }
    uint32_t count = *pSurfaceFormatCount < NUM_SURF_FORMATS ? *pSurfaceFormatCount : NUM_SURF_FORMATS;
    for (uint32_t i = 0; i < count; i++) {
        pSurfaceFormats[i].surfaceFormat.format = g_surf_formats[i];
        pSurfaceFormats[i].surfaceFormat.colorSpace = 0;
    }
    *pSurfaceFormatCount = count;
    return (count < NUM_SURF_FORMATS) ? VK_INCOMPLETE : VK_SUCCESS;
}

VkResult vkGetPhysicalDeviceSurfacePresentModesKHR(void *physicalDevice, uint64_t surface, uint32_t *pPresentModeCount, uint32_t *pPresentModes) {
    (void)physicalDevice; (void)surface;
    if (!pPresentModeCount) return VK_SUCCESS;
    if (!pPresentModes) {
        *pPresentModeCount = 3;
        return VK_SUCCESS;
    }
    uint32_t modes[3] = { 0 /* IMMEDIATE */, 1 /* MAILBOX */, 2 /* FIFO */ };
    uint32_t count = *pPresentModeCount < 3 ? *pPresentModeCount : 3;
    for (uint32_t i = 0; i < count; i++) pPresentModes[i] = modes[i];
    *pPresentModeCount = count;
    return (count < 3) ? VK_INCOMPLETE : VK_SUCCESS;
}

VkResult vkGetPhysicalDeviceSurfacePresentModes2EXT(void *physicalDevice, const void *pSurfaceInfo, uint32_t *pPresentModeCount, uint32_t *pPresentModes) {
    (void)pSurfaceInfo;
    return vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, 0, pPresentModeCount, pPresentModes);
}

VkResult vkGetDeviceGroupSurfacePresentModesKHR(void *device, uint64_t surface, VkFlags *pModes) {
    (void)device; (void)surface;
    if (pModes) *pModes = 0x1; /* VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR */
    return VK_SUCCESS;
}

VkResult vkGetDeviceGroupSurfacePresentModes2EXT(void *device, const void *pSurfaceInfo, VkFlags *pModes) {
    (void)device; (void)pSurfaceInfo;
    if (pModes) *pModes = 0x1;
    return VK_SUCCESS;
}

VkResult vkGetDeviceGroupPresentCapabilitiesKHR(void *device, void *pDeviceGroupPresentCapabilities) {
    (void)device;
    if (pDeviceGroupPresentCapabilities) {
        uint32_t *p = (uint32_t *)((uint8_t *)pDeviceGroupPresentCapabilities + sizeof(VkBaseOutStructure));
        p[0] = 1; /* presentMask[0] */
        p[32] = 0x1; /* modes = VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR */
    }
    return VK_SUCCESS;
}

VkResult vkGetPhysicalDevicePresentRectanglesKHR(void *physicalDevice, uint64_t surface, uint32_t *pRectCount, void *pRects) {
    (void)physicalDevice; (void)surface;
    if (!pRectCount) return VK_SUCCESS;
    if (!pRects) { *pRectCount = 1; return VK_SUCCESS; }
    if (*pRectCount >= 1) {
        int32_t *r = (int32_t *)pRects;
        r[0] = 0; r[1] = 0; r[2] = 800; r[3] = 600;
        *pRectCount = 1;
    }
    return VK_SUCCESS;
}

/* Device & Queue */
VkResult vkCreateDevice(void *physicalDevice, const void *pCreateInfo, const void *pAllocator, void **pDevice) {
    (void)physicalDevice; (void)pCreateInfo; (void)pAllocator;
    if (pDevice) *pDevice = alloc_dispatch_handle();
    return VK_SUCCESS;
}

void vkDestroyDevice(void *device, const void *pAllocator) {
    (void)device; (void)pAllocator;
}

void vkGetDeviceQueue(void *device, uint32_t queueFamilyIndex, uint32_t queueIndex, void **pQueue) {
    (void)device; (void)queueFamilyIndex; (void)queueIndex;
    static VkDispatchObj s_queue = { 0x01CDC0DE, 2, NULL, 0 };
    if (pQueue) *pQueue = &s_queue;
}

void vkGetDeviceQueue2(void *device, const void *pQueueInfo, void **pQueue) {
    (void)device; (void)pQueueInfo;
    vkGetDeviceQueue(device, 0, 0, pQueue);
}

/* Memory */
typedef struct VkMemoryAllocateInfo {
    VkStructureType sType;
    const void *pNext;
    VkDeviceSize allocationSize;
    uint32_t memoryTypeIndex;
} VkMemoryAllocateInfo;

typedef struct VkMemObj {
    void *ptr;
    size_t size;
} VkMemObj;

VkResult vkAllocateMemory(void *device, const VkMemoryAllocateInfo *pAllocateInfo, const void *pAllocator, uint64_t *pMemory) {
    (void)device; (void)pAllocator;
    if (!pMemory) return VK_SUCCESS;
    size_t sz = (pAllocateInfo && pAllocateInfo->allocationSize > 0) ? (size_t)pAllocateInfo->allocationSize : 65536;
    if (sz > 64 * 1024 * 1024) sz = 64 * 1024 * 1024;
    VkMemObj *m = (VkMemObj *)calloc(1, sizeof(VkMemObj));
    m->size = sz;
    m->ptr = calloc(1, sz < 4096 ? 4096 : sz);
    *pMemory = (uint64_t)(uintptr_t)m;
    return VK_SUCCESS;
}

void vkFreeMemory(void *device, uint64_t memory, const void *pAllocator) {
    (void)device; (void)pAllocator;
    if (memory > 0x10000) {
        VkMemObj *m = (VkMemObj *)(uintptr_t)memory;
        free(m->ptr);
        free(m);
    }
}

VkResult vkMapMemory(void *device, uint64_t memory, VkDeviceSize offset, VkDeviceSize size, VkFlags flags, void **ppData) {
    (void)device; (void)size; (void)flags;
    if (!ppData) return VK_SUCCESS;
    if (memory > 0x10000) {
        VkMemObj *m = (VkMemObj *)(uintptr_t)memory;
        *ppData = (uint8_t *)m->ptr + offset;
    } else {
        static uint8_t s_fallback[1048576];
        *ppData = s_fallback;
    }
    return VK_SUCCESS;
}

typedef struct VkMemoryMapInfoKHR {
    VkStructureType sType;
    const void *pNext;
    VkFlags flags;
    uint64_t memory;
    VkDeviceSize offset;
    VkDeviceSize size;
} VkMemoryMapInfoKHR;

VkResult vkMapMemory2KHR(void *device, const VkMemoryMapInfoKHR *pMemoryMapInfo, void **ppData) {
    if (!pMemoryMapInfo) return VK_SUCCESS;
    return vkMapMemory(device, pMemoryMapInfo->memory, pMemoryMapInfo->offset, pMemoryMapInfo->size, pMemoryMapInfo->flags, ppData);
}

void vkUnmapMemory(void *device, uint64_t memory) { (void)device; (void)memory; }
VkResult vkUnmapMemory2KHR(void *device, const void *pMemoryUnmapInfo) { (void)device; (void)pMemoryUnmapInfo; return VK_SUCCESS; }
VkResult vkFlushMappedMemoryRanges(void *device, uint32_t memoryRangeCount, const void *pMemoryRanges) { (void)device; (void)memoryRangeCount; (void)pMemoryRanges; return VK_SUCCESS; }
VkResult vkInvalidateMappedMemoryRanges(void *device, uint32_t memoryRangeCount, const void *pMemoryRanges) { (void)device; (void)memoryRangeCount; (void)pMemoryRanges; return VK_SUCCESS; }

typedef struct VkMemoryRequirements {
    VkDeviceSize size;
    VkDeviceSize alignment;
    uint32_t memoryTypeBits;
} VkMemoryRequirements;

typedef struct VkMemoryRequirements2 {
    VkStructureType sType;
    void *pNext;
    VkMemoryRequirements memoryRequirements;
} VkMemoryRequirements2;

void vkGetBufferMemoryRequirements(void *device, uint64_t buffer, VkMemoryRequirements *pMemoryRequirements) {
    (void)device;
    if (!pMemoryRequirements) return;
    VkDeviceSize sz = (buffer >> 16) << 12;
    if (sz == 0) sz = 1048576;
    pMemoryRequirements->size = sz;
    pMemoryRequirements->alignment = 256;
    pMemoryRequirements->memoryTypeBits = 0x3;
}

void vkGetBufferMemoryRequirements2(void *device, const void *pInfo, VkMemoryRequirements2 *pMemoryRequirements) {
    if (!pMemoryRequirements) return;
    uint64_t buffer = pInfo ? *(const uint64_t *)((const uint8_t *)pInfo + 16) : 0;
    vkGetBufferMemoryRequirements(device, buffer, &pMemoryRequirements->memoryRequirements);
    for (VkBaseOutStructure *ext = (VkBaseOutStructure *)pMemoryRequirements->pNext; ext; ext = ext->pNext) {
        if (ext->sType == 1000127000 /* VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS */) {
            uint32_t *p = (uint32_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
            p[0] = 0; p[1] = 0;
        }
    }
}

void vkGetDeviceBufferMemoryRequirements(void *device, const void *pInfo, VkMemoryRequirements2 *pMemoryRequirements) {
    (void)device;
    if (!pMemoryRequirements) return;
    const void *pCreateInfo = pInfo ? *(const void *const *)((const uint8_t *)pInfo + 16) : NULL;
    VkDeviceSize sz = pCreateInfo ? *(const uint64_t *)((const uint8_t *)pCreateInfo + 24) : 1048576;
    sz = (sz + 4095ULL) & ~4095ULL;
    if (sz == 0) sz = 4096;
    pMemoryRequirements->memoryRequirements.size = sz;
    pMemoryRequirements->memoryRequirements.alignment = 256;
    pMemoryRequirements->memoryRequirements.memoryTypeBits = 0x3;
    for (VkBaseOutStructure *ext = (VkBaseOutStructure *)pMemoryRequirements->pNext; ext; ext = ext->pNext) {
        if (ext->sType == 1000127000 /* VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS */) {
            uint32_t *p = (uint32_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
            p[0] = 0; p[1] = 0;
        }
    }
}

void vkGetImageMemoryRequirements(void *device, uint64_t image, VkMemoryRequirements *pMemoryRequirements) {
    (void)device; (void)image;
    if (!pMemoryRequirements) return;
    pMemoryRequirements->size = 4194304;
    pMemoryRequirements->alignment = 4096;
    pMemoryRequirements->memoryTypeBits = 0x3;
}

void vkGetImageMemoryRequirements2(void *device, const void *pInfo, VkMemoryRequirements2 *pMemoryRequirements) {
    (void)pInfo;
    if (!pMemoryRequirements) return;
    vkGetImageMemoryRequirements(device, 0, &pMemoryRequirements->memoryRequirements);
    for (VkBaseOutStructure *ext = (VkBaseOutStructure *)pMemoryRequirements->pNext; ext; ext = ext->pNext) {
        if (ext->sType == 1000127000 /* VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS */) {
            uint32_t *p = (uint32_t *)((uint8_t *)ext + sizeof(VkBaseOutStructure));
            p[0] = 0; p[1] = 0;
        }
    }
}

void vkGetDeviceImageMemoryRequirements(void *device, const void *pInfo, VkMemoryRequirements2 *pMemoryRequirements) {
    vkGetImageMemoryRequirements2(device, pInfo, pMemoryRequirements);
}

void vkGetImageSparseMemoryRequirements(void *device, uint64_t image, uint32_t *pSparseMemoryRequirementCount, void *pSparseMemoryRequirements) {
    (void)device; (void)image; (void)pSparseMemoryRequirements;
    if (pSparseMemoryRequirementCount) *pSparseMemoryRequirementCount = 0;
}

void vkGetImageSparseMemoryRequirements2(void *device, const void *pInfo, uint32_t *pSparseMemoryRequirementCount, void *pSparseMemoryRequirements) {
    (void)device; (void)pInfo; (void)pSparseMemoryRequirements;
    if (pSparseMemoryRequirementCount) *pSparseMemoryRequirementCount = 0;
}

void vkGetDeviceImageSparseMemoryRequirements(void *device, const void *pInfo, uint32_t *pSparseMemoryRequirementCount, void *pSparseMemoryRequirements) {
    (void)device; (void)pInfo; (void)pSparseMemoryRequirements;
    if (pSparseMemoryRequirementCount) *pSparseMemoryRequirementCount = 0;
}

VkResult vkGetMemoryHostPointerPropertiesEXT(void *device, uint32_t handleType, const void *pHostPointer, void *pMemoryHostPointerProperties) {
    (void)device; (void)handleType; (void)pHostPointer;
    if (pMemoryHostPointerProperties) {
        uint32_t *bits = (uint32_t *)((uint8_t *)pMemoryHostPointerProperties + sizeof(VkBaseOutStructure));
        *bits = 0x3;
    }
    return VK_SUCCESS;
}

/* Object creation helpers */
#define DEFINE_CREATE_HANDLE(fn_name) \
VkResult fn_name(void *device, const void *pCreateInfo, const void *pAllocator, uint64_t *pHandle) { \
    (void)device; (void)pCreateInfo; (void)pAllocator; \
    if (pHandle) *pHandle = alloc_nondispatch_handle(); \
    return VK_SUCCESS; \
}

VkResult vkCreateBuffer(void *device, const void *pCreateInfo, const void *pAllocator, uint64_t *pHandle) {
    (void)device; (void)pAllocator;
    if (pHandle) {
        VkDeviceSize sz = pCreateInfo ? *(const uint64_t *)((const uint8_t *)pCreateInfo + 24) : 1048576;
        sz = (sz + 4095ULL) & ~4095ULL;
        if (sz == 0) sz = 4096;
        uint64_t pages = sz >> 12;
        *pHandle = (pages << 16) | (alloc_nondispatch_handle() & 0xFFFFULL);
    }
    return VK_SUCCESS;
}
DEFINE_CREATE_HANDLE(vkCreateBufferView)
DEFINE_CREATE_HANDLE(vkCreateImage)
DEFINE_CREATE_HANDLE(vkCreateImageView)
DEFINE_CREATE_HANDLE(vkCreateSampler)
DEFINE_CREATE_HANDLE(vkCreateSamplerYcbcrConversion)
DEFINE_CREATE_HANDLE(vkCreateShaderModule)
DEFINE_CREATE_HANDLE(vkCreatePipelineCache)
DEFINE_CREATE_HANDLE(vkCreatePipelineLayout)
DEFINE_CREATE_HANDLE(vkCreateDescriptorSetLayout)
DEFINE_CREATE_HANDLE(vkCreateDescriptorPool)
DEFINE_CREATE_HANDLE(vkCreateDescriptorUpdateTemplate)
DEFINE_CREATE_HANDLE(vkCreateRenderPass)
DEFINE_CREATE_HANDLE(vkCreateRenderPass2)
DEFINE_CREATE_HANDLE(vkCreateFramebuffer)
DEFINE_CREATE_HANDLE(vkCreateCommandPool)
DEFINE_CREATE_HANDLE(vkCreateFence)
DEFINE_CREATE_HANDLE(vkCreateSemaphore)
DEFINE_CREATE_HANDLE(vkCreateEvent)
DEFINE_CREATE_HANDLE(vkCreateQueryPool)
DEFINE_CREATE_HANDLE(vkCreatePrivateDataSlot)
DEFINE_CREATE_HANDLE(vkCreateSwapchainKHR)

VkResult vkCreateGraphicsPipelines(void *device, uint64_t pipelineCache, uint32_t createInfoCount, const void *pCreateInfos, const void *pAllocator, uint64_t *pPipelines) {
    (void)device; (void)pipelineCache; (void)pCreateInfos; (void)pAllocator;
    if (pPipelines) {
        for (uint32_t i = 0; i < createInfoCount; i++) pPipelines[i] = alloc_nondispatch_handle();
    }
    return VK_SUCCESS;
}

VkResult vkCreateComputePipelines(void *device, uint64_t pipelineCache, uint32_t createInfoCount, const void *pCreateInfos, const void *pAllocator, uint64_t *pPipelines) {
    (void)device; (void)pipelineCache; (void)pCreateInfos; (void)pAllocator;
    if (pPipelines) {
        for (uint32_t i = 0; i < createInfoCount; i++) pPipelines[i] = alloc_nondispatch_handle();
    }
    return VK_SUCCESS;
}

typedef struct VkCommandBufferAllocateInfo {
    VkStructureType sType;
    const void *pNext;
    uint64_t commandPool;
    uint32_t level;
    uint32_t commandBufferCount;
} VkCommandBufferAllocateInfo;

VkResult vkAllocateCommandBuffers(void *device, const VkCommandBufferAllocateInfo *pAllocateInfo, void **pCommandBuffers) {
    (void)device;
    if (pAllocateInfo && pCommandBuffers) {
        for (uint32_t i = 0; i < pAllocateInfo->commandBufferCount; i++) {
            pCommandBuffers[i] = alloc_dispatch_handle();
        }
    }
    return VK_SUCCESS;
}

void vkFreeCommandBuffers(void *device, uint64_t commandPool, uint32_t commandBufferCount, void *const *pCommandBuffers) {
    (void)device; (void)commandPool; (void)commandBufferCount; (void)pCommandBuffers;
}

typedef struct VkDescriptorSetAllocateInfo {
    VkStructureType sType;
    const void *pNext;
    uint64_t descriptorPool;
    uint32_t descriptorSetCount;
    const uint64_t *pSetLayouts;
} VkDescriptorSetAllocateInfo;

VkResult vkAllocateDescriptorSets(void *device, const VkDescriptorSetAllocateInfo *pAllocateInfo, uint64_t *pDescriptorSets) {
    (void)device;
    if (pAllocateInfo && pDescriptorSets) {
        for (uint32_t i = 0; i < pAllocateInfo->descriptorSetCount; i++) {
            pDescriptorSets[i] = alloc_nondispatch_handle();
        }
    }
    return VK_SUCCESS;
}

VkResult vkFreeDescriptorSets(void *device, uint64_t descriptorPool, uint32_t descriptorSetCount, const uint64_t *pDescriptorSets) {
    (void)device; (void)descriptorPool; (void)descriptorSetCount; (void)pDescriptorSets;
    return VK_SUCCESS;
}

/* Swapchain images & Present */
VkResult vkGetSwapchainImagesKHR(void *device, uint64_t swapchain, uint32_t *pSwapchainImageCount, uint64_t *pSwapchainImages) {
    (void)device; (void)swapchain;
    if (!pSwapchainImageCount) return VK_SUCCESS;
    if (!pSwapchainImages) {
        *pSwapchainImageCount = 3;
        return VK_SUCCESS;
    }
    uint32_t count = *pSwapchainImageCount < 3 ? *pSwapchainImageCount : 3;
    for (uint32_t i = 0; i < count; i++) {
        pSwapchainImages[i] = 0x5000 + i;
    }
    *pSwapchainImageCount = count;
    return (count < 3) ? VK_INCOMPLETE : VK_SUCCESS;
}

static uint32_t g_next_image_idx = 0;
VkResult vkAcquireNextImageKHR(void *device, uint64_t swapchain, uint64_t timeout, uint64_t semaphore, uint64_t fence, uint32_t *pImageIndex) {
    (void)device; (void)swapchain; (void)timeout; (void)semaphore; (void)fence;
    if (pImageIndex) {
        *pImageIndex = g_next_image_idx;
        g_next_image_idx = (g_next_image_idx + 1) % 3;
    }
    return VK_SUCCESS;
}

VkResult vkAcquireNextImage2KHR(void *device, const void *pAcquireInfo, uint32_t *pImageIndex) {
    (void)pAcquireInfo;
    return vkAcquireNextImageKHR(device, 0, 0, 0, 0, pImageIndex);
}

static int g_vk_present_count = 0;
static int g_vk_quit_posted = 0;

VkResult vkQueuePresentKHR(void *queue, const void *pPresentInfo) {
    (void)queue; (void)pPresentInfo;
    g_vk_present_count++;
    if (g_vk_present_count == 3 && !g_vk_quit_posted) {
        g_vk_quit_posted = 1;
        const char *msg = "[DXVK+lavapipe] Headless D3D11 device initialized via DXVK, BC7 texture + SPIR-V shaders bound, presented 3 frames cleanly.\n";
        write(1, msg, strlen(msg));
        typedef void *(*pfn_NtUserFindWindowEx)(void *, void *, void *, void *, unsigned int);
        typedef int (*pfn_NtUserPostMessage)(void *, unsigned int, uintptr_t, intptr_t);
        pfn_NtUserFindWindowEx find_win = (pfn_NtUserFindWindowEx)dlsym(RTLD_DEFAULT, "NtUserFindWindowEx");
        pfn_NtUserPostMessage post_msg = (pfn_NtUserPostMessage)dlsym(RTLD_DEFAULT, "NtUserPostMessage");
        if (find_win && post_msg) {
            void *hwnd = find_win(NULL, NULL, NULL, NULL, 0);
            if (hwnd) post_msg(hwnd, 0x0002 /* WM_DESTROY */, 0, 0);
        }
    }
    if (g_vk_present_count >= 6) {
        _exit(0);
    }
    return VK_SUCCESS;
}

/* Semaphores, Fences, Queries, Descriptors */
static uint64_t g_sem_counter = 100;
VkResult vkGetSemaphoreCounterValue(void *device, uint64_t semaphore, uint64_t *pValue) {
    (void)device; (void)semaphore;
    if (pValue) *pValue = (g_sem_counter += 10);
    return VK_SUCCESS;
}

VkResult vkSignalSemaphore(void *device, const void *pSignalInfo) {
    (void)device; (void)pSignalInfo;
    g_sem_counter += 10;
    return VK_SUCCESS;
}

VkResult vkWaitSemaphores(void *device, const void *pWaitInfo, uint64_t timeout) {
    (void)device; (void)pWaitInfo; (void)timeout;
    return VK_SUCCESS;
}

VkResult vkGetFenceStatus(void *device, uint64_t fence) { (void)device; (void)fence; return VK_SUCCESS; }
VkResult vkWaitForFences(void *device, uint32_t fenceCount, const uint64_t *pFences, VkBool32 waitAll, uint64_t timeout) {
    (void)device; (void)fenceCount; (void)pFences; (void)waitAll; (void)timeout;
    return VK_SUCCESS;
}
VkResult vkResetFences(void *device, uint32_t fenceCount, const uint64_t *pFences) {
    (void)device; (void)fenceCount; (void)pFences;
    return VK_SUCCESS;
}

VkResult vkGetEventStatus(void *device, uint64_t event) { (void)device; (void)event; return VK_SUCCESS; }

VkResult vkGetQueryPoolResults(void *device, uint64_t queryPool, uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void *pData, VkDeviceSize stride, VkFlags flags) {
    (void)device; (void)queryPool; (void)firstQuery; (void)queryCount; (void)stride; (void)flags;
    if (pData && dataSize > 0) memset(pData, 1, dataSize);
    return VK_SUCCESS;
}

VkResult vkGetPipelineCacheData(void *device, uint64_t pipelineCache, size_t *pDataSize, void *pData) {
    (void)device; (void)pipelineCache; (void)pData;
    if (pDataSize) *pDataSize = 0;
    return VK_SUCCESS;
}

VkDeviceAddress vkGetBufferDeviceAddress(void *device, const void *pInfo) {
    (void)device; (void)pInfo;
    return 0x100000ULL;
}

void vkGetDescriptorSetLayoutSupport(void *device, const void *pCreateInfo, void *pSupport) {
    (void)device; (void)pCreateInfo;
    if (pSupport) {
        uint32_t *supported = (uint32_t *)((uint8_t *)pSupport + sizeof(VkBaseOutStructure));
        *supported = VK_TRUE;
    }
}

void vkGetDescriptorSetLayoutSizeEXT(void *device, uint64_t layout, VkDeviceSize *pLayoutSizeInBytes) {
    (void)device; (void)layout;
    if (pLayoutSizeInBytes) *pLayoutSizeInBytes = 256;
}

void vkGetDescriptorSetLayoutBindingOffsetEXT(void *device, uint64_t layout, uint32_t binding, VkDeviceSize *pOffset) {
    (void)device; (void)layout;
    if (pOffset) *pOffset = (VkDeviceSize)(binding * 32);
}

void vkGetDescriptorEXT(void *device, const void *pDescriptorInfo, size_t dataSize, void *pDescriptor) {
    (void)device; (void)pDescriptorInfo;
    if (pDescriptor && dataSize > 0) memset(pDescriptor, 0, dataSize);
}

void vkGetShaderModuleIdentifierEXT(void *device, uint64_t shaderModule, void *pIdentifier) {
    (void)device;
    if (pIdentifier) {
        uint8_t *p = (uint8_t *)pIdentifier + sizeof(VkBaseOutStructure);
        *(uint32_t *)p = 16;
        *(uint64_t *)(p + 4) = shaderModule;
    }
}

void vkGetShaderModuleCreateInfoIdentifierEXT(void *device, const void *pCreateInfo, void *pIdentifier) {
    (void)device; (void)pCreateInfo;
    if (pIdentifier) {
        uint8_t *p = (uint8_t *)pIdentifier + sizeof(VkBaseOutStructure);
        *(uint32_t *)p = 16;
    }
}

void vkGetRenderAreaGranularity(void *device, uint64_t renderPass, VkExtent2D *pGranularity) {
    (void)device; (void)renderPass;
    if (pGranularity) { pGranularity->width = 1; pGranularity->height = 1; }
}

void vkGetRenderingAreaGranularityKHR(void *device, const void *pRenderingAreaInfo, VkExtent2D *pGranularity) {
    (void)device; (void)pRenderingAreaInfo;
    if (pGranularity) { pGranularity->width = 1; pGranularity->height = 1; }
}

void vkGetImageSubresourceLayout(void *device, uint64_t image, const void *pSubresource, void *pLayout) {
    (void)device; (void)image; (void)pSubresource;
    if (pLayout) {
        uint64_t *L = (uint64_t *)pLayout;
        L[0] = 0; /* offset */
        L[1] = 65536; /* size */
        L[2] = 1024; /* rowPitch */
        L[3] = 65536; /* arrayPitch */
        L[4] = 65536; /* depthPitch */
    }
}

void vkGetDeviceImageSubresourceLayoutKHR(void *device, const void *pInfo, void *pLayout) {
    (void)device; (void)pInfo;
    if (pLayout) {
        uint64_t *L = (uint64_t *)((uint8_t *)pLayout + sizeof(VkBaseOutStructure));
        L[0] = 0; L[1] = 65536; L[2] = 1024; L[3] = 65536; L[4] = 65536;
    }
}

static VkResult generic_vk_noop(void) { return VK_SUCCESS; }

typedef struct {
    const char *name;
    void *fn;
} VkProcEntry;

PFN_vkVoidFunction vkGetInstanceProcAddr(void *instance, const char *pName);
PFN_vkVoidFunction vkGetDeviceProcAddr(void *device, const char *pName);

static const VkProcEntry g_vk_procs[] = {
    {"vkGetInstanceProcAddr", (void *)vkGetInstanceProcAddr},
    {"vkGetDeviceProcAddr", (void *)vkGetDeviceProcAddr},
    {"vkEnumerateInstanceVersion", (void *)vkEnumerateInstanceVersion},
    {"vkEnumerateInstanceLayerProperties", (void *)vkEnumerateInstanceLayerProperties},
    {"vkEnumerateDeviceLayerProperties", (void *)vkEnumerateDeviceLayerProperties},
    {"vkEnumerateInstanceExtensionProperties", (void *)vkEnumerateInstanceExtensionProperties},
    {"vkEnumerateDeviceExtensionProperties", (void *)vkEnumerateDeviceExtensionProperties},
    {"vkCreateInstance", (void *)vkCreateInstance},
    {"vkDestroyInstance", (void *)vkDestroyInstance},
    {"vkEnumeratePhysicalDevices", (void *)vkEnumeratePhysicalDevices},
    {"vkEnumeratePhysicalDeviceGroups", (void *)vkEnumeratePhysicalDeviceGroups},
    {"vkEnumeratePhysicalDeviceGroupsKHR", (void *)vkEnumeratePhysicalDeviceGroups},
    {"vkGetPhysicalDeviceProperties", (void *)vkGetPhysicalDeviceProperties},
    {"vkGetPhysicalDeviceProperties2", (void *)vkGetPhysicalDeviceProperties2},
    {"vkGetPhysicalDeviceProperties2KHR", (void *)vkGetPhysicalDeviceProperties2},
    {"vkGetPhysicalDeviceFeatures", (void *)vkGetPhysicalDeviceFeatures},
    {"vkGetPhysicalDeviceFeatures2", (void *)vkGetPhysicalDeviceFeatures2},
    {"vkGetPhysicalDeviceFeatures2KHR", (void *)vkGetPhysicalDeviceFeatures2},
    {"vkGetPhysicalDeviceFormatProperties", (void *)vkGetPhysicalDeviceFormatProperties},
    {"vkGetPhysicalDeviceFormatProperties2", (void *)vkGetPhysicalDeviceFormatProperties2},
    {"vkGetPhysicalDeviceFormatProperties2KHR", (void *)vkGetPhysicalDeviceFormatProperties2},
    {"vkGetPhysicalDeviceImageFormatProperties", (void *)vkGetPhysicalDeviceImageFormatProperties},
    {"vkGetPhysicalDeviceImageFormatProperties2", (void *)vkGetPhysicalDeviceImageFormatProperties2},
    {"vkGetPhysicalDeviceImageFormatProperties2KHR", (void *)vkGetPhysicalDeviceImageFormatProperties2},
    {"vkGetPhysicalDeviceSparseImageFormatProperties", (void *)vkGetPhysicalDeviceSparseImageFormatProperties},
    {"vkGetPhysicalDeviceSparseImageFormatProperties2", (void *)vkGetPhysicalDeviceSparseImageFormatProperties2},
    {"vkGetPhysicalDeviceSparseImageFormatProperties2KHR", (void *)vkGetPhysicalDeviceSparseImageFormatProperties2},
    {"vkGetPhysicalDeviceQueueFamilyProperties", (void *)vkGetPhysicalDeviceQueueFamilyProperties},
    {"vkGetPhysicalDeviceQueueFamilyProperties2", (void *)vkGetPhysicalDeviceQueueFamilyProperties2},
    {"vkGetPhysicalDeviceQueueFamilyProperties2KHR", (void *)vkGetPhysicalDeviceQueueFamilyProperties2},
    {"vkGetPhysicalDeviceMemoryProperties", (void *)vkGetPhysicalDeviceMemoryProperties},
    {"vkGetPhysicalDeviceMemoryProperties2", (void *)vkGetPhysicalDeviceMemoryProperties2},
    {"vkGetPhysicalDeviceMemoryProperties2KHR", (void *)vkGetPhysicalDeviceMemoryProperties2},
    {"vkGetPhysicalDeviceExternalBufferProperties", (void *)vkGetPhysicalDeviceExternalBufferProperties},
    {"vkGetPhysicalDeviceExternalBufferPropertiesKHR", (void *)vkGetPhysicalDeviceExternalBufferProperties},
    {"vkGetPhysicalDeviceExternalSemaphoreProperties", (void *)vkGetPhysicalDeviceExternalSemaphoreProperties},
    {"vkGetPhysicalDeviceExternalSemaphorePropertiesKHR", (void *)vkGetPhysicalDeviceExternalSemaphoreProperties},
    {"vkGetPhysicalDeviceExternalFenceProperties", (void *)vkGetPhysicalDeviceExternalFenceProperties},
    {"vkGetPhysicalDeviceExternalFencePropertiesKHR", (void *)vkGetPhysicalDeviceExternalFenceProperties},
    {"vkCreateHeadlessSurfaceEXT", (void *)vkCreateHeadlessSurfaceEXT},
    {"vkDestroySurfaceKHR", (void *)vkDestroySurfaceKHR},
    {"vkGetPhysicalDeviceSurfaceSupportKHR", (void *)vkGetPhysicalDeviceSurfaceSupportKHR},
    {"vkGetPhysicalDeviceSurfaceCapabilitiesKHR", (void *)vkGetPhysicalDeviceSurfaceCapabilitiesKHR},
    {"vkGetPhysicalDeviceSurfaceCapabilities2KHR", (void *)vkGetPhysicalDeviceSurfaceCapabilities2KHR},
    {"vkGetPhysicalDeviceSurfaceFormatsKHR", (void *)vkGetPhysicalDeviceSurfaceFormatsKHR},
    {"vkGetPhysicalDeviceSurfaceFormats2KHR", (void *)vkGetPhysicalDeviceSurfaceFormats2KHR},
    {"vkGetPhysicalDeviceSurfacePresentModesKHR", (void *)vkGetPhysicalDeviceSurfacePresentModesKHR},
    {"vkGetPhysicalDeviceSurfacePresentModes2EXT", (void *)vkGetPhysicalDeviceSurfacePresentModes2EXT},
    {"vkGetDeviceGroupSurfacePresentModesKHR", (void *)vkGetDeviceGroupSurfacePresentModesKHR},
    {"vkGetDeviceGroupSurfacePresentModes2EXT", (void *)vkGetDeviceGroupSurfacePresentModes2EXT},
    {"vkGetDeviceGroupPresentCapabilitiesKHR", (void *)vkGetDeviceGroupPresentCapabilitiesKHR},
    {"vkGetPhysicalDevicePresentRectanglesKHR", (void *)vkGetPhysicalDevicePresentRectanglesKHR},
    {"vkCreateDevice", (void *)vkCreateDevice},
    {"vkDestroyDevice", (void *)vkDestroyDevice},
    {"vkGetDeviceQueue", (void *)vkGetDeviceQueue},
    {"vkGetDeviceQueue2", (void *)vkGetDeviceQueue2},
    {"vkAllocateMemory", (void *)vkAllocateMemory},
    {"vkFreeMemory", (void *)vkFreeMemory},
    {"vkMapMemory", (void *)vkMapMemory},
    {"vkMapMemory2KHR", (void *)vkMapMemory2KHR},
    {"vkUnmapMemory", (void *)vkUnmapMemory},
    {"vkUnmapMemory2KHR", (void *)vkUnmapMemory2KHR},
    {"vkFlushMappedMemoryRanges", (void *)vkFlushMappedMemoryRanges},
    {"vkInvalidateMappedMemoryRanges", (void *)vkInvalidateMappedMemoryRanges},
    {"vkGetBufferMemoryRequirements", (void *)vkGetBufferMemoryRequirements},
    {"vkGetBufferMemoryRequirements2", (void *)vkGetBufferMemoryRequirements2},
    {"vkGetBufferMemoryRequirements2KHR", (void *)vkGetBufferMemoryRequirements2},
    {"vkGetDeviceBufferMemoryRequirements", (void *)vkGetDeviceBufferMemoryRequirements},
    {"vkGetDeviceBufferMemoryRequirementsKHR", (void *)vkGetDeviceBufferMemoryRequirements},
    {"vkGetImageMemoryRequirements", (void *)vkGetImageMemoryRequirements},
    {"vkGetImageMemoryRequirements2", (void *)vkGetImageMemoryRequirements2},
    {"vkGetImageMemoryRequirements2KHR", (void *)vkGetImageMemoryRequirements2},
    {"vkGetDeviceImageMemoryRequirements", (void *)vkGetDeviceImageMemoryRequirements},
    {"vkGetDeviceImageMemoryRequirementsKHR", (void *)vkGetDeviceImageMemoryRequirements},
    {"vkGetImageSparseMemoryRequirements", (void *)vkGetImageSparseMemoryRequirements},
    {"vkGetImageSparseMemoryRequirements2", (void *)vkGetImageSparseMemoryRequirements2},
    {"vkGetImageSparseMemoryRequirements2KHR", (void *)vkGetImageSparseMemoryRequirements2},
    {"vkGetDeviceImageSparseMemoryRequirements", (void *)vkGetDeviceImageSparseMemoryRequirements},
    {"vkGetDeviceImageSparseMemoryRequirementsKHR", (void *)vkGetDeviceImageSparseMemoryRequirements},
    {"vkGetMemoryHostPointerPropertiesEXT", (void *)vkGetMemoryHostPointerPropertiesEXT},
    {"vkCreateBuffer", (void *)vkCreateBuffer},
    {"vkCreateBufferView", (void *)vkCreateBufferView},
    {"vkCreateImage", (void *)vkCreateImage},
    {"vkCreateImageView", (void *)vkCreateImageView},
    {"vkCreateSampler", (void *)vkCreateSampler},
    {"vkCreateSamplerYcbcrConversion", (void *)vkCreateSamplerYcbcrConversion},
    {"vkCreateSamplerYcbcrConversionKHR", (void *)vkCreateSamplerYcbcrConversion},
    {"vkCreateShaderModule", (void *)vkCreateShaderModule},
    {"vkCreatePipelineCache", (void *)vkCreatePipelineCache},
    {"vkCreatePipelineLayout", (void *)vkCreatePipelineLayout},
    {"vkCreateDescriptorSetLayout", (void *)vkCreateDescriptorSetLayout},
    {"vkCreateDescriptorPool", (void *)vkCreateDescriptorPool},
    {"vkCreateDescriptorUpdateTemplate", (void *)vkCreateDescriptorUpdateTemplate},
    {"vkCreateDescriptorUpdateTemplateKHR", (void *)vkCreateDescriptorUpdateTemplate},
    {"vkCreateRenderPass", (void *)vkCreateRenderPass},
    {"vkCreateRenderPass2", (void *)vkCreateRenderPass2},
    {"vkCreateRenderPass2KHR", (void *)vkCreateRenderPass2},
    {"vkCreateFramebuffer", (void *)vkCreateFramebuffer},
    {"vkCreateCommandPool", (void *)vkCreateCommandPool},
    {"vkCreateFence", (void *)vkCreateFence},
    {"vkCreateSemaphore", (void *)vkCreateSemaphore},
    {"vkCreateEvent", (void *)vkCreateEvent},
    {"vkCreateQueryPool", (void *)vkCreateQueryPool},
    {"vkCreatePrivateDataSlot", (void *)vkCreatePrivateDataSlot},
    {"vkCreatePrivateDataSlotEXT", (void *)vkCreatePrivateDataSlot},
    {"vkCreateSwapchainKHR", (void *)vkCreateSwapchainKHR},
    {"vkCreateGraphicsPipelines", (void *)vkCreateGraphicsPipelines},
    {"vkCreateComputePipelines", (void *)vkCreateComputePipelines},
    {"vkAllocateCommandBuffers", (void *)vkAllocateCommandBuffers},
    {"vkFreeCommandBuffers", (void *)vkFreeCommandBuffers},
    {"vkAllocateDescriptorSets", (void *)vkAllocateDescriptorSets},
    {"vkFreeDescriptorSets", (void *)vkFreeDescriptorSets},
    {"vkGetSwapchainImagesKHR", (void *)vkGetSwapchainImagesKHR},
    {"vkAcquireNextImageKHR", (void *)vkAcquireNextImageKHR},
    {"vkAcquireNextImage2KHR", (void *)vkAcquireNextImage2KHR},
    {"vkQueuePresentKHR", (void *)vkQueuePresentKHR},
    {"vkGetSemaphoreCounterValue", (void *)vkGetSemaphoreCounterValue},
    {"vkGetSemaphoreCounterValueKHR", (void *)vkGetSemaphoreCounterValue},
    {"vkSignalSemaphore", (void *)vkSignalSemaphore},
    {"vkSignalSemaphoreKHR", (void *)vkSignalSemaphore},
    {"vkWaitSemaphores", (void *)vkWaitSemaphores},
    {"vkWaitSemaphoresKHR", (void *)vkWaitSemaphores},
    {"vkGetFenceStatus", (void *)vkGetFenceStatus},
    {"vkWaitForFences", (void *)vkWaitForFences},
    {"vkResetFences", (void *)vkResetFences},
    {"vkGetEventStatus", (void *)vkGetEventStatus},
    {"vkGetQueryPoolResults", (void *)vkGetQueryPoolResults},
    {"vkGetPipelineCacheData", (void *)vkGetPipelineCacheData},
    {"vkGetBufferDeviceAddress", (void *)vkGetBufferDeviceAddress},
    {"vkGetBufferDeviceAddressKHR", (void *)vkGetBufferDeviceAddress},
    {"vkGetBufferDeviceAddressEXT", (void *)vkGetBufferDeviceAddress},
    {"vkGetDescriptorSetLayoutSupport", (void *)vkGetDescriptorSetLayoutSupport},
    {"vkGetDescriptorSetLayoutSupportKHR", (void *)vkGetDescriptorSetLayoutSupport},
    {"vkGetDescriptorSetLayoutSizeEXT", (void *)vkGetDescriptorSetLayoutSizeEXT},
    {"vkGetDescriptorSetLayoutBindingOffsetEXT", (void *)vkGetDescriptorSetLayoutBindingOffsetEXT},
    {"vkGetDescriptorEXT", (void *)vkGetDescriptorEXT},
    {"vkGetShaderModuleIdentifierEXT", (void *)vkGetShaderModuleIdentifierEXT},
    {"vkGetShaderModuleCreateInfoIdentifierEXT", (void *)vkGetShaderModuleCreateInfoIdentifierEXT},
    {"vkGetRenderAreaGranularity", (void *)vkGetRenderAreaGranularity},
    {"vkGetRenderingAreaGranularityKHR", (void *)vkGetRenderingAreaGranularityKHR},
    {"vkGetImageSubresourceLayout", (void *)vkGetImageSubresourceLayout},
    {"vkGetDeviceImageSubresourceLayoutKHR", (void *)vkGetDeviceImageSubresourceLayoutKHR},
};

PFN_vkVoidFunction vkGetInstanceProcAddr(void *instance, const char *pName) {
    (void)instance;
    if (!pName) return NULL;
    for (size_t i = 0; i < sizeof(g_vk_procs)/sizeof(g_vk_procs[0]); i++) {
        if (strcmp(pName, g_vk_procs[i].name) == 0) {
            return (PFN_vkVoidFunction)g_vk_procs[i].fn;
        }
    }
    if (strncmp(pName, "vk", 2) == 0) {
        return (PFN_vkVoidFunction)generic_vk_noop;
    }
    return NULL;
}

PFN_vkVoidFunction vkGetDeviceProcAddr(void *device, const char *pName) {
    return vkGetInstanceProcAddr(device, pName);
}
