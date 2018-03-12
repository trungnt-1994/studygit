#!/bin/sh
# author: trungntcmc
#=======================================================================
# DESCRIPTION: Test for crash package 
# PRECONDITION: Need run as root
#=======================================================================

debug_kernel="/usr/lib/debug/vmlinux-3.16.0-4-amd64"
system_map="/boot/System.map-3.16.0-4-amd64"
commands_file="${data_dir}/commands.txt"
crash_commands=(ps swap irq ipcs vm sys files alias mount net timer mod sig dev set)

if [ ! -f "$debug_kernel" ]; then
  echo "You don't have a debug kernel. Please create debug kernel for test crash!"  > ${log_file}
  echo "Then update path to debug kernel in ${TESTCASES_DIR}/${package_name}" >> ${log_file}
  exit 1
fi

# Prepare a commands list to run with crash
echo "" > $commands_file
for i in ${crash_commands[@]}; do
  echo "$i > ${data_dir}/result_$i.txt" >> $commands_file
done
echo exit >> $commands_file

# This command to run crash's commands
crash $debug_kernel $system_map -i $commands_file

# Expected value when run test
expected_ps="PID PPID CPU TASK ST %MEM VSZ RSS COMM"
expected_swap="SWAP_INFO_STRUCT TYPE SIZE USED PCT PRI FILENAME"
expected_irq="IRQ IRQ_DESC/_DATA IRQACTION NAME"
expected_mount="MOUNT SUPERBLK TYPE DEVNAME DIRNAME"
expected_set="PID: COMMAND: TASK: CPU: STATE:"
expected_net="NET_DEVICE NAME IP ADDRESS(ES)"
expected_timer="TVEC_BASES[0]: JIFFIES EXPIRES TIMER_LIST FUNCTION"
expected_files="PID: TASK: CPU: COMMAND: ROOT: CWD: FD FILE DENTRY INODE TYPE PATH"
expected_alias="ORIGIN ALIAS COMMAND"
expected_vm="PID: TASK: CPU: COMMAND: MM PGD RSS TOTAL_VM VMA START END FLAGS FILE"
expected_ipcs="SHMID_KERNEL KEY SHMID UID PERMS BYTES NATTCH STATUS"
expected_mod="MODULE NAME SIZE OBJECT FILE"
expected_sig="PID: TASK: CPU: COMMAND: SIGNAL_STRUCT: NR_THREADS: SIG SIGACTION HANDLER MASK FLAGS"
expected_sys="SYSTEM DEBUG DUMPFILE: CPUS: DATE: UPTIME: LOAD TASKS: NODENAME: RELEASE: VERSION: MACHINE: MEMORY:"
expected_dev="CHRDEV NAME CDEV OPERATIONS BLKDEV NAME GENDISK OPERATIONS" 

# Output value when run test
output_ps=$( echo "$(grep -e "PID" ${data_dir}/result_ps.txt | awk '{print $1, $2, $3, $4, $5, $6, $7, $8, $9}')" )
output_swap=$( echo "$(grep -e "SWAP_INFO_STRUCT" ${data_dir}/result_swap.txt | awk '{print $1, $2, $3, $4, $5, $6, $7}')" )
output_irq=$( echo "$(grep -e "IRQ_DESC/_DATA" ${data_dir}/result_irq.txt | awk '{print $1, $2, $3, $4 }')" )
output_mount=$( echo "$(grep -e "MOUNT"  ${data_dir}/result_mount.txt | awk '{print $1, $2, $3, $4, $5 }')" )
output_net=$( echo "$(grep -e "NET_DEVICE" ${data_dir}/result_net.txt | awk '{print $1, $2, $3, $4 }')" )
output_mod=$( echo "$(grep -e "MODULE" ${data_dir}/result_mod.txt | awk '{print $1, $2, $3, $4, $5 }')" )
output_alias=$( echo "$(grep -e "ORIGIN" ${data_dir}/result_alias.txt | awk '{print $1, $2, $3}')" )
output_ipcs=$( echo "$(grep -e "SHMID_KERNEL"  ${data_dir}/result_ipcs.txt | awk '{print $1, $2, $3, $4, $5, $6, $7, $8}'| uniq)" )

