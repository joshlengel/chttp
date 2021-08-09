#pragma once

#include<stddef.h>

typedef struct
{
    size_t capacity, size;
    size_t elem_size;
    void *arr;
} vector_t;

int vector_create(vector_t *vector, size_t elem_size, size_t capacity);
void *vector_get(vector_t *vector, size_t index);
int vector_push_back(vector_t *vector, const void *elem);
int vector_pop_front(vector_t *vector, void *elem);
int vector_pop_back(vector_t *vector, void *elem);
int vector_remove(vector_t *vector, size_t index, void *elem);
int vector_optimize(vector_t *vector);
void vector_destroy(vector_t *vector);