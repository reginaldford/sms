
double fib_recursive(int n) {
    if (n < 2)
        return 1;
    return fib_recursive(n-1)+fib_recursive(n-2);
}
