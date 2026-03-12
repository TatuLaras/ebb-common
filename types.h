#ifndef _TYPES
#define _TYPES

#include "cglm/types-struct.h"
#include <stdint.h>

#define FONT_ATLAS_DIMENSION_CHARS 10

typedef struct {
    vec3s pos;
    vec3s color;
    vec3s normal;
    vec2s uv;
} Vertex;

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
} Rect2D;

typedef struct {
    uint32_t vertex_count;
    uint32_t index_count;
    Vertex *vertices;
    uint32_t *indices;
} Mesh;

typedef struct {
    uint32_t *pixels;
    uint32_t width;
    uint32_t height;
} Image;

typedef struct {
    uint16_t x0;
    uint16_t y0;
    uint16_t x1;
    uint16_t y1;
    int16_t offset_top;
    int16_t offset_left;
    uint16_t advance;
} GlyphInfo;

typedef struct {
    uint8_t first_char;
    uint8_t last_char;
    uint16_t cell_size;
    uint32_t atlas_dimension;
    uint16_t glyph_cell_size;
    GlyphInfo
        glyph_infos[FONT_ATLAS_DIMENSION_CHARS * FONT_ATLAS_DIMENSION_CHARS];
} FontMetadata;

typedef struct {
    FontMetadata metadata;
    Image atlas;
} Font;

typedef struct {
    uint8_t r;
    uint8_t b;
    uint8_t g;
    uint8_t a;
} Color;

#endif