#this is output for timer command 
output_timer1=$( echo "$(grep -e "TVEC_BASES" ${data_dir}/result_timer.txt | awk '{print $1}')" )
output_timer2=$( echo "$(grep -e "JIFFIES" ${data_dir}/result_timer.txt | awk '{print $1}')" )
output_timer3=$( echo "$(grep -e "EXPIRES" ${data_dir}/result_timer.txt | awk '{print $1, $2, $3}')" )
output_timer="$output_timer1 $output_timer2 $output_timer3"

#this is output for files command
output_files1=$( echo "$(grep -e "PID:" ${data_dir}/result_files.txt | awk '{print $1, $3, $5, $7}')" )
output_files2=$( echo "$(grep -e "ROOT:" ${data_dir}/result_files.txt | awk '{print $1, $3}')" )
output_files3=$( echo "$(grep -e "FD" ${data_dir}/result_files.txt | awk '{print $1, $2, $3, $4, $5, $6}')" )
output_files="$output_files1 $output_files2 $output_files3"

#this is output for vm  command
output_vm1=$( echo "$(grep -e "PID:" ${data_dir}/result_vm.txt | awk '{print $1, $3, $5, $7}')" )
output_vm2=$( echo "$(grep -e "PGD" ${data_dir}/result_vm.txt | awk '{print $1, $2, $3, $4}')" )
output_vm3=$( echo "$(grep -e "VMA" ${data_dir}/result_vm.txt | awk '{print $1, $2, $3, $4, $5}')" )
output_vm="$output_vm1 $output_vm2 $output_vm3"

#this is output for sig  command
output_sig1=$( echo "$(grep -e "PID"  ${data_dir}/result_sig.txt | awk '{print $1, $3, $5, $7}')" )
output_sig2=$( echo "$(grep -e "SIGNAL_STRUCT"   ${data_dir}/result_sig.txt | awk '{print $1, $3}')" )
output_sig3=$( echo "$(grep -e "HANDLER" ${data_dir}/result_sig.txt | awk '{print $1, $2, $3, $4,  $5}')" )
output_sig="$output_sig1 $output_sig2 $output_sig3"

#this is output for sys command
echo "$(sed 1p ${data_dir}/result_sys.txt | awk '{print $1}')" >${data_dir}/output_sys_command.txt
output_sys_notcut=$( echo "$(awk 'ORS=NR%4?" ":" "' ${data_dir}/output_sys_command.txt)" )
echo  "$output_sys_notcut" | sed -e 's/\<crash\>>//g' >${data_dir}/output_sys_notcut.txt
output_sys=$( awk '{$1=$1;print}' ${data_dir}/output_sys_notcut.txt)

#this is output for dev command
output_dev1=$( echo "$(grep -e "CHRDEV"  ${data_dir}/result_dev.txt | awk '{print $1, $2, $3, $4}')" )
output_dev2=$( echo "$(grep -e "BLKDEV"  ${data_dir}/result_dev.txt | awk '{print $1, $2, $3, $4}')" )
output_dev="$output_dev1 $output_dev2"

#this is output for set command
echo "$(sed 1p ${data_dir}/result_set.txt | awk '{print $1}')" >${data_dir}/output_set_command.txt
output_set_notcut=$( echo "$(awk 'ORS=NR%4?" ":" "' ${data_dir}/output_set_command.txt)" )
echo  "$output_set_notcut" | sed -e 's/\<crash\>>//g' >${data_dir}/output_set_notcut.txt
output_set=$( awk '{$1=$1;print}' ${data_dir}/output_set_notcut.txt)

check=0
for var in ${crash_commands[@]}; 
do
  expected_var="expected_$var"
  output_var="output_$var"
  expected_value=${!expected_var}
  output_value=${!output_var}

if [ "$expected_value" = "" ]; then
  echo "No expected output was defined!" >> ${log_file}
  echo "${test_failed_text}" >> ${RESULT_FILE}
elif [ "$expected_value" != "$output_value" ]; then
  echo "Expected value and output value is not equal!" >> ${log_file}
  echo "${test_failed_text}" >> ${RESULT_FILE}
else
  assert_passed $check 0
fi
done

#Delete non-necessary file after run test
rm -f ${data_dir}/result_*.txt
rm -f ${data_dir}/output_*.txt
rm -f ${commands_file}
