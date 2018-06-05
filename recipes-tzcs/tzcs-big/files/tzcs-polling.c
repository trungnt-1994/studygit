/************************************************
 * tzcs_polling.c
 *
 * Copyright 2016 Toshiba corporation
 *
 ************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <tzcslib/TzcsUtil.h>
#include <tzcslib/TzcsDisplay.h>

const char* tzcsserverinfo = "cat /etc/TZCS/SserverInfo/server";
const char* displayinfo = "cat /etc/X11/display.conf";
const char* networkwired = "cat /sys/class/net/eth0/operstate";
//const char* networkwired = "ethtool eth0 | grep \"Link detected\" | cut -d \' \' -f3";

int TSCM_Connect_Chk(int mode, char *server, char *trycount, char *connecttimer);
// 2017/05/22 start
//int http_header_check(void);
// 2017/05/22 end

//#define TZCS_DEBUG




// 2017/05/22 start
//#========================================================
//# function  network_connect_check()
//#
//# IN  log_name : check log
//#
//# OUT  0 : OK
//#      1 : NG
//#
//#========================================================
int network_connect_check(const char *log_name)
{

	FILE *fp;
	int j,k,period_cnt;
	int	ret=1;
//	int	pid_ret;
//	char command[256];
	char string[256];
	char connect_ip[256];
	char *p;
	


#ifdef TZCS_DEBUG
	printf("tzcs-polling [network_connect_check] start log name=%s  \n",log_name);
#endif

// Check network connection status with traceroute command
//	sprintf(command, "traceroute -w 1 -q 1 -n %s > /dev/null 2>&1 > connect.log &", url);
//	system(command);

	//***** Check results ******
	// Successful example
  ////////////////////////
	//traceroute to google.com (172.217.27.78), 30 hops max, 46 byte packets
	// 1  172.31.35.1  2.115 ms
	// 2  192.168.0.1  2.555 ms
	// 3  114.190.175.1  6.112 ms
	// 4  114.190.175.17  5.502 ms
	// 5  210.163.62.97  7.263 ms
	// 6  118.21.197.25  7.231 ms
	// 7  211.6.91.173  9.663 ms
	// 8  211.0.193.30  10.847 ms
	// 9  108.170.242.129  8.025 ms
	//10  108.170.236.7  14.653 ms
	//11  172.217.27.78  8.089 ms
	///////////////////////// 
	// Successful when [172.217.27.XXX] is output
	//////////////////////

	memset(connect_ip, '\0', sizeof(connect_ip));
	fp = fopen(log_name, "r");
	if(fp != NULL){
	
		//***************************************************************
		//*** Get the connection destination IP address of the first line
		//***************************************************************
		if(fgets(string, 256, fp) != NULL){
			p = strstr(string,"(");
			if(p != NULL){
				for(j = 1,period_cnt = 0,k = 0;;j++){
					if(*(p + j) == '\0'){
						memset(connect_ip, '\0', sizeof(connect_ip));
						break;
					}
					connect_ip[k] = *(p + j);
					k++;
					if(*(p + j) == '.'){
						period_cnt++;
						if(period_cnt >= 3)break;
					}
				}
#ifdef TZCS_DEBUG
			}else{	
				printf("tzcs-polling [network_connect_check] connect IP address get error( ( : non) \n");
#endif
			}
#ifdef TZCS_DEBUG
		}else{
			printf("tzcs-polling [network_connect_check] connect IP address get error(data non) \n");
#endif
		}
#ifdef TZCS_DEBUG
		printf("tzcs-polling [network_connect_check] connect IP address = %s \n",connect_ip);
#endif

		//***********************
		//*** Routing table check
		//***********************
		if(strlen(connect_ip) != 0){
			while(fgets(string, 256, fp) != NULL) {
				if(strstr(string ,connect_ip) != NULL){
#ifdef TZCS_DEBUG
					printf("tzcs-polling [network_connect_check] Matched IP address = %s \n",string);
#endif
					ret = 0;
					break;
				}
			}
#ifdef TZCS_DEBUG
		}else{
			printf("tzcs-polling [network_connect_check] connect IP address get error( IP non) \n");
#endif
		}

#ifdef TZCS_DEBUG
	}else{
		printf("tzcs-polling [network_connect_check] connect.log file open NG \n");
#endif
	}

	if(fp != NULL) fclose(fp);


#ifdef TZCS_DEBUG
	printf("tzcs-polling [network_connect_check] result = %d \n",ret);
#endif

	return(ret);

}

//#========================================================
//# function  light_kill()
//#
//# IN  non
//#
//# OUT non
//#     
//#
//#========================================================
void light_kill(void)
{

	char	work[256];
	char	command[256];
	int		ret;

//	ret = get_info("pidof light", work);
	ret = get_info("pidof firefox", work);
	if(ret == TZCS_TRUE){ 
			//###kill -15 ${FIREFOX_PID}###
			sprintf(command, "kill -15 %s", work);
			system(command);
	}
}

//#========================================================
//# function  traceroute_startup_check()
//#
//# IN  url : check url
//#
//# OUT 1 : No startup
//#     0 : startup
//#
//#========================================================
int traceroute_startup_check(char *url)
{

	char	command[256];
	char	work[256];
	int		result = 0;
	int		ret;

  memset(work, '\0', sizeof(work));
	sprintf(command, "ps | grep traceroute | grep %s", url);
	ret = get_info(command, work);
	if(ret == TZCS_TRUE){
		if((strlen(work) == 0) || (strstr(work ,command) != NULL)){
			 result = 1;
		}
	}

#ifdef TZCS_DEBUG
	printf("tzcs-polling [traceroute_startup_check] work = %s \nresult = %d \n",work,result);
#endif

	return(result);

}
// 2017/05/22 end



//#========================================================================
//# main
//#========================================================================
int main (int argc , char *argv[])
{

//	int		firefoxrebootflag = 0;
	int		webauthfirstflag = 0;
	int		LockState = 0;
	int		Index = 0;
	int		FirstPolling = 0;
//	int		result = 0;
	char	input[100];
	char	Serverinfo[2];

	ULONG	disconnectmode = 0;
	ULONG	wiredretryinterval = 0;
	ULONG	wifiretryinterval = 0;
	ULONG	retrycount = 0;
	ULONG	hci_out = 0;
	ULONG	wlan0sectype = 0;
	char	wlan0ssid[256];
	char	wlan0pass[256];
	char	wlan0channel[4];
	ULONG	pingcheckinterval = 0;
//	ULONG	serial = 0;
	ULONG	testmode = 0;
	char	tscmurl2[256];
	ULONG	Language;
	char	work[256];
	int 	ret;

	int		TSCM_Result = 0;
	int		TzcsMode = 0;
	
	char	displayinfo1[10];
	char	displayinfo2[10];
  char Last_Ssid[STR_BUF_SIZE];
  FILE *fp;
// 2017/05/22 start
//	struct stat st;
// 2017/05/22 end

	memset(input, 0, sizeof(input));
	memset(wlan0ssid, 0, sizeof(wlan0ssid));
	memset(wlan0pass, 0, sizeof(wlan0pass));
	memset(wlan0channel, 0, sizeof(wlan0channel));
	memset(tscmurl2, 0, sizeof(tscmurl2));
  memset(Last_Ssid, '\0', sizeof(Last_Ssid));

	//######### Info get #############
	//### Disconnect mode get ###
	get_hci_value(VAL_SCREEN_LOCK_MODE, &disconnectmode);
	//### Reconnect info get ###
	get_hci_value(VAL_RECONNECT_INTERVAL_LAN, &wiredretryinterval);
	get_hci_value(VAL_RECONNECT_INTERVAL_WLAN, &wifiretryinterval);
	get_hci_value(VAL_RECONNECT_RETRY_CNT, &retrycount);

	hci_out = 1;
	set_hci_value(VAL_WIFI_PRIORITY_NO, &hci_out);
	get_hci_value(VAL_WIFI_AP_SECURITY_DATA, &wlan0sectype);
	get_hci_string(VAL_WIFI_AP_SSID_DATA, &wlan0ssid[0]);
	get_hci_string(VAL_WIFI_AP_PASS_DATA, &wlan0pass[0]);
	get_hci_string(VAL_WIFI_AP_CHAN_DATA, &wlan0channel[0]);
	get_hci_value(VAL_LANGUAGE, &Language);
  set_hci_string(VAL_WIFI_PRIORITY_NO, "1");
  get_hci_string(VAL_WIFI_AP_SSID_DATA, Last_Ssid);

	//### Polling interval get ###
	get_hci_value(VAL_POLLING_INTERVAL, &pingcheckinterval);
//	//### Serial no get ###
//	get_hci_value(VAL_TSCM_SERIAL, &serial);
	
	get_hci_value(VAL_TEST_MODE, &testmode);

	get_hci_string(VAL_SECOND_TSCM_URL, &tscmurl2[0]);

	//######### Web auth check #############
	if(get_env_variable(WEB_AUTH_MODE) == WEB_AUTH_ON){
#ifdef TZCS_DEBUG
    printf("tzcs-polling [main] webauth mode strat  \n");
#endif
//		firefoxrebootflag = 0;
		webauthfirstflag = 0;

		// traceroute start
		system("traceroute -w 1 -q 1 -n google.com > google.log &");
		system("traceroute -w 1 -q 1 -n line.com > line.log &");
		system("traceroute -w 1 -q 1 -n facebook.com > facebook.log &");
		sleep(1);

		//### External site connection check ###
		while(1){
			sleep(1);

			// google.com site check
			if(network_connect_check("google.log") == 0){
#ifdef TZCS_DEBUG
				printf("tzcs-polling [main] google.com connect ok  \n");
#endif
				if(webauthfirstflag != 0){
					sleep(3);
				}
				light_kill();
				break;
			}
			if(traceroute_startup_check("google.com") == 1){
				system("traceroute -w 1 -q 1 -n google.com > google.log &");
			}

			// line.com site check
			if(network_connect_check("line.log") == 0){
#ifdef TZCS_DEBUG
				printf("tzcs-polling [main] line.com connect ok  \n");
#endif
				if(webauthfirstflag != 0){
					sleep(3);
				}
				light_kill();
				break;
			}
			if(traceroute_startup_check("line.com") == 1){
				system("traceroute -w 1 -q 1 -n line.com > line.log &");
			}

			// facebook.com site check
			if(network_connect_check("facebook.log") == 0){
#ifdef TZCS_DEBUG
				printf("tzcs-polling [main] facebook.com connect ok  \n");
#endif
				if(webauthfirstflag != 0){
					sleep(3);
				}
				light_kill();
				break;
			}
			if(traceroute_startup_check("facebook.com") == 1){
				system("traceroute -w 1 -q 1 -n facebook.com > facebook.log &");
			}

			system("echo \"1\" > /etc/TZCS/SserverInfo/server");
			TSCM_Result = TSCM_Connect_Chk(1, "1", "1", "3");
			if( TSCM_Result >= 100){
				//### TZCS Auth NG ###
				if( TSCM_Result == 100 || TSCM_Result == 101 || TSCM_Result == 172){
					//### Power-off or License error from TZCS Server ###
					if(webauthfirstflag != 0){
						sleep(3);
					}
					light_kill();
					break;
				}
				system("rm *.html > /dev/null 2>&1");
				if((tscmurl2[0] != 0) || (TSCM_Result != 110)){
					system("echo \"2\" > /etc/TZCS/SserverInfo/server");
					TSCM_Result = TSCM_Connect_Chk(1, "2", "1", "3");
					if( TSCM_Result == 100 || TSCM_Result == 101 || TSCM_Result == 172){
						//### Power-off or License error from TZCS Server ###
						if(webauthfirstflag != 0){
							sleep(3);
						}
						light_kill();
						break;
					}
				  system("rm *.html > /dev/null 2>&1");
				}
			}
			if( (TSCM_Result < 100) || (TSCM_Result == 110)){
				//### TZCS Auth OK ###
				if(webauthfirstflag != 0){
					sleep(3);
				}
				light_kill();
				break;
			}
			webauthfirstflag = 1;
		}

#ifdef TZCS_DEBUG
		printf("tzcs-polling [main] External site connection check end  \n");
#endif
		system("killall traceroute > /dev/null 2>&1");
		system("rm google.log > /dev/null 2>&1");
		system("rm line.log > /dev/null 2>&1");
		system("rm facebook.log > /dev/null 2>&1");

		while(1){
			//#-----------------------------------------
			//# Mode          : GetClient
			//# TSCS Server   : server1
			//# try count     : 1
			//# connect timer : 15sec
			//#-----------------------------------------
			system("echo \"1\" > /etc/TZCS/SserverInfo/server");
			TSCM_Result = TSCM_Connect_Chk(1, "1", "1", "15");
#ifdef TZCS_DEBUG
    printf("tzcs-polling [main] GetClient server1 Result = %d \n",TSCM_Result);
#endif
			if( TSCM_Result >= 100){
				//### TZCS Auth NG ###
/***** -add [TMZC_Server_License](TZCS (Base))  2017/07/14 Tanaka *****/
//				if( TSCM_Result == 100 || TSCM_Result == 101){
//					//### Power-off from TZCS Server ###
				if( TSCM_Result == 100 || TSCM_Result == 101 || TSCM_Result == 172){
					//### Power-off or License error from TZCS Server ###
/***** -end [TMZC_Server_License](TZCS (Base)) *****/
					break;
				}
				system("rm *.html > /dev/null 2>&1");
				if((tscmurl2[0] != 0) || (TSCM_Result != 110)){
					//#-----------------------------------------
					//# Mode          : GetClient
					//# TSCS Server   : server2
					//# try count     : 1
					//# connect timer : 15sec
					//#-----------------------------------------
					system("echo \"2\" > /etc/TZCS/SserverInfo/server");
					TSCM_Result = TSCM_Connect_Chk(1, "2", "1", "15");
/***** -add [TMZC_Server_License](TZCS (Base))  2017/07/14 Tanaka *****/
//					if( TSCM_Result == 100 || TSCM_Result == 101){
//						//### Power-off from TZCS Server ###
					if( TSCM_Result == 100 || TSCM_Result == 101 || TSCM_Result == 172){
						//### Power-off or License error from TZCS Server ###
/***** -end [TMZC_Server_License](TZCS (Base)) *****/
						break;
					}
				  system("rm *.html > /dev/null 2>&1");
				}
			}
			if( (TSCM_Result < 100) || (TSCM_Result == 110)){
				//### TZCS Auth OK ###
				//### check-webauth kill ###
				ret = get_info("pidof check-webauth", work);
				if(ret == TZCS_TRUE){ 
					//###kill -15 ${CHECK_WEBAUTH_PID}###
					sprintf(&input[0], "kill -15 %s", work);
					system(input);
				}
//				if(firefoxrebootflag == 0){
//					//### firefox kill ###
//				  ret = get_info("pidof light", work);
//				  if(ret == TRUE){ 
//						//###kill -15 ${FIREFOX_PID}###
//						sprintf(&input[0], "kill -15 %s", work);
//						system(input);
//					}
//				}
				break;
			}
		}
	}

	//######### Ctest mode check #############
	if(testmode == 3){
		pingcheckinterval = 5;
	}
	//######### Polling Start #############
	//### Flag Clear ###
	LockState = 0;
	Index = 0;
	FirstPolling = 0;

	system("printf \"0\" > /root/ActModeFlag");
	//### Tzcs server info set ###
	ret = get_info(tzcsserverinfo, work);
	if(ret == TZCS_TRUE){ 
		Serverinfo[0] = work[0];
	}else{
		Serverinfo[0] = '1';
	}
	Serverinfo[1] = '\0';
	//### Tzcs mode get client state set ###
	TzcsMode = 2;
	//### TZCS Server polling Check ###
	while(1){
		//### TZCS Server polling request ###
		if(TzcsMode == 1 || testmode == 3){
			//### GetClient ###
			//#-----------------------------------------
			//# Mode          : GetClient
			//# TSCS Server   : server1 or server2
			//# try count     : 3
			//# connect timer : 60sec
			//#-----------------------------------------
			TSCM_Result = TSCM_Connect_Chk(1, &Serverinfo[0], "3", "60");
		}else{
			//### GetClient state ###
			//#-----------------------------------------
			//# Mode          : GetClientState                                                                                                
			//# TSCS Server   : server1 or server2
			//# try count     : 3
			//# connect timer : 60sec
			//#-----------------------------------------
			TSCM_Result = TSCM_Connect_Chk(2, &Serverinfo[0], "3", "60");
		}
		//### Check the first polling and TZCS server disconnected state error ###
		if(FirstPolling == 0){
			if(TSCM_Result == 102){
				//### Set for GetClient request ###
				//#-----------------------------------------
				//# Mode          : GetClient
				//# TSCS Server   : server1 or server2
				//# try count     : 3
				//# connect timer : 60sec
				//#-----------------------------------------
				TSCM_Result = TSCM_Connect_Chk(1, &Serverinfo[0], "3", "60");
				FirstPolling = 1;
			}
		}
//		if(TSCM_Result >= 100){
		if((TSCM_Result >= 100) && (TSCM_Result != 110)){
			//### Error ###
			if(TSCM_Result == 102){
				//### Tzcs server disconnect ###
				//### Tzcs mode get clien set ###
				TzcsMode = 1;
			}else{
/***** -add [TMZC_Server_License](TZCS (Base))  2017/07/14 Tanaka *****/
//				if( (TSCM_Result != 100) && (TSCM_Result != 101)){
				if( (TSCM_Result != 100) && (TSCM_Result != 101) && (TSCM_Result != 172)){
/***** -end [TMZC_Server_License](TZCS (Base)) *****/
					if(tscmurl2[0] != 0){
						//### Other tzcs server connect ###
						if( Serverinfo[0] == '1'){
							Serverinfo[0] = '2';
						}else{
							Serverinfo[0] = '1';
						}
					//### GetClient state ###
					//#-----------------------------------------
					//# Mode          : GetClient
					//# TSCS Server   : server1 or server2
					//# try count     : 3
					//# connect timer : 60sec
					//#-----------------------------------------
					TSCM_Result = TSCM_Connect_Chk(1, &Serverinfo[0], "3", "60");
					}
				}
			}
			//### Tzcs server auth NG ###
#ifdef TZCS_DEBUG
    printf("tzcs-polling [main] counter=%d retrycount=%d \n",Index,retrycount);
#endif
/***** -add [TMZC_Server_License](TZCS (Base))  2017/07/14 Tanaka *****/
//			if(LockState == 0){
//				if((Index >=  retrycount) || (TSCM_Result == 100) || (TSCM_Result == 101)){
//					if((TSCM_Result == 100) || (TSCM_Result == 101)){
//						//### Power off Req. from TSCM Server  ###
//						system("printf \"1\" > /root/ActModeFlag");
///***** -mod [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
////						if(Language == 1){
////							Big_Core_Error_Display("/usr/share/pngdisplay/PowerOFFfromTSCM_J.png");
////						}else{
////							Big_Core_Error_Display("/usr/share/pngdisplay/PowerOFFfromTSCM.png");
////						}
//						switch(Language){
//							case JAPANESE:
//								Big_Core_Error_Display("/usr/share/pngdisplay/PowerOFFfromTSCM_J.png");
//								break;
//							case GERMAN:
//								Big_Core_Error_Display("/usr/share/pngdisplay/PowerOFFfromTSCM_D.png");
//								break;
//							default:
//								Big_Core_Error_Display("/usr/share/pngdisplay/PowerOFFfromTSCM.png");
//								break;
//						}
///***** -end [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
//						system("sleep 10");
//						system("poweroff");
//					}else if((disconnectmode == 0) || (get_env_variable(WEB_AUTH_MODE) == WEB_AUTH_ON)){
//						//### Disconnect mode power off  ###
//						system("printf \"2\" > /root/ActModeFlag");
///***** -mod [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
////						if(Language == 1){
////							Big_Core_Error_Display("/usr/share/pngdisplay/LostServer_J.png");
////						}else{
////							Big_Core_Error_Display("/usr/share/pngdisplay/LostServer.png");
////						}
//						switch(Language){
//							case JAPANESE:
//								Big_Core_Error_Display("/usr/share/pngdisplay/LostServer_J.png");
//								break;
//							case GERMAN:
//								Big_Core_Error_Display("/usr/share/pngdisplay/LostServer_D.png");
//								break;
//							default:
//								Big_Core_Error_Display("/usr/share/pngdisplay/LostServer.png");
//								break;
//						}
///***** -end [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
//						system("sleep 10");
//						system("poweroff");
//					}else{
//						system("printf \"3\" > /root/ActModeFlag");
//                                          LockState = 1;
///***** -mod [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
////						if(Language == 1){
////							Big_Core_Error_Display("/usr/share/pngdisplay/ScreenLock_J.png");
////						}else{
////							Big_Core_Error_Display("/usr/share/pngdisplay/ScreenLock.png");
////						}
//						switch(Language){
//							case JAPANESE:
//								Big_Core_Error_Display("/usr/share/pngdisplay/ScreenLock_J.png");
//								break;
//							case GERMAN:
//								Big_Core_Error_Display("/usr/share/pngdisplay/ScreenLock_D.png");
//								break;
//							default:
//								Big_Core_Error_Display("/usr/share/pngdisplay/ScreenLock.png");
//								break;
//						}
///***** -end [EAP-TLS](ZCS (1.2)) 2016/12/19 Shimizu *****/
//						//###DisplayInfo=$(cat /etc/X11/display.conf)
//						fp = fopen("/etc/X11/display.conf", "r");
//						if (fp != NULL) {
//							fgets(displayinfo1, sizeof(displayinfo1), fp);
//							fclose(fp);
//						}else{
//							displayinfo1[0] = '\0';
//						}
//					}
//				}
//			}
			if((TSCM_Result == 100) || (TSCM_Result == 101) || (TSCM_Result == 172)){
				//### Power off Req. from TSCM Server  ###
				system("printf \"1\" > /root/ActModeFlag");
				switch(Language){
					case JAPANESE:
							Big_Core_Error_Display("/usr/share/pngdisplay/PowerOFFfromTSCM_J.png");
							break;
					case GERMAN:
							Big_Core_Error_Display("/usr/share/pngdisplay/PowerOFFfromTSCM_D.png");
							break;
					default:
							Big_Core_Error_Display("/usr/share/pngdisplay/PowerOFFfromTSCM.png");
							break;
				}
				system("sleep 10");
				system("poweroff");
			}
			if(LockState == 0){
				if(Index >=  retrycount) {
					if((disconnectmode == 0) || (get_env_variable(WEB_AUTH_MODE) == WEB_AUTH_ON)){
						//### Disconnect mode power off  ###
						system("printf \"2\" > /root/ActModeFlag");
						switch(Language){
							case JAPANESE:
								Big_Core_Error_Display("/usr/share/pngdisplay/LostServer_J.png");
								break;
							case GERMAN:
								Big_Core_Error_Display("/usr/share/pngdisplay/LostServer_D.png");
								break;
							default:
								Big_Core_Error_Display("/usr/share/pngdisplay/LostServer.png");
								break;
						}
						system("sleep 10");
						system("poweroff");
					}else{
						system("printf \"3\" > /root/ActModeFlag");
						LockState = 1;
						switch(Language){
							case JAPANESE:
								Big_Core_Error_Display("/usr/share/pngdisplay/ScreenLock_J.png");
								break;
							case GERMAN:
								Big_Core_Error_Display("/usr/share/pngdisplay/ScreenLock_D.png");
								break;
							default:
								Big_Core_Error_Display("/usr/share/pngdisplay/ScreenLock.png");
								break;
						}
						//###DisplayInfo=$(cat /etc/X11/display.conf)
						memset(displayinfo1, '\0', sizeof(displayinfo1));
						fp = fopen("/etc/X11/display.conf", "r");
						if (fp != NULL) {
							fgets(displayinfo1, sizeof(displayinfo1), fp);
							fclose(fp);
						}
					}
				}
			}
/***** -end [TMZC_Server_License](TZCS (Base)) *****/
#if 0
			result = system("ifconfig|awk 'BEGIN{RS=\"\"}/'eth0'/'|grep -q 'inet addr'");
			if(WEXITSTATUS(result) == 0){
				//###sleep $WiredRetryInterval###
				sprintf(&input[0], "sleep %d", wiredretryinterval);
				system(input);
			}else if(wlan0sectype != 2){
				sprintf(&input[0], "sleep %d", wifiretryinterval);
				system(input);
			}else if(wlan0pass[0] == 0){
				//###iwconfig wlan0 essid "${Wlan0Ssid}" channel ${Wlan0Channel} > /dev/null###
				sprintf(&input[0], "iwconfig wlan0 essid %s channel %s > /dev/null", wlan0ssid, wlan0channel);
				system(input);
				sprintf(&input[0], "sleep %d", wifiretryinterval);
				system(input);
			}else{
				//iwconfig wlan0 essid "${Wlan0Ssid}" key "s:${Wlan0Pass}" channel ${Wlan0Channel} > /dev/null
				sprintf(&input[0], "iwconfig wlan0 essid %s key s:%s channel %s > /dev/null", wlan0ssid, wlan0pass, wlan0channel);
				system(input);
				sprintf(&input[0], "sleep %d", wifiretryinterval);
				system(input);
			}
#endif
//      if(wiredretryinterval > wifiretryinterval){
      if(strcmp(Last_Ssid, "") == 0){
				sprintf(input, "sleep %d", wiredretryinterval);
      }else{
				sprintf(input, "sleep %d", wifiretryinterval);
      }
			system(input);
			Index++;
		}else{
			//### Success & Restert###
			if(TSCM_Result == 110){
				// Restart request
				ret = get_info("pidof restart-required", work);
				if(ret != TZCS_TRUE){ 
					system("restart-required &");
				}
			}
			if(LockState == 1){
				LockState = 0;
				system("printf \"0\" > /root/ActModeFlag");
				ret = get_info("pidof png-display", work);
				if(ret == TZCS_TRUE){ 
					//###kill ${PID}###
					sprintf(&input[0], "kill %s", work);
					system(input);
				}
				//###DisplayInfo2=$(cat /etc/X11/display.conf)
				fp = fopen("/etc/X11/display.conf", "r");
				if (fp != NULL) {
					fgets(displayinfo2, sizeof(displayinfo2), fp);
					fclose(fp);
				}else{
					displayinfo2[0] = '\0';
				}
				if ((strlen(displayinfo1) != 0) && (strlen(displayinfo2) != 0)){
					if(strcmp(displayinfo1, displayinfo2) != 0){
						if(strcmp(displayinfo2, "1") == 0 ){
							if(strcmp(displayinfo1, "5") != 0){
								system("xrandr `xrandr --profile extend`");
								system("xrandr `xrandr --profile switch`");
							}
						}else if(strcmp(displayinfo2, "2") == 0 ){
							system("xrandr `xrandr --profile single`");
						}else if(strcmp(displayinfo2, "3") == 0 ){
							system("xrandr `xrandr --profile external`");
						}else if(strcmp(displayinfo2, "4") == 0 ){
							system("xrandr `xrandr --profile clone`");
						}else if(strcmp(displayinfo2, "5") == 0 ){
							system("xrandr `xrandr --profile extend`");
						}
					}
				}
			}
			//###sleep $PingCheckInterval###
			sprintf(&input[0], "sleep %d", pingcheckinterval);
			system(input);
			Index = 0;
			FirstPolling = 1;
			//### Tzcs mode get client state set ###
			TzcsMode=2;
		}
	}
	return 0;
}



