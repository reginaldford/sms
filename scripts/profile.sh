#!/bin/bash
# Requires a .sms file path as argument 1
# Compiles sms with profiling and leaves results in prof_results
# Example: ./profile.sh ../sms_src/benchmark.sms

# openbsd uses gmake
MAKE=make
# 1400 seems just enough for non-memoized recursive fib 27
MEM_MB=1

# Provide an SMS file name as the argument
if [ $# -eq 0 ]; then
  echo "Provide the path to an SMS file to profile."
  exit 1
fi

cd .. &&
$MAKE prof &&
cd scripts &&

# You may need to adjust the memory usage:
../bin/sms_prof -m1400 -s $1
llvm-profdata-18 merge -o default.profdata-18 default.profraw &&

mkdir -p prof_results

llvm-cov-18 show ../bin/sms_prof -instr-profile=default.profdata-18 > prof_results/coverage_count.dat
llvm-profdata-18 show default.profdata-18 > prof_results/summary.dat
llvm-profdata-18 show --all-functions default.profdata-18 > prof_results/thorough.dat
llvm-profdata-18 show --topn=40 default.profdata-18 > prof_results/top_40.dat
mv *.profdata-18 *.profraw ../scripts/prof_results
grep -v "|      " -C1 ../scripts/prof_results/coverage_count.dat > prof_results/coverage_count_nonzero.dat
