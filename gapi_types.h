#ifndef _GAPI_TYPES
#define _GAPI_TYPES

#include "cglm/types-struct.h"
#include "cglm/types.h"
#include "types.h"
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#define GAPI_MAX_FRAMES_IN_FLIGHT 2
#define GAPI_MAX_LAYOUT_BINDINGS 32
#define GAPI_OBJECT_MAX_TEXTURES 8
#define GAPI_OBJECT_MAX_UNIFORM_BUFFERS 8

typedef enum {
    GAPI_SUCCESS = 0,
    GAPI_ERROR_GENERIC,
    GAPI_SYSTEM_ERROR,
    GAPI_VULKAN_ERROR,
    GAPI_GLFW_ERROR,
    GAPI_NO_DEVICE_FOUND,
    GAPI_VULKAN_FEATURE_UNSUPPORTED,
    GAPI_INVALID_HANDLE,
    GAPI_TOO_MANY_LAYOUT_BINDINGS,
    GAPI_TOO_MANY_TEXTURES,
    GAPI_TOO_MANY_UNIFORM_BUFFERS,
} GapiResult;

typedef enum {
    GAPI_WINDOW_RESIZEABLE = 1,
} GapiWindowFlags;

typedef enum {
    GAPI_ALPHA_BLENDING_NONE = 0,
    GAPI_ALPHA_BLENDING_BLEND,
    GAPI_ALPHA_BLENDING_ADDITIVE,
} GapiAlphaBlending;

typedef enum {
    GAPI_TOPOLOGY_TRIANGLES = 0,
    GAPI_TOPOLOGY_LINES,
    GAPI_TOPOLOGY_POINTS,
} GapiTopology;

typedef uint32_t GapiMeshHandle;
typedef uint32_t GapiObjectHandle;
typedef uint32_t GapiTextureHandle;
typedef uint32_t GapiFontHandle;
typedef uint32_t GapiRectTextureHandle;
typedef uint32_t GapiPipelineHandle;
typedef uint32_t GapiUniformBufferHandle;

typedef struct {
    uint32_t width;
    uint32_t height;
    const char *title;
    GapiWindowFlags flags;
} GapiWindowInitInfo;

typedef enum {
    GAPI_DESCRIPTOR_UNIFORM_BUFFER,
    GAPI_DESCRIPTOR_TEXTURE,
} GapiDescriptorType;

typedef enum {
    GAPI_STAGE_VERTEX,
    GAPI_STAGE_FRAGMENT,
} GapiShaderStage;

typedef struct {
    uint32_t binding;
    GapiDescriptorType type;
    GapiShaderStage stage;
} GapiDescriptorLayoutItem;

typedef struct {
    GapiShaderStage stage;
    uint32_t offset;
    uint32_t size;
} GapiPushConstantRange;

typedef struct {
    const char *shader_code;
    uint32_t shader_code_size;
    uint32_t layout_item_count;
    GapiDescriptorLayoutItem *layout_items;
    uint32_t push_constant_range_count;
    GapiPushConstantRange *push_constant_ranges;
    GapiAlphaBlending alpha_blending_mode;
    GapiTopology topology;
} GapiPipelineCreateInfo;

typedef struct {
    VkPipeline pipeline;
    VkPipelineLayout pipeline_layout;
    VkDescriptorSetLayout descriptor_set_layout;
} GapiPipeline;

typedef struct {
    GapiWindowInitInfo window;
} GapiInitInfo;

typedef struct {
    vec3 pos;
    vec3 target;
    vec3 up;
    float fov_degrees;
    float near_plane;
    float far_plane;
} GapiCamera;

typedef struct {
    VkImage image;
    VkDeviceMemory image_memory;
    VkImageView image_view;
    VkSampler sampler;
    uint32_t binding;
} GapiTexture;

typedef struct {
    VkDescriptorSet descriptor_set;
} GapiRectTexture;

typedef struct {
    mat4 model;
    mat4 view;
    mat4 projection;
    vec4 color_tint;
} GapiUBO;

typedef struct {
    VkBuffer vertex_buffer;
    VkBuffer index_buffer;
    VkDeviceMemory vertex_memory;
    VkDeviceMemory index_memory;
    uint32_t index_count;
} GapiMesh;

typedef struct {
    VkBuffer buffers[GAPI_MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory memories[GAPI_MAX_FRAMES_IN_FLIGHT];
    void *mappings[GAPI_MAX_FRAMES_IN_FLIGHT];
    uint32_t size;
    uint32_t binding;
} GapiUniformBuffer;

typedef struct {
    GapiMeshHandle mesh_handle;
    uint32_t texture_count;
    GapiTextureHandle texture_handles[GAPI_OBJECT_MAX_TEXTURES];
    uint32_t uniform_buffer_count;
    GapiUniformBufferHandle
        uniform_buffer_handles[GAPI_OBJECT_MAX_UNIFORM_BUFFERS];
} GapiObject;

typedef struct {
    FontMetadata metadata;
    GapiTextureHandle atlas_texture;
} GapiFont;

typedef struct {
    float x;
    float y;
    float width;
    float height;
} GapiRect;

typedef struct {
    GapiRect positioning;
    vec4 color;
    GapiRect texture_slice;
    float z_index;
} RectPushConstantData;

#endif