// 2017/05/22 start
////#========================================================
////# function  http_header_check()
////#
////# IN  non
////#
////# OUT  0 : OK
////#      1 : NG
////#
////#========================================================
//int http_header_check()
//{
//
//  int ret = 1;
//  int connecting_find_flg = 0;
//  FILE *fp;
//  char string[256];
//  char data[3000];
//
//#ifdef TZCS_DEBUG
//    printf("http_header_check\n");
//    system("cat http-head.log");
//#endif
//  fp = fopen("http-head.log", "r");
//  if(fp != NULL){
//#ifdef TZCS_DEBUG
//    printf("http-head.log file open ok\n");
//#endif
//    memset(data, 0, sizeof(data));
//    while(fgets(string, 256, fp) != NULL) {
//      if(strstr(string ,"Connecting to") != NULL){
//          connecting_find_flg = connecting_find_flg + 1;
//      }
//      if(connecting_find_flg >= 2){
//          break;
//      }
//      strcat(data, string);
//    }
//    fclose(fp);
//#ifdef TZCS_DEBUG
//    printf("********** data ******************\n");
//    printf("%s",data);
//#endif
//    if(strstr(data ,"200 OK") != NULL){
//      ret = 0;
//    }
//  }
//#ifdef TZCS_DEBUG
//    printf("result = %d \n",ret);
//    system("sleep 5");
//#endif
//  return(ret);
//}
// 2017/05/22 end


