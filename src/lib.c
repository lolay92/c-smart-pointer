#include "include/lib.h"


control_t* create_controller(void* data, size_t data_size, destructor_fn destructor){
    control_t* controller = malloc(sizeof(control_t));
    if(!controller){
        perror("Failed to allocate memory for controller creation!");
        return NULL;
    };

    controller->data = data;
    controller->destructor = destructor;
    controller->data_size = data_size;
    controller->ref_count= 1;

    return controller;
}

shared_ptr_t* init_shared_ptr(){
    control_t* controller = create_controller(NULL, 0, NULL);
    shared_ptr_t* sp = malloc(sizeof(shared_ptr_t));
    if(!sp){
        perror("Failed to allocate memory for shared_ptr initialization!");
        return NULL;
    };
    sp->controller = controller;

    return sp;
}

shared_ptr_t* create_shared_ptr(void* data, size_t data_size, destructor_fn destructor){
    control_t* controller = create_controller(data, data_size, destructor);
    shared_ptr_t* sp = malloc(sizeof(shared_ptr_t));
    if(!sp){
        perror("Failed to allocate memory for shared_ptr initialization!");
        return NULL;
    };
    sp->controller = controller;

    return sp;
}

// TO IMPLEMENT
void safe_increment_ref_count(spinlock_t* spinlock, size_t ref_count){

}

shared_ptr_t* copy_shared_ptr(shared_ptr_t* src){
    shared_ptr_t* copy = malloc(sizeof(shared_ptr_t));
    if(!copy){
        perror("Failed to allocate memory for shared_ptr initialization!");
        return NULL;
    };

    if(src->controller){
        copy->controller = src->controller;
        // safe_increment_ref_count(spinlock, src->controller->ref_count)
        // src->controller->ref_count++; // Later implement spinlock for thread safety
    }; 

    return copy; 
}

shared_ptr_t* move_shared_ptr(shared_ptr_t* src){
    shared_ptr_t* dest = malloc(sizeof(shared_ptr_t));
    if(!dest){
        perror("Failed to allocate memory for shared_ptr initialization!");
        return NULL;
    };

    if(src->controller){
        dest->controller = src->controller;
        // safe_increment_ref_count(spinlock, src->controller->ref_count)
        // src->controller->ref_count++; // Later implement spinlock for thread safety
    };

    destroy_shared_ptr(src);
    free(src); 

    return dest; 
}

shared_ptr_t* make_shared(void* data, size_t data_size){
    
}

void destroy_shared_ptr(shared_ptr_t*){

}


