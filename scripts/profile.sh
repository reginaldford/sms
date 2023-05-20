# compiles sms with profiling, runs the next steps
if [ $# -eq 0 ]; then
  echo "Provide the path to an sms file to profile."
  exit 1
fi
# Your script logic goes here
echo "Profiling: $1"

cd .. &&
make prof &&
cd bin &&
./sms_prof -s $1
llvm-profdata merge -o default.profdata default.profraw &&
mkdir -p ../scripts/prof_results
llvm-cov show ./sms_prof -instr-profile=default.profdata > ../scripts/prof_results/coverage_count.dat
llvm-profdata show default.profdata > ../scripts/prof_results/summary.dat
llvm-profdata show --all-functions default.profdata > ../scripts/prof_results/thorough.dat
llvm-profdata show --topn=40 default.profdata > ../scripts/prof_results/top_40.dat
