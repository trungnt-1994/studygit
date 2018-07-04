/************************************************
 * check_webauth.c
 *
 * Copyright 2016 Toshiba corporation
 *
 ************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <tzcslib/TzcsUtil.h>
#include <tzcslib/TzcsDisplay.h>

const char* indicator_pid = "pidof indicator";

//#========================================================================
//# main
//#========================================================================
int main(int argc , char *argv[])
{
	int		cont = 0,ret;
	ULONG	web_auth_time = 0;
	char	input[256],p_id[100];

	memset(input, 0, sizeof(input));

	//######### Info get #############
	//### Web auth timer get ###
	get_hci_value(VAL_WEB_AUTH_TIME, &web_auth_time);
	//######### Web auth timer check #############
	while(1){
		if(cont >= web_auth_time){
			system("printf \"5\" > /root/ActModeFlag");
			ret = get_info("pidof tzcs-polling", p_id);
			if(ret == TZCS_TRUE){
				//###kill ${PID}###
				sprintf(&input[0], "kill %s", p_id);
				system(input);
			}
			Big_Core_Error_Display_log("/usr/share/pngdisplay/E038.png", 0x00000038);
			ret = get_info(indicator_pid, p_id);
//printf("indicate ret = %d p-id = %s\n",ret,p_id);
			if(ret == TZCS_TRUE){
				//###kill ${INDICATOR_PID}###
				sprintf(&input[0], "kill %s", p_id);
				system(input);
			}
			system("sleep 10");
			system("poweroff > /dev/null 2>&1");
//			gui_disperr_and_powoff(Error_WebAuth, TIMEOUT);
			break;
		}
		system("sleep 60");
		cont++;
	}
	return 0;
}

