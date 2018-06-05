/************************************************
 * heatrun.c
 *
 * Copyright 2016 Toshiba corporation
 *
 ************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <tzcslib/TzcsUtil.h>


//#========================================================================
//# main
//#========================================================================
int main (int argc , char *argv[])
{
	ULONG	testmode = 0;

	get_hci_value(VAL_TEST_MODE, &testmode);

	system("sleep 20");
	if(testmode == 1){
		system("reboot");
	}else{
		system("poweroff");
	}
	return 0;
}
