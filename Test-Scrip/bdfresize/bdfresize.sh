#!/bin/sh
# author: trungntcmc
#=======================================================================
# DESCRIPTION: Test for the bdf file is resize when run command bdfresize
#=======================================================================
size_change=2
# This command to run bdfesize command to change size of bdf file
bdfresize -f $size_change ${data_dir}/bdfresize1.bdf  > ${log_file} 2>&1
pixel_size1=$(grep  "PIXEL_SIZE" ${data_dir}/bdfresize1.bdf | awk '{print $2}')
pixel_size2=$(grep  "PIXEL_SIZE" ${log_file} | awk '{print $2}')

expected=`expr $pixel_size1 \* $size_change`
output=$pixel_size2

check=0
# if PIXEL_SIZE in SIZE_FILE_ROOT= = SIZE_FILE_CHANGE is True, else False
if [ "$expected" = "" ] && [ "$output" = "" ]; then
  echo "${test_failed_text}" >> ${RESULT_FILE}
  echo "No expected output was defined!" >> ${log_file}
  check=1
elif [ "$expected" != "$output" ]; then
  echo "${test_failed_text}" >> ${RESULT_FILE}
  echo "Expected value and output value is not equal!" >> ${log_file}
  check=1
else
  assert_passed $check 0
fi

