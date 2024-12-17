#include "smart_ptr.h"
#include "debug.h"

controller_t* create_controller(size_t resource_size, res_destructor_fn destructor){
    controller_t* controller = malloc(sizeof(controller_t));
    check_mem(controller);

    controller->resource = malloc(resource_size);
    check_mem(controller->resource);

    controller->destructor = destructor;
    controller->resource_size = resource_size;
    atomic_init(&(controller->ref_count), 1);
    atomic_init(&(controller->weak_count), 0);

    return controller;

    error:
        if(controller) free(controller); 
        return NULL; 
}

shared_ptr_t* init_shared_ptr(size_t resource_size, res_destructor_fn destructor){
    controller_t* controller = create_controller(resource_size, destructor);
    controller->single_malloc=false;

    shared_ptr_t* sp = malloc(sizeof(shared_ptr_t));
    check_mem(sp);
    sp->controller = controller;

    return sp;

    error:
        log_err("Error during shared pointer initialization!");
        return NULL;
}

shared_ptr_t* make_shared(size_t resource_size, res_destructor_fn destructor){
    controller_t* controller = malloc(sizeof(controller_t) + resource_size); 
    check_mem(controller);

    controller->single_malloc=true; 
    controller->resource = (void*)((char*)controller + sizeof(controller_t));
    check_mem(controller->resource); 

    atomic_init(&(controller->ref_count), 1);
    atomic_init(&(controller->weak_count), 0); 
    controller->destructor = destructor; 
    controller->resource_size = resource_size; 

    shared_ptr_t* sp = malloc(sizeof(shared_ptr_t));
    check_mem(sp); 

    sp->controller = controller;

    return sp;

    error:
        log_err("Error during shared pointer creation!");
        if(controller) {
            if(controller->destructor) {
                controller->destructor(controller->resource);
            }
            free(controller);
        }
        return NULL;
}

shared_ptr_t* copy_shared_ptr(const shared_ptr_t* const src){
    shared_ptr_t* copy = malloc(sizeof(shared_ptr_t));
    check_mem(copy);
    check(src, "Invalid shared pointer state!"); 
    check(src->controller, "Invalid controller of shared pointer state!"); 

    copy->controller = src->controller;
    atomic_fetch_add(&(src->controller->ref_count), 1);

    return copy; 

    error:
        if(copy) free(copy);
        return NULL;
}

shared_ptr_t* move_shared_ptr(shared_ptr_t* src){
    shared_ptr_t* dest = malloc(sizeof(shared_ptr_t));
    check_mem(dest);
    check(src->controller && src, "Invalid input shared pointer state!"); 

    dest->controller = src->controller;
    atomic_fetch_add(&(src->controller->ref_count), 1);

    destroy_sp(src);
    return dest;

    error:
        if(dest) free(dest);
        return NULL; 
}

void destroy_sp(shared_ptr_t* sp){
    int valid = sp && sp->controller && sp->controller->resource; 
    if(!valid) {
        log_err("Invalid shared pointer state!");
        return;
    }

    atomic_fetch_sub(&sp->controller->ref_count, 1);
    if(atomic_load(&(sp->controller->ref_count)) == 0) { 
        if(sp->controller->destructor) {
            sp->controller->destructor(sp->controller->resource);
        } 
        if(!(sp->controller->single_malloc)) { free(sp->controller->resource); }
        free(sp->controller);
        sp->controller=NULL;
    } else {
        sp->controller = NULL; 
    }
    free(sp);
}

weak_ptr_t* weak_ptr_create(const shared_ptr_t* const sp) {
    weak_ptr_t* wp = malloc(sizeof(weak_ptr_t));
    check_mem(wp); 
    check(sp && sp->controller, "Invalid shared pointer state!"); 
    wp->controller = sp->controller;

    atomic_fetch_add(&(((controller_t*)(wp->controller))->weak_count), 1);

    return wp;

    error:
        if(wp) free(wp);
        return NULL; 
}

void destroy_wp(weak_ptr_t* wp) {
    int valid = wp && wp->controller;
    if(!valid) {
        log_err("Invalid weak pointer state!");
        return;
    }
    // Little trick: casting const controller inside weakptr into
    // a non const controller for atomic operation  
    atomic_fetch_sub(&(((controller_t*)wp->controller)->weak_count), 1); 
    wp->controller = NULL; 
    free(wp);
}

const void* get_raw_ptr(shared_ptr_t* sp) {
    int valid = sp && sp->controller && sp->controller->resource; 
    check(valid, "Invalid shared pointer state!"); 

    return sp->controller->resource; 
    error:
        return NULL;
}