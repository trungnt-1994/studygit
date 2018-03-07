#!/bin/sh
# author: trungntcmc
#======================================================
# DESCRIPTION: Create test script for guile-2.0 package
#======================================================

# Run commands of guile-2.0 package in file guile-2.0_command.txt
guile < ${data_dir}/guile-2.0_command.txt > ${data_dir}/result.txt
# Expected value when run test
expected1="\$1 = 3"
expected2="\$2 = 10"
expected3="\$3 = (2 3 4)"
expected4="\$4 = 120"
expected5="\$5 = 8"
expected6="\$6 = \"Hello Guile-2.0\""
expected7="\$7 = (1 2 3 4)"
expected8="\$8 = (4 1 2 3)"
expected9="\$9 = #f"
expected10="\$10 = #f"
expected11="\$11 = \"This is true\""
expected12="\$12 = Oke"
expected13="\$13 = (0 1 2 3 4 5 6 7 8 9)"

# Output value when run test
output1=$(grep -e "$expected1" ${data_dir}/result.txt)
output2=$(grep -e "$expected2" ${data_dir}/result.txt)
output3=$(grep -e "$expected3" ${data_dir}/result.txt)
output4=$(grep -e "$expected4" ${data_dir}/result.txt)
output5=$(grep -e "$expected5" ${data_dir}/result.txt)
output6=$(grep -e "$expected6" ${data_dir}/result.txt)
output7=$(grep -e "$expected7" ${data_dir}/result.txt)
output8=$(grep -e "$expected8" ${data_dir}/result.txt)
output9=$(grep -e "$expected9" ${data_dir}/result.txt)
output10=$(grep -e "$expected10" ${data_dir}/result.txt)
output11=$(grep -e "$expected11" ${data_dir}/result.txt)
output12=$(grep -e "$expected12" ${data_dir}/result.txt)
output13=$(grep -e "$expected13" ${data_dir}/result.txt)

check=0
for var in 1 2 3 4 5 6 7 8 9 10 11 12 13
do
  expected_var="expected$var"
  output_var="output$var"
  expected_value=${!expected_var}
  output_value=${!output_var}

if [ "$expected_value" = "" ]; then
  echo "No expected output was defined!" >> ${log_file}
  echo "${test_failed_text}" >> ${RESULT_FILE}
  exit 1
elif [ "$expected_value" != "$output_value" ]; then
  echo "Expected value and output value is not equal!" >> ${log_file}
  echo "${test_failed_text}" >> ${RESULT_FILE}
  exit 1
else
  assert_passed $check 0
fi
done
# Delete non-necesscery file after run test
rm -f ${data_dir}/output*
rm -f ${data_dir}/expected*
rm -f ${data_dir}/result.txt
