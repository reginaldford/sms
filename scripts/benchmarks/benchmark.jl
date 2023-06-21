function fibonacci_recursive(n)
    if n < 2
        return n
    else
        return fibonacci_recursive(n - 1) + fibonacci_recursive(n - 2)
    end
end

function benchmark_fibonacci_recursive()
    start_time = time()
    result = fibonacci_recursive(27)
    end_time = time()
    elapsed_time = end_time - start_time

    println("Fibonacci(27) = ", result)
    println("Elapsed Time: ", elapsed_time, " seconds")
end

benchmark_fibonacci_recursive()
