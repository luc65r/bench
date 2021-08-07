#ifndef VEC_H
#define VEC_H

#include <stddef.h>

typedef struct {
    void **elems;
    size_t len;
    size_t capacity;
    void (*free)(void *ptr);
} Vec;

/* Allocates an empty `Vec` on the heap
 * `free` is by default the `free` function from `stdlib.h`.
 */
Vec *vec_init();
Vec *vec_init_with_capacity(size_t capacity);
Vec *vec_init_with_free(void (*free)(void *ptr));
Vec *vec_init_with_capacity_and_free(size_t capacity, void (*free)(void *ptr));

void *vec_get(const Vec *vec, size_t index);

/* TODO: error handling */
void vec_push_back(Vec *vec, void *elem);

/* Frees `vec`, and all the elements in it with `free`
 */
void vec_deinit(Vec *vec);

#endif
