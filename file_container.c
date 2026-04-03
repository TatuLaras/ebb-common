#include "file_container.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAGIC 0x8d
#define VERSION 1
#define MIN_HEADER_SIZE 8

typedef struct {
    uint8_t magic;
    uint8_t specific_magic;
    uint16_t version;
    uint32_t header_size;
} FcHeader;

typedef struct {
    uint16_t kind;
    uint16_t extra_info_size;
    uint32_t block_size;
    uint32_t count;
} FcElement;

#define ERR(ret)                                                               \
    if ((ret) < 0)                                                             \
    return FC_SYSTEM_ERROR

FcResult fc_write_header(int fd, uint8_t magic) {

    FcHeader header = {
        .magic = MAGIC,
        .specific_magic = magic,
        .version = VERSION,
        .header_size = sizeof(FcHeader),
    };
    ERR(write(fd, &header, sizeof header));
    return FC_SUCCESS;
}

FcResult fc_write_blocks(int fd,
                         uint32_t kind,
                         uint32_t block_count,
                         uint32_t block_size,
                         void *data,
                         uint32_t extra_info_size,
                         void *extra_info) {

    FcElement element = {.kind = kind,
                         .block_size = block_size,
                         .count = block_count,
                         .extra_info_size = extra_info_size};
    ERR(write(fd, &element, sizeof element));
    if (extra_info_size > 0)
        ERR(write(fd, extra_info, extra_info_size));
    ERR(write(fd, data, block_size * block_count));

    return FC_SUCCESS;
}

FcResult fc_read(int fd, uint8_t expected_magic, FcElementFn element_callback) {

    struct stat file_info;
    ERR(fstat(fd, &file_info));

    uint32_t filesize = file_info.st_size;
    char *file_buf = malloc(filesize);
    if (file_buf == NULL)
        return -1;

    ERR(read(fd, file_buf, filesize));

    FcHeader header = {0};
    memcpy(&header, file_buf, MIN_HEADER_SIZE);
    if (header.magic != MAGIC || header.specific_magic != expected_magic)
        return FC_WRONG_MAGIC;
    if (header.header_size < MIN_HEADER_SIZE)
        return FC_MANGLED_FORMAT;
    if (header.version > VERSION)
        return FC_VERSION_UNSUPPORTED;

    memcpy(&header, file_buf, header.header_size);

    uint32_t cursor = header.header_size;

    while (cursor < filesize) {

        FcElement element = {0};
        if (filesize - cursor < sizeof(FcElement))
            return FC_MANGLED_FORMAT;

        memcpy(&element, file_buf + cursor, sizeof(FcElement));
        cursor += sizeof(FcElement);

        if (filesize - cursor < element.count * element.block_size)
            return FC_MANGLED_FORMAT;

        element_callback(element.kind,
                         element.count,
                         element.block_size,
                         file_buf + cursor + element.extra_info_size,
                         element.extra_info_size,
                         file_buf + cursor);
        cursor += element.count * element.block_size;
    }

    return FC_SUCCESS;
}

char *fc_strerror(FcResult result) {

    switch (result) {

    case FC_SUCCESS:
        return "Success";
    case FC_SYSTEM_ERROR:
        return strerror(errno);
    case FC_WRONG_MAGIC:
        return "Wrong magic number";
    case FC_MANGLED_FORMAT:
        return "Data in the file is malformed";
    case FC_VERSION_UNSUPPORTED:
        return "The version of the file is newer than what this program can "
               "parse";
    }
}
