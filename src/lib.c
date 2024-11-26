#include "include/lib.h"

void fail(const char* message) {
    if (errno) {
        perror(message); 
    } else {
        printf("ERROR: ‰s\n", message); 
    }
    exit(1); 
}

controller_t* create_controller(size_t resource_size, res_destructor_fn destructor){
    controller_t* controller = malloc(sizeof(controller_t));
    if(!controller){ fail("Failed to allocate memory for controller creation!"); }

    controller->resource = malloc(resource_size);
    if(!controller->resource){ fail("Failed to allocate memory for resource initialization!"); }

    controller->destructor = destructor;
    controller->resource_size = resource_size;
    atomic_fetch_add(&(controller->ref_count), 1);

    return controller;
}

shared_ptr_t* init_shared_ptr(size_t resource_size, res_destructor_fn destructor){
    controller_t* controller = create_controller(resource_size, destructor);
    shared_ptr_t* sp = malloc(sizeof(shared_ptr_t));
    if(!sp){ fail("Failed to allocate memory for shared_ptr initialization!"); }
    sp->controller = controller;

    return sp;
}

shared_ptr_t* make_shared(size_t resource_size, res_destructor_fn destructor){
    controller_t* controller = malloc(sizeof(controller_t) + resource_size); 
    if(!controller){ fail("Failed to allocate memory for controller + shared_ptr creation!"); }

    controller->resource = controller + sizeof(controller_t); 
    atomic_fetch_add(&(controller->ref_count), 1);
    controller->destructor = destructor; 
    controller->resource_size = resource_size; 

    shared_ptr_t* sp = malloc(sizeof(shared_ptr_t));
    if(!sp){ fail("Failed to allocate memory for shared_ptr creation!"); }

    sp->controller = controller; 

    return sp; 
}

shared_ptr_t* copy_shared_ptr(shared_ptr_t* src){
    shared_ptr_t* copy = malloc(sizeof(shared_ptr_t));
    if(!copy){ fail("Failed to allocate memory for shared_ptr initialization!"); }

    if(src->controller){
        copy->controller = src->controller;
        atomic_fetch_add(&(src->controller->ref_count), 1);
    };

    return copy; 
}

shared_ptr_t* move_shared_ptr(shared_ptr_t* src){
    shared_ptr_t* dest = malloc(sizeof(shared_ptr_t));
    if(!dest){
        perror("Failed to allocate memory for shared_ptr initialization!");
        return NULL;
    }

    if(src->controller){
        dest->controller = src->controller;
        atomic_fetch_add(&(src->controller->ref_count), 1);
    }

    destroy_shared_ptr(src);
    return dest; 
}

void destroy_sp(shared_ptr_t* sp){
    if(!sp || !(sp->controller) || !(sp->controller->resource)) {
        perror("Unsafe memory access during shared pointer destruction!");
        return; 
    }
    atomic_fetch_sub(&sp->controller->ref_count, 1);
    
    if(atomic_load(&(sp->controller->ref_count)) == 0) { 
        if(sp->controller->destructor) {
            sp->controller->destructor(sp->controller->resource);
        } 
        free(sp->controller);
    } else {
        sp->controller = NULL; 
    }
    free(sp);
}

weak_ptr_t* weak_ptr_create(shared_ptr_t* sp) {
    weak_ptr_t* wp = malloc(sizeof(weak_ptr_t));
    if(!wp) { fail("Failed to allocate memory for weak_ptr initialization!"); }
    if(sp) {
        wp->controller = sp->controller;
        atomic_fetch_add(&(sp->controller->weak_count), 1);
    } else { return; }

    return wp; 
}

void destroy_wp(weak_ptr_t* wp) {
    if(wp) {
        atomic_fetch_sub(&(wp->controller->weak_count), 1);
        wp->controller = NULL; 
        free(wp); 
    }
}

