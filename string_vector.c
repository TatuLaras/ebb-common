#include "string_vector.h"
#include "utility_macros.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define STARTING_SIZE_CHARACTERS 200
#define STARTING_SIZE_INDICES 4
#define GROWTH_FACTOR 2

StringVector stringvec_init(void) {
    StringVector vec = {
        .data = malloc(STARTING_SIZE_CHARACTERS),
        .data_allocated = STARTING_SIZE_CHARACTERS,
        .indices = malloc(STARTING_SIZE_INDICES * sizeof(size_t)),
        .indices_allocated = STARTING_SIZE_INDICES,
    };
    return vec;
}

void stringvec_append(StringVector *vec, char *buffer, size_t buffer_size) {
    assert(vec->data);
    assert(vec->indices);

    while (vec->data_used + buffer_size >= vec->data_allocated) {
        // Grow char buffer
        vec->data_allocated *= GROWTH_FACTOR;
        vec->data = realloc(vec->data, vec->data_allocated);
        assert(vec->data);
    }
    if (vec->indices_used >= vec->indices_allocated) {
        // Grow pointer buffer
        vec->indices_allocated *= GROWTH_FACTOR;
        vec->indices =
            realloc(vec->indices, vec->indices_allocated * sizeof(char *));
        assert(vec->indices);
    }

    size_t index = vec->data_used;
    char *pointer = vec->data + index;
    memcpy(pointer, buffer, buffer_size);
    vec->data_used += buffer_size;

    // Extra null terminator to be safe
    vec->data[vec->data_used++] = 0;

    vec->indices[vec->indices_used++] = index;
}

char *stringvec_get(StringVector *vec, size_t index) {
    assert(vec->data);
    assert(vec->indices);

    if (index >= vec->indices_used)
        return 0;

    return vec->data + vec->indices[index];
}

void stringvec_free(StringVector *vec) {
    if (vec->data) {
        free(vec->data);
        vec->data = 0;
    }
    if (vec->indices) {
        free(vec->indices);
        vec->indices = 0;
    }
}

size_t stringvec_count(StringVector *vec) {
    assert(vec->data);
    assert(vec->indices);

    return vec->indices_used;
}

void stringvec_as_newline_separated(StringVector *vec,
                                    char *out_buffer,
                                    size_t out_buffer_size,
                                    int max_string_count) {
    assert(vec->data);
    assert(vec->indices);

    size_t limit = MIN(out_buffer_size, vec->data_used);

    if (limit == 0) {
        return;
    }

    memcpy(out_buffer, vec->data, limit);
    int strings = 0;

    for (size_t i = 0; i < limit - 1; i++) {
        if (out_buffer[i] == 0) {
            strings++;
            if (strings == max_string_count && max_string_count != -1)
                out_buffer[i] = 0;
            else
                out_buffer[i] = '\n';
        }
    }

    out_buffer[limit - 1] = 0;
}

StringVector stringvec_clone(StringVector *vec) {
    StringVector new_vec = {
        .data = malloc(vec->data_allocated),
        .data_allocated = vec->data_allocated,
        .data_used = vec->data_used,
        .indices = malloc(vec->indices_allocated * sizeof(char *)),
        .indices_allocated = vec->indices_allocated,
        .indices_used = vec->indices_used,
    };
    memcpy(new_vec.data, vec->data, vec->data_allocated);
    memcpy(
        new_vec.indices, vec->indices, vec->indices_allocated * sizeof(char *));
    return new_vec;
}

void stringvec_truncate(StringVector *vec) {
    vec->data_used = 0;
    vec->indices_used = 0;
}

int64_t stringvec_index_of(StringVector *vec, const char *string) {
    size_t i = 0;
    char *candidate = 0;
    while ((candidate = stringvec_get(vec, i++))) {
        if (!strcmp(candidate, string))
            return i - 1;
    }

    return -1;
}
