#!/bin/bash
# author: huyendkcmc
#==============================================================================
# DESCRIPTION: Testing for dnsmasq command.
#              Start, stop and restart dnsmasq service then check 
#              result.
#              Read and syntax check configuration files.
#==============================================================================

check=0
echo "Testing dnsmasq" > ${log_file}

# Start dnsmasq service
service dnsmasq start >> ${log_file} 2>&1
if [ $? -ne 0 ]; then
  check=1
fi

# Stop dnsmasq service
service dnsmasq stop >> ${log_file} 2>&1
if [ $? -ne 0 ]; then
  check=1
fi

# Restart dnsmasq service
service dnsmasq restart >> ${log_file} 2>&1
if [ $? -ne 0 ]; then
  check=1
fi

# Read and syntax check configuration files
dnsmasq --test >> ${log_file} 2>&1
if [ $? -ne 0 ]; then
  check=1
fi

# Check result of testcase
assert_passed $check 0
