#!/bin/bash
# author: huyendkcmc
#==============================================================================
# DESCRIPTION: Testing for isovfy command.
#              Verify the integrity of the specified ISO9660 image (cd.iso) and 
#              write the results to standard output.
#==============================================================================

echo "Testing isovfy" > ${log_file}

# Verify the integrity of the "cd.iso".
isovfy -i ${data_dir}/cd.iso >> ${log_file} 2>&1

# Check result of testcase
assert_passed $? 0

