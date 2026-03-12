#include "font_loading.h"
#include "log.h"
#include "utility_macros.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef DEBUG
#define FT2_ERR(ret)                                                           \
    {                                                                          \
        FT_Error __ret = ret;                                                  \
        if (__ret != 0) {                                                      \
            ft_error = __ret;                                                  \
            ERROR("%s: %s", STR(ret), FT_Error_String(__ret));                 \
            return FLD_FT2_ERROR;                                              \
        }                                                                      \
    }
#else
#define FT2_ERR(ret)                                                           \
    {                                                                          \
        FT_Error __ret = ret;                                                  \
        if (__ret != 0) {                                                      \
            ft_error = __ret;                                                  \
            return FLD_FT2_ERROR;                                              \
        }                                                                      \
    }
#endif

#define FIRST_CHAR 32
#define LAST_CHAR 127
#define NUM_CHARS (LAST_CHAR - FIRST_CHAR)

static FT_Library library;
static FT_Error ft_error = 0;

FT_EXPORT(const char *)
FT_Error_String(FT_Error error_code);

FldResult fld_load_file(const char *filepath,
                        uint32_t face_index,
                        uint32_t character_size,
                        Font *out_font) {

    FT2_ERR(FT_Init_FreeType(&library));

    FT_Face face;
    FT2_ERR(FT_New_Face(library, filepath, face_index, &face));

    FT2_ERR(FT_Set_Char_Size(face, 0, character_size, 300, 300));
    FT2_ERR(FT_Set_Pixel_Sizes(face, 0, character_size));

    uint32_t glyph_cell_size = face->size->metrics.height / 64 + 1;

    uint32_t dimension = 1;
    while (dimension < glyph_cell_size * ceilf(sqrtf(NUM_CHARS)))
        dimension *= 2;

    *out_font = (Font){
        .atlas =
            {
                .pixels = calloc(dimension * dimension, sizeof(uint32_t)),
                .width = dimension,
                .height = dimension,
            },
        .metadata =
            {
                .first_char = FIRST_CHAR,
                .last_char = LAST_CHAR,
                .cell_size = glyph_cell_size,
                .atlas_dimension = dimension,
                .glyph_cell_size = glyph_cell_size,
            },

    };
    if (out_font->atlas.pixels == NULL)
        return FLD_SYSTEM_ERROR;

    uint32_t pen_x = 0;
    uint32_t pen_y = 0;

    for (uint8_t c = FIRST_CHAR; c < LAST_CHAR; c++) {

        uint32_t glyph_index = FT_Get_Char_Index(face, c);
        FT2_ERR(FT_Load_Glyph(face, glyph_index, 0));
        FT2_ERR(FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL));

        FT_Bitmap *bitmap = &face->glyph->bitmap;

        if (pen_x + bitmap->width >= dimension) {
            pen_x = 0;
            pen_y += glyph_cell_size;
        }

        out_font->metadata.glyph_infos[c - FIRST_CHAR] = (GlyphInfo){
            .offset_left = face->glyph->bitmap_left,
            .offset_top = glyph_cell_size - face->glyph->bitmap_top,
            .x0 = pen_x,
            .y0 = pen_y,
            .x1 = pen_x + bitmap->width,
            .y1 = pen_y + bitmap->rows,
            .advance = face->glyph->advance.x / 64,
        };

        uint32_t pixel_count = bitmap->width * bitmap->rows;

        for (uint32_t i = 0; i < pixel_count; i++) {

            uint8_t gray = bitmap->buffer[i];
            uint32_t value = (gray << 24) | (gray << 16) | (gray << 8) | gray;

            uint32_t x = i % bitmap->width;
            uint32_t y = i / bitmap->width;

            uint32_t atlas_i = pen_x + x + (pen_y + y) * dimension;
            out_font->atlas.pixels[atlas_i] = value;
        }
        pen_x += bitmap->width + 1;
    }

    return FLD_SUCCESS;
}

const char *fld_strerror(FldResult result) {

    switch (result) {

    case FLD_SUCCESS:
        return "Success";
    case FLD_FT2_ERROR:
        return FT_Error_String(ft_error);
    case FLD_SYSTEM_ERROR:
        return strerror(errno);
    }

    return "";
}
