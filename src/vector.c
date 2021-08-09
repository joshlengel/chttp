#include"vector.h"

#include<assert.h>
#include<stdlib.h>
#include<string.h>

int vector_create(vector_t *vector, size_t elem_size, size_t capacity)
{
    assert(vector != NULL);

    if (!(vector->arr = calloc(capacity, elem_size))) return -1;
    
    vector->capacity = capacity;
    vector->size = 0;
    vector->elem_size = elem_size;

    return 0;
}

void *vector_get(vector_t *vector, size_t index)
{
    assert(vector != NULL);

    return (char*)vector->arr + index * vector->elem_size;
}

int vector_push_back(vector_t *vector, const void *elem)
{
    assert(vector != NULL);

    if (vector->size >= vector->capacity)
    {
        // Grow vector
        size_t c = vector->capacity + vector->capacity;

        void *tmp = vector->arr;
        vector->arr = realloc(tmp, c * vector->elem_size);

        if (!vector->arr)
        {
            vector->arr = tmp;
            return -1;
        }

        vector->capacity = c;
    }

    memcpy((char*)vector->arr + vector->size++ * vector->elem_size, elem, vector->elem_size);
    return 0;
}

int vector_pop_front(vector_t *vector, void *elem) { return vector_remove(vector, 0, elem); }
int vector_pop_back(vector_t *vector, void *elem) { return vector_remove(vector, vector->size - 1, elem); }

int vector_remove(vector_t *vector, size_t index, void *elem)
{
    assert(vector != NULL);
    assert(vector->size > 0);
    
    if (elem) memcpy(elem, (char*)vector->arr + index * vector->elem_size, vector->elem_size);
    
    const char *src_itr = (char*)vector->arr + (index + 1) * vector->elem_size;
    char *dst_itr = (char*)vector->arr + index * vector->elem_size;

    const size_t num_cpy = vector->size - index - 1;
    for (size_t i = 0; i < num_cpy; ++i)
    {
        memcpy(dst_itr, src_itr, vector->elem_size);
        dst_itr += vector->size;
        src_itr += vector->size;
    }

    --vector->size;

    return 0;
}

int vector_optimize(vector_t *vector)
{
    assert(vector != NULL);

    void *tmp = vector->arr;
    if (!(vector->arr = calloc(vector->size, vector->elem_size)))
    {
        vector->arr = tmp;
        return -1;
    }

    vector->capacity = vector->size;

    memcpy(vector->arr, tmp, vector->size * vector->elem_size);
    free(tmp);

    return 0;
}

void vector_destroy(vector_t *vector)
{
    assert(vector != NULL);
    free(vector->arr);
}