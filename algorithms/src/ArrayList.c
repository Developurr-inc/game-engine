#include <PAL/types.h>
#include <PAL/memory.h>

#define ARRAY_LIST_INITIAL_CAPACITY  5
#define ARRAY_LIST_GROWTH_FACTOR     2
#define ARRAY_LIST_SHRINK_FACTOR     2
#define ARRAY_LIST_SHRINK_THRESHOLD  4

typedef struct ArrayList {
    void *data;
    uint64 stride;
    uint64 size;
    uint64 capacity;
} ArrayList;

ArrayList *al_create(const uint64 stride) {
    ArrayList *new_array_list = (ArrayList *) pal_memory_alloc(sizeof(ArrayList), true);
    if (new_array_list == NULL) {
        return NULL;
    }

    new_array_list->data = pal_memory_alloc(ARRAY_LIST_INITIAL_CAPACITY * sizeof(stride), true);
    if (new_array_list->data == NULL) {
        pal_memory_free(new_array_list, true);
        return NULL;
    }

    new_array_list->capacity = ARRAY_LIST_INITIAL_CAPACITY;
    new_array_list->stride   = stride;
    new_array_list->size     = 0;

    return new_array_list;
}

void al_destroy(ArrayList *array_list) {
    if (array_list == NULL) {
        return;
    }

    pal_memory_free(array_list->data, true);
    pal_memory_free(array_list, true);

    array_list = NULL;
}

void al_clear(ArrayList *array_list) {
    if (array_list == NULL) {
        return;
    }

    array_list->size = 0;
}

bool1 al_add(ArrayList *array_list, const void *element) {
    if (array_list == NULL || element == NULL) {
        return false;
    }

    if (! al_grow(array_list)) {
        return false;
    }

    if (pal_memory_copy(&array_list->data[array_list->size], element, array_list->stride) == NULL) {
        return false;
    }

    if (! pal_memory_cmp(&array_list->data[array_list->size], element)) {
        return false;
    }

    array_list->size++;

    return true;
}

void *al_get(const ArrayList *array_list, const uint64 index) {
    if (array_list == NULL || index >= array_list->size) {
        return NULL;
    }

    return &array_list->data[index];
}

void *al_remove(ArrayList *array_list, const uint64 index) {
    if (array_list == NULL || index >= array_list->size) {
        return NULL;
    }

    void *element = &array_list->data[index];

    // maybe we lost the data at index position
    if (pal_memory_copy(&array_list->data[index], &array_list->data[index + 1], (array_list->size - index - 1) * array_list->stride) == NULL) {
        return NULL;
    }

    array_list->size--;

    if (! al_shrink(array_list)) {
        return NULL;
    }

    return element;
}

bool1 al_grow(ArrayList *array_list) {
    if (array_list == NULL) {
        return false;
    }

    if (array_list->size == array_list->capacity) {
        return al_resize(array_list, array_list->capacity * ARRAY_LIST_GROWTH_FACTOR);
    }

    return true;
}

bool1 al_shrink(ArrayList *array_list) {
    if (array_list == NULL) {
        return false;
    }

    if (array_list->size <= array_list->capacity / ARRAY_LIST_SHRINK_THRESHOLD) {
        return al_resize(array_list, array_list->capacity / ARRAY_LIST_SHRINK_FACTOR);
    }

    return true;
}

bool1 al_resize(ArrayList *array_list, uint64 new_capacity) {
    if (new_capacity < ARRAY_LIST_INITIAL_CAPACITY) {
        new_capacity = ARRAY_LIST_INITIAL_CAPACITY;
    }

    if (new_capacity == array_list->capacity) {
        return true;
    }

    void *new_data = pal_memory_alloc(new_capacity * array_list->stride, true);
    if (new_data == NULL) {
        return false;
    }

    if (pal_memory_copy(new_data, array_list->data, array_list->size * array_list->stride) == NULL) {
        pal_memory_free(new_data, true);
        return false;
    }

    pal_memory_free(array_list->data, true);

    array_list->data     = new_data;
    array_list->capacity = new_capacity;

    return true;
}

uint64 al_size(const ArrayList *array_list) {
    if (array_list == NULL) {
        return 0;
    }

    return array_list->size;
}

bool1 al_is_empty(const ArrayList *array_list) {
    if (array_list == NULL) {
        return true;
    }

    return array_list->size == 0;
}

