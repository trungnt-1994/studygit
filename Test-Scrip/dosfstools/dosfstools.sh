#!/bin/sh
# author: trungntcmc
#======================================================
# DESCRIPTION: Create test script for dosfstools package
#======================================================

# create device for test if not exist
mknod  /dev/loop8 b 7 8
dd if=/dev/loop8 of=image.img bs=1M count=1024 > ${data_dir}
losetup /dev/loop8/ image.img
device="/dev/loop8"

# make linux ext2 filesystem
mkfs $device
# veryfi that it created ext2 file system
file -sL $device > ${data_dir}/result1.txt
# make specify linux filesystem type (ext3)
mkfs -t ext3 $device
# veryfi that it created ext3 file system
file -sL $device > ${data_dir}/result2.txt
# specify a block count
mkfs $device 10000 > ${data_dir}/result3.txt
# just get report about errors
fsck -n $device > ${data_dir}/result4.txt

# Expected value when run test
expected1="Linux rev 1.0 ext2 filesystem data,"
expected2="Linux rev 1.0 ext3 filesystem data,"
expected3="Creating filesystem with 10000 blocks"
expected4="$device: clean,"

# Output value when run test
output1=$(grep -e "Linux rev" ${data_dir}/result1.txt | awk '{print $2, $3, $4, $5, $6, $7}')
output2=$(grep -e "Linux rev" ${data_dir}/result2.txt | awk '{print $2, $3, $4, $5, $6, $7}')
output3=$(grep -e "Creating filesystem" ${data_dir}/result3.txt | awk '{print $1, $2, $3, $4, $6}')
output4=$(grep -e "$device:" ${data_dir}/result4.txt | awk '{print $1, $2}')

check=0
for var in 1 2 3 4
do
  expected_var="expected$var"
  output_var="output$var"
  expected_value=${!expected_var}
  output_value=${!output_var}

if [ "$expected_value" = "" ]; then
  echo "No expected output was defined!" >> ${log_file}
  echo "${test_failed_text}" >> ${RESULT_FILE}
  exit -1
elif [ "$expected_value" != "$output_value" ]; then
  echo "Expected value and output value is not equal!" >> ${log_file}
  echo "${test_failed_text}" >> ${RESULT_FILE}
  exit -1
else
  assert_passed $check 0
fi
done

# Delete non-necesscery file after run test
rm -f ${data_dir}/output*
rm -f ${data_dir}/expected*
rm -f ${data_dir}/result*
