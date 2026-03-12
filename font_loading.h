#ifndef _FONT_LOADING
#define _FONT_LOADING

#include "types.h"

#define FLD_ERR_MSG(result, message)                                           \
    {                                                                          \
        FldResult __res = result;                                              \
        if (__res != FLD_SUCCESS) {                                            \
            ERROR(message ": %s", fld_strerror(__res));                        \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \
    }
#define FLD_ERR(result) FLD_ERR_MSG(result, STR(result))

typedef enum {
    FLD_SUCCESS = 0,
    FLD_SYSTEM_ERROR,
    FLD_FT2_ERROR,
} FldResult;

FldResult fld_load_file(const char *filepath,
                        uint32_t face_index,
                        uint32_t character_size,
                        Font *out_font);

const char *fld_strerror(FldResult result);

#endif
