#include <stdio.h>
#include <stdlib.h>

typedef void (*destructor_fn)(void*);

typedef struct Control {
	void* data;
	destructor_fn destructor;
	size_t ref_count;
	size_t data_size;
} control_t;

typedef struct SharedPtr {
	control_t* controller;
} shared_ptr_t;

typedef spinlock_t; 
 
control_t* create_controller(void* data, size_t data_size, destructor_fn destructor);
shared_ptr_t* init_shared_ptr();
shared_ptr_t* create_shared_ptr(void* data, size_t data_size, destructor_fn destructor);
void safe_increment_ref_count(spinlock_t* spinlock, size_t ref_count);
shared_ptr_t* copy_shared_ptr(shared_ptr_t* src);
shared_ptr_t* move_shared_ptr(shared_ptr_t* src);
shared_ptr_t* make_shared(void* data, size_t size);
void destroy_shared_ptr(shared_ptr_t*);
