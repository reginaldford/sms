#!/usr/bin/bash
# Compiles sms with profiling and leaves results in prof_results

# Provide an SMS file name as the argument
if [ $# -eq 0 ]; then
  echo "Provide the path to an SMS file to profile."
  exit 1
fi

cd .. &&
make prof &&
cd bin &&

# You may need to adjust the memory usage:
./sms_prof -m1400 -s $1
llvm-profdata merge -o default.profdata default.profraw &&

mkdir -p ../scripts/prof_results

llvm-cov show ./sms_prof -instr-profile=default.profdata > ../scripts/prof_results/coverage_count.dat
llvm-profdata show default.profdata > ../scripts/prof_results/summary.dat
llvm-profdata show --all-functions default.profdata > ../scripts/prof_results/thorough.dat
llvm-profdata show --topn=40 default.profdata > ../scripts/prof_results/top_40.dat
mv *.profdata *.profraw ../scripts/prof_results
grep -v "|      " ../scripts/prof_results/coverage_count.dat > ../scripts/prof_results/coverage_count_nonzero.dat
