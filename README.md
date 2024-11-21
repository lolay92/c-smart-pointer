# Thread-Safe Smart Pointer Implementation in C

A lightweight implementation of reference-counted smart pointers with thread safety, similar to C++'s `std::shared_ptr` and `std::weak_ptr`.

## Features

- Thread-safe reference counting using spinlocks
- Memory-efficient single allocation with `make_shared`
- Support for custom destructors
- Weak pointer support to break reference cycles
- Optimized struct layout for better memory usage

## Design Structure

```mermaid
classDiagram
    class control_block_t {
        size_t shared_count
        size_t weak_count
        atomic_flag lock
        void* ptr
        void (*destructor)(void*)
        size_t size
    }
    
    class shared_ptr_t {
        control_block_t* control
    }
    
    class weak_ptr_t {
        control_block_t* control
    }

    class Functions {
        make_shared()
        shared_ptr_destroy()
        weak_ptr_create()
        weak_ptr_destroy()
        weak_ptr_lock()
    }
    
    shared_ptr_t --> control_block_t : references
    weak_ptr_t --> control_block_t : references
    Functions --> shared_ptr_t : manages
    Functions --> weak_ptr_t : manages

```

## Usage Example

```c
// Create a shared pointer
shared_ptr_t* sp = make_shared(sizeof(Resource), resource_destructor);

// Create a weak reference
weak_ptr_t* wp = weak_ptr_create(sp);

// Clean up
shared_ptr_destroy(sp);
weak_ptr_destroy(wp);
```

## Building

```bash
make
```

## Notes

- Uses C11 features (`stdatomic.h`)
- Optimized for single-allocation pattern with `make_shared`
- Thread-safe operations using spinlocks
- Memory efficient control block design

## License

