#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "vec.h"

Vec *vec_init() {
    return vec_init_with_capacity(0);
}

Vec *vec_init_with_capacity(size_t capacity) {
    return vec_init_with_capacity_and_free(capacity, &free);
}

Vec *vec_init_with_free(void (*free)(void *ptr)) {
    return vec_init_with_capacity_and_free(0, free);
}

Vec *vec_init_with_capacity_and_free(size_t capacity, void (*free)(void *ptr)) {
    Vec *vec = malloc(sizeof *vec);
    if (vec == NULL)
        goto err;

    vec->elems = calloc(capacity, sizeof *vec->elems);
    if (vec->elems == NULL)
        goto err;

    vec->len = 0;
    vec->capacity = capacity;
    vec->free = free;

    return vec;

err:
    if (vec != NULL) {
        if (vec->elems != NULL) {
            free(vec->elems);
        }
        free(vec);
    }
    return NULL;
}

void *vec_get(const Vec *vec, size_t index) {
    assert(vec != NULL);

    if (index >= vec->len)
        return NULL;

    return vec->elems[index];
}

/* TODO: error handling */
static void vec_realloc_if_needed(Vec *vec, size_t elems) {
    assert(vec->len <= vec->capacity);

    size_t new_capacity = vec->capacity;

    if (new_capacity == 0 && elems > 0)
        new_capacity = 1;

    while (vec->len + elems > new_capacity)
        new_capacity *= 2;

    if (new_capacity > vec->capacity) {
        vec->elems = realloc(vec->elems, sizeof *vec->elems * new_capacity);
        vec->capacity = new_capacity;
    }
}

void vec_push_back(Vec *vec, void *elem) {
    assert(vec != NULL);

    vec_realloc_if_needed(vec, 1);
    vec->elems[vec->len++] = elem;
}

void vec_deinit(Vec *vec) {
    assert(vec != NULL);

    for (size_t i = 0; i < vec->len; i++) {
        vec->free(vec->elems[i]);
    }
    free(vec->elems);

    free(vec);
}
