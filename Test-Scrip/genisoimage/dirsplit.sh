#!/bin/bash
# author: huyendkcmc
#==============================================================================
# DESCRIPTION: Testing for dirsplit command.
#              Split "sampledir" folder containing some files totaling more 
#              than 2MB into individual directories of 2MB each.
#==============================================================================

echo "Testing dirsplit" > ${log_file}

# Split "sampledir" folder.
dirsplit --size 2M --expmode 1 ${data_dir}/sampledir/ >> ${log_file} 2>&1
ls vol_1.list vol_2.list >> ${log_file} 2>&1

# Check result of testcase
assert_passed $? 0

rm -f vol_1.list vol_2.list 
