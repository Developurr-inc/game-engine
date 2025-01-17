//
// Created by Vinícius Ferreira Aguiar on 13/01/25.
//

#ifndef NAYLA_DYNAMIC_ARRAY_H
#define NAYLA_DYNAMIC_ARRAY_H

#include <NAYLA/types.h>
#include <NAYLA/export.h>

#define DARRAY_DEFAULT_CAPACITY  1
#define DARRAY_RESIZE_FACTOR     2

#define darray_create(type)  _darray_create(DARRAY_DEFAULT_CAPACITY, sizeof(type))

#define darray_reserve(type, capacity)  _darray_create(capacity, sizeof(type))

#define darray_destroy(darray)  _darray_destroy(darray)

#define darray_push(darray, element)               \
    {                                              \
        typeof(element) _element = element;        \
        darray = _darray_push(darray, &_element);  \
    }

#define darray_pop(darray)  _darray_pop(darray)

#define darray_pop_at(darray, index)  _darray_pop_at(darray, index)

#define darray_insert_at(darray, index, element)               \
    {                                                          \
        typeof(element) _element = element;                    \
        darray = _darray_insert_at(darray, index, &_element);  \
    }

#define darray_clear(darray)  _darray_field_set(darray, DARRAY_LENGTH, 0)

#define darray_capacity(darray)  _darray_field_get(darray, DARRAY_CAPACITY)

#define darray_length(darray)  _darray_field_get(darray, DARRAY_LENGTH)

#define darray_stride(darray)  _darray_field_get(darray, DARRAY_STRIDE)

#define darray_length_set(darray, length)  _darray_field_set(darray, DARRAY_LENGTH, length)

enum {
  DARRAY_CAPACITY,
  DARRAY_LENGTH,
  DARRAY_STRIDE,
  DARRAY_FIELD_LENGTH
};

N_EXPORT void *_darray_create (uint64 length, uint64 stride);
N_EXPORT void  _darray_destroy(void *darray);

N_EXPORT uint64 _darray_field_get(void *darray, uint64 field);
N_EXPORT void   _darray_field_set(void *darray, uint64 field, uint64 value);
N_EXPORT void * _darray_resize   (void *darray, uint64 new_length);

N_EXPORT void *_darray_push(void *darray, const void *element);
N_EXPORT void *_darray_pop (void *darray);

N_EXPORT void *_darray_pop_at   (void *darray, uint64 index);
N_EXPORT void *_darray_insert_at(void *darray, uint64 index, const void *element);

#endif //NAYLA_DYNAMIC_ARRAY_H
