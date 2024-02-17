#!/usr/bin/python
#!/usr/local/bin/python
import time

num_to_fib=31;

def fibonacci_recursive(n):
    if n <= 1:
        return 1
    return fibonacci_recursive(n - 1) + fibonacci_recursive(n - 2)

def benchmark_fibonacci_recursive():
    start_time = time.time()
    result = fibonacci_recursive(num_to_fib);
    end_time = time.time()
    execution_time = (end_time - start_time) * 1000  # Convert to milliseconds
    print("Recursive Fibonacci(",num_to_fib,") =", result)
    print("Execution time:", execution_time, "milliseconds")

benchmark_fibonacci_recursive()
