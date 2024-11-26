#include "include/lib.h"

typedef struct Array {
    int* data;
    size_t size;
} Array;

void array_destructor(void* resource) {
    Array* arr = (Array*)resource;
    free(arr->data);
}

int main() {
    shared_ptr_t* array_sp = make_shared(sizeof(Array), array_destructor);

    Array* arr = (Array*)array_sp->controller->resource;
    arr->size = 100;
    arr->data = malloc(arr->size * sizeof(int));
    
    for (size_t i = 0; i < arr->size; i++) {
        arr->data[i] = i;
    }

    shared_ptr_t* worker_ref = copy_shared_ptr(array_sp);

    destroy_sp(array_sp);
    destroy_sp(worker_ref);

    return 0;
}