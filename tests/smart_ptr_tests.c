#include "unittest.h"
#include "debug.h"
#include "smart_ptr.h"

typedef struct ArrayInt {
    int* data; 
    size_t size; 
} ArrayInt_t; 

void arrayInt_destructor(void* resource) {
    ArrayInt_t* array = (ArrayInt_t*) resource; 
    free(array->data); 
}

void array_int_test_setup(ArrayInt_t* arr, size_t DATA_SIZE) {
    arr->data = malloc(DATA_SIZE*sizeof(int));
    arr->size = DATA_SIZE;

    for(size_t i=0; i<DATA_SIZE; ++i) {
        arr->data[i] = i+1; 
    }
}

void shared_ptr_test_cleanup(shared_ptr_t* sp){
    destroy_sp(sp);
}

char* test_create_controller() {
    controller_t* controller = create_controller(sizeof(ArrayInt_t), arrayInt_destructor); 
    unittest_assert(controller!=NULL, "Controller should not be null!\n"); 
    unittest_assert(controller->ref_count==1, "ref count should be initialized to 1!\n"); 

    free(controller->resource);
    free(controller);
    controller=NULL;

    return NULL;
}

char* test_init_sp() {
    shared_ptr_t* sp = init_shared_ptr(sizeof(ArrayInt_t), arrayInt_destructor); 
    unittest_assert(sp!=NULL && sp->controller->ref_count==1, "Should initialize smart pointer with ref count equal to 1!\n");

    ArrayInt_t* arr = (ArrayInt_t*)get_raw_ptr(sp);
    unittest_assert(arr==sp->controller->resource, "Testing 'get_raw_ptr' func: Both pointers should point to the same memory adress!\n");
    
    array_int_test_setup(arr, 10); 
    unittest_assert(arr->size == 10, "Data size of the resource (array of integers here) should be 10!\n");
    unittest_assert(arr->data[0] == 1, "First element of the array should be 1!\n");

    destroy_sp(sp);
    sp=NULL;
    arr = NULL;
    return NULL;
}

char* test_make_shared() {
    shared_ptr_t* sp = make_shared(sizeof(ArrayInt_t), arrayInt_destructor);
    unittest_assert(sp!=NULL && sp->controller->ref_count==1, "Should initialize smart pointer!\n");

    ArrayInt_t* arr = (ArrayInt_t*)get_raw_ptr(sp);
    unittest_assert(arr==sp->controller->resource, "Both pointers should point to the same memory adress!\n")
    array_int_test_setup(arr, 20);
    
    unittest_assert(arr->size == 20, "Data size of the resource (array of integers) should be 20!\n");
    unittest_assert(arr->data[19] == 20, "Last element of the array should be 20!\n");

    destroy_sp(sp);
    sp=NULL;
    arr = NULL;
    return NULL;
}

shared_ptr_t* global_sp = NULL;
ArrayInt_t* global_sp_rsrc = NULL;

void shared_ptr_test_setup(){
    global_sp = make_shared(sizeof(ArrayInt_t), arrayInt_destructor);
    global_sp_rsrc = (ArrayInt_t*) global_sp->controller->resource;
    array_int_test_setup(global_sp_rsrc, 50);
}

char* test_copy_sp() {
    shared_ptr_test_setup();
    shared_ptr_t* sp_copy = copy_shared_ptr(global_sp);
    unittest_assert(sp_copy->controller==global_sp->controller, "Should point to the same controller!")
    unittest_assert(sp_copy->controller->ref_count==2, "shared pointer copy ref count should have increased to 2!");
    unittest_assert(global_sp->controller->ref_count==2, "global shared pointer ref count should have increased to 2!");

    ArrayInt_t* arr_copy = (ArrayInt_t*)get_raw_ptr(sp_copy);
    unittest_assert((global_sp_rsrc->size==arr_copy->size), "Member variables of the resource pointed should have the same values!");
    unittest_assert((global_sp_rsrc->data[global_sp_rsrc->size-1]==arr_copy->data[arr_copy->size-1]), "Last data element should be the same: 50");

    destroy_sp(sp_copy);
    sp_copy=NULL;
    arr_copy = NULL;

    unittest_assert(global_sp->controller->ref_count == 1, "Ref count should have decreased to 1 after the destruction of the copy!");
    return NULL;
}

char* test_move_sp() {
    // Storing global_sp current state
    int data_stored[global_sp_rsrc->size];
    for(size_t i=0; i<global_sp_rsrc->size; i++) {
        data_stored[i] = global_sp_rsrc->data[i]; 
    }

    // Transferring the ownership/control
    shared_ptr_t* moved_sp = move_shared_ptr(global_sp);

    global_sp_rsrc = NULL; 
    global_sp=NULL;

    unittest_assert(moved_sp->controller!=NULL, "Should not be NULL because of ownership transferred!\n"); 
    unittest_assert(moved_sp->controller->ref_count ==1, "Ref count should be 1!\n");

    ArrayInt_t* moved_arr = (ArrayInt_t*)get_raw_ptr(moved_sp);
    size_t data_stored_size = sizeof(data_stored)/ sizeof(int);
    unittest_assert(moved_arr->size == data_stored_size, "Data size of the resource (array of integers) should be 50!\n");
    unittest_assert(moved_arr->data[data_stored_size-1] == data_stored[data_stored_size-1], "Last element of the array should be 50!\n"); 

    destroy_sp(moved_sp);
    moved_sp=NULL;
    moved_arr = NULL;
    return NULL; 
}

char* test_weak_ptr_create() {
    shared_ptr_test_setup();
    weak_ptr_t* wp = weak_ptr_create(global_sp);

    unittest_assert(wp!=NULL, "Should initialize weak pointer!\n");
    unittest_assert(wp->controller->weak_count==1, "xeak count should have been set to 1!\n");

    unittest_assert(global_sp->controller->ref_count==1, "Ref count should stay the same!\n");

    ArrayInt_t* wp_rsrc = (ArrayInt_t*)wp->controller->resource;
    unittest_assert(wp_rsrc->size==global_sp_rsrc->size, "weak ptr resource and shared pointer resource should be the same!\n");
    unittest_assert(wp_rsrc->data[wp_rsrc->size-1] == global_sp_rsrc->data[global_sp_rsrc->size-1], "Last element of both resource arrays should be the same!\n"); 

    destroy_wp(wp);
    wp=NULL;
    wp_rsrc=NULL;

    destroy_sp(global_sp);
    global_sp=NULL;
    global_sp_rsrc=NULL;
    return NULL;
}

char* all_tests() {
    test_suite_start();

    unittest_run(test_create_controller);
    unittest_run(test_init_sp);
    unittest_run(test_make_shared);
    unittest_run(test_copy_sp);
    unittest_run(test_move_sp);
    unittest_run(test_weak_ptr_create);
    
    return NULL; 
}

int main() {

    char* result=all_tests(); 
    if(result) {
        printf("FAILED: %s\n", result); } 
    else { 
        debug("ALL TESTS PASSED!"); 
        }
    return result!=0; 
}
