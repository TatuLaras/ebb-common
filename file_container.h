#ifndef _FILE_CONTAINER
#define _FILE_CONTAINER

// A binary file format for storing various data like rooms, scenes, savefiles..

#include <stdint.h>

typedef enum {
    FC_SUCCESS = 0,
    FC_SYSTEM_ERROR,
    FC_WRONG_MAGIC,
    FC_MANGLED_FORMAT,
    FC_VERSION_UNSUPPORTED,
} FcResult;

typedef void (*FcElementFn)(uint32_t kind,
                            uint32_t block_count,
                            uint32_t block_size,
                            void *data,
                            uint32_t extra_info_size,
                            void *extra_info);

FcResult fc_write_header(int fd, uint8_t magic);
FcResult fc_write_blocks(int fd,
                         uint32_t kind,
                         uint32_t block_count,
                         uint32_t block_size,
                         void *data,
                         uint32_t extra_info_size,
                         void *extra_info);

FcResult fc_read(int fd, uint8_t expected_magic, FcElementFn element_callback);

char *fc_strerror(FcResult result);

#endif
