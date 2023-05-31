#!/usr/local/bin/node
const n = 30;

function fibonacciRecursive(n) {
  if (n <= 1) {
    return n;
  }
  return fibonacciRecursive(n - 1) + fibonacciRecursive(n - 2);
}

function benchmark(func, param) {
  const start = process.hrtime();
  const result = func(param);
  const end = process.hrtime(start);
  const executionTime = (end[0] * 1e9 + end[1]) / 1e6; // Convert to milliseconds
  console.log(`Result: ${result}`);
  console.log(`Execution time: ${executionTime}ms`);
}


console.log('Benchmarking Recursive Fib:');
benchmark(fibonacciRecursive, n);