//#========================================================
//# function  TSCM_Connect_Chk()
//#
//# IN  $1 : Mode
//#           1 : GetClient
//#           2 : GetClientState
//#           3 : GetClientAll
//#     $2 : TSCS Server(1 or 2)
//#     $3 : try count
//#     $4 : connect timer
//#
//# OUT  TSCM_Result
//#
//#========================================================
int TSCM_Connect_Chk(int mode, char *server, char *trycount, char *connecttimer)
{
	int result=0,ret;
	int TSCM_Result = 0;
	struct stat st;
  int NetworkState = 0;
	char LanCableState[100];
	char input[100];

	//### Nework check ###
	get_info(networkwired, LanCableState);
	if(WEXITSTATUS(system("iwconfig wlan0 | grep -q Not-Associated")) != 0){
    // wifi connect
    NetworkState = 2;
  }else if((strcmp(LanCableState, "up\n") == 0) && 
           (WEXITSTATUS(system("ifconfig eth0 | grep -q \'inet addr\'"))  == 0)){
    // Wired LAN connect
    NetworkState = 1;
  }else{
    // Network is broken
    NetworkState = 0;
  }

	if(NetworkState == 0){
		//### network disconnect ###
		//### error set ###
		TSCM_Result = 157;
#ifdef TZCS_DEBUG
printf("TSCM_Connect_Chk network NG \n");
#endif
	}else{
		if(mode == 2){
			//### GetClient state ###
			//###GetClientStatus -server ${2} -count ${3} -timer ${4}   > /dev/null 2>&1###
			sprintf(&input[0], "GetClientStatus -server %s -count %s -timer %s >/dev/null 2>&1", server, trycount, connecttimer);
			result=system(input);
		}else if(mode == 3){
			//### GetClient all (Bios policy update non)###
			//###GetClientAll -server ${2} -count ${3} -timer ${4} -update 0  > /dev/null 2>&1###
			sprintf(&input[0], "GetClientAll -server %s -count %s -timer %s -update 0 > /dev/null 2>&1", server, trycount, connecttimer);
			result=system(input);
		}else{
			//### GetClient ###
			//###GetClient -server ${2} -count ${3} -timer ${4}   > /dev/null 2>&1###
			sprintf(&input[0], "GetClient -server %s -count %s -timer %s > /dev/null 2>&1", server, trycount, connecttimer);
			result=system(input);
		}
		TSCM_Result=WEXITSTATUS(result);
#ifdef TZCS_DEBUG
printf("polling comand = %s\n",input);
#endif
	}
	
	ret = stat("./ExchData",&st);
	if(ret == 0){
		ret = remove("./ExchData");
	}
	ret = stat("./PostData.xml",&st);
	if(ret == 0){
		ret = remove("./PostData.xml");
	}

	//###printf "${TSCM_Result}"###
#ifdef TZCS_DEBUG
  printf("tzcs ret = %d\n",TSCM_Result);
#endif
	return(TSCM_Result);
}


