#include "src/main/sms.h"
#include "stdio.h"
// Shared object example
// Example of the unwrapping necessary to access a raw value, like a double (f64)
/* This file is to be compiled with gcc -shared fib.c -o fib.so
 * Then you would load the file with let so = soLoad(<full path to fib.so>).
 * Obtain the function with let f=soFun(so,"fib"), and then call it with f(whatever you want)
 * */

double fib_recursive(int n) {
    if (n < 2)
        return 1;
    return fib_recursive(n-1)+fib_recursive(n-2);
}

/*
 * This is the function that gets loaded by soLoad(...)
 * The only function signature supported is sm_object * fname(sm_object* input)
 * The input is a pointer to the sm_expr that is sent from the SMS call.
 * From here, C code can mutate the values given from SMS.
 */
sm_object * fib(sm_object * input){
  sm_expr* array = (sm_expr*)input;
  if(array->size != 2){
    printf("There should be 2 inputs: the fib sequence index, and a place for the output_number (a number).\n");
    return input;
  }
  sm_object** ptr_array = (sm_object**)(array+1);
  sm_f64 * input_number = (sm_f64*)(ptr_array[0]);
  sm_f64 * output_number = (sm_f64*)(ptr_array[1]);
  if(input_number->my_type != SM_F64_TYPE || output_number -> my_type != SM_F64_TYPE){
    printf("Both values given to fib must be floating point numbers.\n");
    return (sm_object*)input;
  }
  output_number->value = fib_recursive(input_number->value);
  return (sm_object*)output_number;
}

