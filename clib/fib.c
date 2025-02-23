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

sm_object * fib(sm_object * input){
  sm_expr* array = (sm_expr*)input;
  sm_object** ptr_array = (sm_object**)(array+1);
  sm_f64 * n0 = (sm_f64*)(ptr_array[0]);
  if(n0->my_type == SM_F64_TYPE){
    n0->value = fib_recursive(n0->value);
  }
  return input;
}

