#ifndef _SMART_PTR_H
#define _SMART_PTR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <stdbool.h>

typedef void (*res_destructor_fn)(void* resource);

typedef struct Controller {
	void* resource;
	size_t resource_size;
	atomic_size_t ref_count;
	atomic_size_t weak_count;
	res_destructor_fn destructor;
	bool single_malloc; 
} controller_t;

typedef struct SharedPtr {
	controller_t* controller;
} shared_ptr_t;

typedef struct WeakPtr {
	const controller_t* controller;
} weak_ptr_t;

controller_t* create_controller(size_t resource_size, res_destructor_fn destructor);
shared_ptr_t* init_shared_ptr(size_t resource_size, res_destructor_fn destructor);
weak_ptr_t* weak_ptr_create(const shared_ptr_t* const sp);
shared_ptr_t* make_shared(size_t resource_size, res_destructor_fn destructor);
shared_ptr_t* copy_shared_ptr(const shared_ptr_t* const src);
shared_ptr_t* move_shared_ptr(shared_ptr_t* src);
void destroy_sp(shared_ptr_t*);
void destroy_wp(weak_ptr_t*);
const void* get_raw_ptr(shared_ptr_t* sp);

#endif