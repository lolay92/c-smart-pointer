# C Smart Pointer Library

A thread-safe implementation of smart pointers in C, providing reference-counted memory management with shared and weak pointer semantics.

## Features

- Shared pointer implementation with automatic reference counting
- Weak pointer support
- Thread-safe operations using atomic counters
- Custom resource destruction support
- Memory leak prevention through automatic cleanup

## Design Structure

```mermaid
classDiagram
    class Controller_t {
        void* resource
        void (*destructor_fn)(void* resource)        
        size_t resource_size
        atomic_size_t weak_count
        atomic_size_t ref_count
    }
    
    class shared_ptr_t {
        Controller_t* control
    }
    
    class weak_ptr_t {
        Controller_t* control
    }

    class Functions {
        fail()
        create_controller()
        init_shared_ptr()
        weak_ptr_create()
        make_shared()
        copy_shared_ptr()
        move_shared_ptr()
        destroy_sp()
        destroy_wp()
    }
    
    shared_ptr_t --> Controller_t : references
    weak_ptr_t --> Controller_t : references
    Functions --> shared_ptr_t : manages
    Functions --> weak_ptr_t : manages

```

## Usage Example with Dynamic Array (Recommended Approach Using make_shared)

```c

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
```

## Dependencies

- C11 or later (for `stdatomic.h`)
- Standard C library

## Best Practices

1. Prefer `make_shared` over `init_shared_ptr` for better performance
2. Implement proper destructor functions for managed resources

## Building

Include `lib.h` and link against `lib.c` in your project. Ensure you're compiling with C11 or later standard support.

```bash
gcc -std=c11 -c lib.c
```


