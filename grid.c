#include "grid.h"
#include "gapi.h"
#include "gapi_builtin_shaders.h"
#include "gapi_types.h"
#include "utility_macros.h"

GapiResult grid_pipeline_create(GapiPipelineHandle *out_pipeline_handle) {

    GapiDescriptorLayoutItem layout_items[] = {
        {
            .binding = 0,
            .type = GAPI_DESCRIPTOR_UNIFORM_BUFFER,
            .stage = GAPI_STAGE_VERTEX,
        },
    };
    GapiPipelineCreateInfo create_info = {
        .shader_code = gapi_builtin_line_shader,
        .shader_code_size = gapi_builtin_line_shader_size,
        .alpha_blending_mode = GAPI_ALPHA_BLENDING_BLEND,
        .layout_item_count = COUNT(layout_items),
        .layout_items = layout_items,
        .topology = GAPI_TOPOLOGY_LINES,
    };
    GAPI_ERR(gapi_pipeline_create(&create_info, out_pipeline_handle));
    return GAPI_SUCCESS;
}

GapiResult grid_object_create(uint32_t size,
                              float density,
                              GapiObjectHandle *out_object_handle) {

    size++;
    float total_width = (size - 1) * density;

    uint32_t vertex_count = size * 4;

    Vertex vertices[vertex_count];
    uint32_t indices[vertex_count];
    memset(vertices, 0, vertex_count * sizeof *vertices);
    memset(indices, 0, vertex_count * sizeof *indices);

    for (uint32_t x = 0; x < size; x++) {
        vertices[x * 4 + 0] = (Vertex){.pos = {
                                           .x = x * density,
                                       }};
        vertices[x * 4 + 1] = (Vertex){.pos = {
                                           .x = x * density,
                                           .z = total_width,
                                       }};
        vertices[x * 4 + 2] = (Vertex){.pos = {
                                           .z = x * density,
                                       }};
        vertices[x * 4 + 3] = (Vertex){.pos = {
                                           .x = total_width,
                                           .z = x * density,
                                       }};
    }

    for (uint32_t i = 0; i < vertex_count; i++)
        indices[i] = i;

    Mesh mesh = {
        .vertex_count = vertex_count,
        .index_count = vertex_count,
        .vertices = vertices,
        .indices = indices,
    };

    GapiMeshHandle mesh_handle;
    GAPI_ERR(gapi_mesh_upload(&mesh, &mesh_handle));

    GAPI_ERR(gapi_object_create(mesh_handle, 0, 0, out_object_handle));

    return GAPI_SUCCESS;
}
