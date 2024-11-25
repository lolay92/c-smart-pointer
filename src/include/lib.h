#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <errno.h>

typedef void (*res_destructor_fn)(void* resource);

typedef struct Controller {
	void* resource;
	res_destructor_fn destructor;
	atomic_size_t ref_count;
	size_t resource_size;
} controller_t;

typedef struct SharedPtr {
	controller_t* controller;
} shared_ptr_t;

void fail(const char* message); 
controller_t* create_controller(size_t resource_size, res_destructor_fn destructor);
shared_ptr_t* init_shared_ptr(size_t resource_size, res_destructor_fn destructor);
shared_ptr_t* make_shared(size_t resource_size, res_destructor_fn destructor);
shared_ptr_t* copy_shared_ptr(shared_ptr_t* src);
shared_ptr_t* move_shared_ptr(shared_ptr_t* src);
void destroy_shared_ptr(shared_ptr_t*);
