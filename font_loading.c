/* example1.c                                                      */
/*                                                                 */
/* This small program shows how to print a rotated string with the */
/* FreeType 2 library.                                             */

#include "font_loading.h"
#include "log.h"
#include "utility_macros.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define ATLAS_WIDTH 10

static FT_Library library;
static FT_Error ft_error = 0;

#define FT2_ERR(ret)                                                           \
    {                                                                          \
        FT_Error __ret = ret;                                                  \
        if (__ret != 0) {                                                      \
            ft_error = __ret;                                                  \
            return FLD_FT2_ERROR;                                              \
        }                                                                      \
    }

FT_EXPORT(const char *)
FT_Error_String(FT_Error error_code);

FldResult fld_load_file(const char *filepath,
                        uint32_t face_index,
                        uint32_t character_size,
                        Image *out_font_atlas) {

    FT2_ERR(FT_Init_FreeType(&library));

    FT_Face face;
    FT2_ERR(FT_New_Face(library, filepath, 0, &face));

    FT2_ERR(FT_Set_Char_Size(face, 0, 16 * 64, 300, 300));
    FT2_ERR(FT_Set_Pixel_Sizes(face, 0, character_size));

    Image atlas = {
        .pixels = malloc(character_size * ATLAS_WIDTH * ATLAS_WIDTH * 2 *
                         sizeof(uint32_t)),
        .width = character_size * ATLAS_WIDTH,
        .height = character_size * ATLAS_WIDTH,
    };
    if (atlas.pixels == NULL)
        return FLD_SYSTEM_ERROR;

    for (uint8_t c = 32; c < 127; c++) {

        uint32_t glyph_index = FT_Get_Char_Index(face, c);
        FT2_ERR(FT_Load_Glyph(face, glyph_index, 0));
        FT2_ERR(FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL));

        //  TODO: Margin:
        //  INFO("top %u", face->glyph->bitmap_left);

        uint32_t pixel_count =
            face->glyph->bitmap.width * face->glyph->bitmap.rows;
        uint32_t pos_x = ((c - 32) % ATLAS_WIDTH) * character_size;
        uint32_t pos_y = ((c - 32) / ATLAS_WIDTH) * character_size;

        for (uint32_t i = 0; i < pixel_count; i++) {
            uint32_t x = i % character_size;
            uint32_t y = i / character_size;
            uint8_t gray = face->glyph->bitmap.buffer[i];
            uint32_t value = (gray << 24) | (gray << 16) | (gray << 8) | gray;
            atlas.pixels[(pos_x + x) +
                         (pos_y + y) * ATLAS_WIDTH * character_size] = value;
        }
    }

    *out_font_atlas = atlas;
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
