#!/bin/bash
# author: trungntcmc
#=======================================================================
# DESCRIPTION: Test for the bdf file is resize when run command bdfresize
#=======================================================================

file_change=bdfresize2.bdf
file_root=${data_dir}/bdfresize1.bdf
size=2/1

# This command to run bdfesize command to change size of bdf file
# bdfresize -f $size $file_root >$file_change
bdfresize -f $size $file_root >${log_file}
pixel_size1=$( echo "$(grep  "PIXEL_SIZE" $file_root | awk '{print $2}')" )
pixel_size2=$( echo "$(grep  "PIXEL_SIZE" ${log_file} | awk '{print $2}')" )

size_file_root=$( expr "$pixel_size1" * "$size" )
size_file_change=$pixel_size2

# This function to check result of command
# bdfresize -f 2/1 bdfresize1.bdf >bdfresize2.bdf
# if PIXEL_SIZE in SIZE_FILE_ROOT= = SIZE_FILE_CHANGE is True, else False

assert_passed $? 0
