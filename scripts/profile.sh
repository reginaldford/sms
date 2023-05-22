#!/bin/bash
# Requires a .sms file path as argument 1
# Compiles sms with profiling and leaves results in prof_results
# Example: ./profile.sh ../sms_src/benchmark.sms

# openbsd uses gmake
MAKE=make
# 1400 seems just enough for non-memoized recursive fib 27
MEM_MB=1400

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
llvm-profdata merge -o default.profdata default.profraw &&

mkdir -p prof_results

llvm-cov show ../bin/sms_prof -instr-profile=default.profdata > prof_results/coverage_count.dat
llvm-profdata show default.profdata > prof_results/summary.dat
llvm-profdata show --all-functions default.profdata > prof_results/thorough.dat
llvm-profdata show --topn=40 default.profdata > prof_results/top_40.dat
mv *.profdata *.profraw ../scripts/prof_results
grep -v "|      " -C1 ../scripts/prof_results/coverage_count.dat > prof_results/coverage_count_nonzero.dat
