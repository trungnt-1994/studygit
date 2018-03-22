#!/bin/sh
# author: trungntcmc
#======================================================
# DESCRIPTION: Create test script for guile-2.0 package
#              Run guile command to implementation of the
#              Scheme programming language.
#======================================================

# Run guile commands of guile-2.0 package in file guile-2.0_command.txt
guile < ${data_dir}/guile-2.0_command.txt > ${data_dir}/result.txt

grep "$1 = 3" ${data_dir}/result.txt > ${data_dir}/output.txt
grep "$2 = 10" ${data_dir}/result.txt >> ${data_dir}/output.txt
grep "$3 = (2 3 4)" ${data_dir}/result.txt >> ${data_dir}/output.txt
grep "$4 = 120" ${data_dir}/result.txt >> ${data_dir}/output.txt
grep "$5 = 8" ${data_dir}/result.txt >> ${data_dir}/output.txt
grep "$6 = \"Hello Guile-2.0\"" ${data_dir}/result.txt >> ${data_dir}/output.txt
grep "$7 = (1 2 3 4)" ${data_dir}/result.txt >> ${data_dir}/output.txt
grep "$8 = #f" ${data_dir}/result.txt >> ${data_dir}/output.txt
grep "$9 = \"This is true\"" ${data_dir}/result.txt >> ${data_dir}/output.txt
grep "$10 = Oke" ${data_dir}/result.txt >> ${data_dir}/output.txt
grep "$11 = (0 1 2 3 4 5 6 7 8 9)" ${data_dir}/result.txt >> ${data_dir}/output.txt

check=0
# Compare two file expected.txt and output.txt to get result
diff ${data_dir}/expected.txt ${data_dir}/output.txt
if [ $? != 0 ]; then
        echo "Test Failed!" >> ${log_file} 2>&1
        check=1
fi

# Check test case pass or not
assert_passed $check 0

# Delete non-necesscery file after run test
rm -f ${data_dir}/output.txt
rm -f ${data_dir}/result.txt