bool1 al_contains(const ArrayList *array_list, const void *element) {
    if (array_list == NULL || element == NULL) {
        return false;
    }

    for (uint64 i = 0; i < array_list->size; i++) {
        if (pal_memory_cmp(&array_list->data[i], element)) {
            return true;
        }
    }

    return false;
}

bool1 al_is_equal(const ArrayList *array_list1, const ArrayList *array_list2) {
    if (array_list1 == NULL || array_list2 == NULL) {
        return false;
    }

    if (array_list1->size != array_list2->size) {
        return false;
    }

    for (uint64 i = 0; i < array_list1->size; i++) {
        if (! pal_memory_cmp(&array_list1->data[i], &array_list2->data[i])) {
            return false;
        }
    }

    return true;
}

bool1 al_for_each(const ArrayList *array_list, void (*callback)(void *)) {
    if (array_list == NULL || callback == NULL) {
        return false;
    }

    for (uint64 i = 0; i < array_list->size; i++) {
        callback(&array_list->data[i]);
    }

    return true;
}

bool1 al_map(const ArrayList *array_list, void *(*callback)(void *)) {
    if (array_list == NULL || callback == NULL) {
        return false;
    }

    for (uint64 i = 0; i < array_list->size; i++) {
        void *element = callback(&array_list->data[i]);
        if (element == NULL) {
            return false;
        }

        if (pal_memory_copy(&array_list->data[i], element, array_list->stride) == NULL) {
            return false;
        }
    }

    return true;
}

bool1 al_filter(const ArrayList *array_list, bool1 (*callback)(void *)) {
    if (array_list == NULL || callback == NULL) {
        return false;
    }

    uint64 j = 0;
    for (uint64 i = 0; i < array_list->size; i++) {
        if (callback(&array_list->data[i])) {
            if (pal_memory_copy(&array_list->data[j], &array_list->data[i], array_list->stride) == NULL) {
                return false;
            }

            j++;
        }
    }

    array_list->size = j;

    if (! al_shrink(array_list)) {
        return false;
    }

    return true;
}

bool1 al_reduce(const ArrayList *array_list, void *initial_value, void *(*callback)(void *, void *)) {
    if (array_list == NULL || callback == NULL) {
        return false;
    }

    void *accumulator = initial_value;
    for (uint64 i = 0; i < array_list->size; i++) {
        accumulator = callback(accumulator, &array_list->data[i]);
        if (accumulator == NULL) {
            return false;
        }
    }

    return true;
}

bool1 al_sort(ArrayList *array_list, int32 (*comparator)(const void *, const void *)) {
    if (array_list == NULL || comparator == NULL) {
        return false;
    }

    qsort(array_list->data, array_list->size, array_list->stride, comparator);

    return true;
}

bool1 al_reverse(ArrayList *array_list) {
    if (array_list == NULL) {
        return false;
    }

    for (uint64 i = 0; i < array_list->size / 2; i++) {
        void *temp = &array_list->data[i];
        if (pal_memory_copy(&array_list->data[i], &array_list->data[array_list->size - i - 1], array_list->stride) == NULL) {
            return false;
        }

        if (pal_memory_copy(&array_list->data[array_list->size - i - 1], temp, array_list->stride) == NULL) {
            return false;
        }
    }

    return true;
}

bool1 al_swap(ArrayList *array_list, const uint64 index1, const uint64 index2) {
    if (array_list == NULL || index1 >= array_list->size || index2 >= array_list->size) {
        return false;
    }

    void *temp = &array_list->data[index1];
    if (pal_memory_copy(&array_list->data[index1], &array_list->data[index2], array_list->stride) == NULL) {
        return false;
    }

    if (pal_memory_copy(&array_list->data[index2], temp, array_list->stride) == NULL) {
        return false;
    }

    return true;
}

bool1 al_copy(const ArrayList *array_list, ArrayList *new_array_list) {
    if (array_list == NULL || new_array_list == NULL) {
        return false;
    }

    if (new_array_list->capacity < array_list->size) {
        if (! al_resize(new_array_list, array_list->size)) {
            return false;
        }
    }

    if (pal_memory_copy(new_array_list->data, array_list->data, array_list->size * array_list->stride) == NULL) {
        return false;
    }

    new_array_list->size = array_list->size;

    return true;
}

