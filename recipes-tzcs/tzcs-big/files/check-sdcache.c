/************************************************
 * check_sdcache.c
 *
 * Copyright 2016-2018 Toshiba corporation
 *
 ************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include <tzcslib/TzcsUtil.h>

// -Add START [SD cache progress] 2017/08/24 Shimizu 
///// for GetText /////
#include <libintl.h>

#define _(String)gettext(String)
#ifdef gettext_noop
 #define N_(String) gettext_noop(String)
#else
 #define N_(String)(String)
#endif

///// disp messages /////
#define	SDMSG_START		0
#define	SDMSG_ERR		1
#define	SDMSG_ERR001	2
#define	SDMSG_ERR014	3	
#define	SDMSG_END		4
#define	SDMSG_REPLACE	5


void exe_disp_sts(int num)
{
	char	disp_cmd[200];
	char	*msg;

	switch(num){
		case SDMSG_START:
			msg = N_(" System cache is being created. \n Please do not remove SD card until it is completed. ");
			break;

		case SDMSG_ERR:		// Change ERROR message 2018/02/27 shimizu
			msg = N_(" *** Error (E050) : System cache could not be created. *** \n Failed to access a SD card. Please check inserted SD card.");
			break;

		case SDMSG_ERR001:	// Change ERROR message 2018/02/27 shimizu
			msg = N_(" *** Error (E051) : System cache could not be created. *** \n Failed to open a downloaded data from TMZC authentication server. \n Please contact your administrator.");
			break;

		case SDMSG_ERR014:	// Change ERROR message 2018/02/27 shimizu
			msg = N_(" *** Error (E052) : System cache could not be created. *** \n Failed to copy a downloaded data to a SD card. \n Please check inserted SD card.");
			break;

		case SDMSG_END:
			msg = N_(" Creation of the system cache has been completed. ");
			break;

		case SDMSG_REPLACE:
			msg = N_(" Creation of the system cache has been completed. \n (The old cache that existed on the SD card was deleted.) ");
			break;

		default:
			msg = "";
			break;
	}

	sprintf(disp_cmd, "disp-status '%s' & > /dev/null", _(msg));
	system(disp_cmd);
	return;
}
// -Add END [SD cache progress] 2017/08/24 Shimizu 

// -mod SD old data delete 2017-02-15
int	sd_old_bigcore_del(char *big_core_name)		// -mod [SD cache progress] 2017/08/24 Shimizu 
{
  DIR *dir;
  struct dirent *dent;
  char sd_data_name[512];
  char sd_modules_name[512];
  char command[512];
  char *p;
  int len;
  int ret = 0;		// -Add [SD cache progress] 2017/08/24 Shimizu

	sprintf(sd_data_name   ,"%s-data.tcf"   ,big_core_name);
	sprintf(sd_modules_name,"%s-modules.tcf",big_core_name);

  // SD directory open
  dir = opendir("/mnt");
  if (dir == NULL) {
  return 0;		// -Mod [SD cache progress] 2017/08/24 Shimizu
  }

	// SD cache data check
	while ((dent = readdir(dir)) != NULL) {

		if (dent->d_type != DT_DIR) {
			// not directork
			p = &dent->d_name[0];
			len = strlen((const char *)dent->d_name);
			if (memcmp(dent->d_name,"TZCS-",5) == 0) {
				if (strcmp((const char *)p + (len - 9),"-data.tcf") == 0) {
					if (strcmp(dent->d_name,sd_data_name) != 0) {
						// SD cache data delete
						sprintf(command,"rm /mnt/%s",dent->d_name);
						system(command);
						ret++;		// -Add [SD cache progress] 2017/08/24 Shimizu
					}
				}else if (strcmp((const char *)p + (len - 12),"-modules.tcf") == 0) {
					if (strcmp(dent->d_name,sd_modules_name) != 0) {
						// SD cache module delete
						sprintf(command,"rm /mnt/%s",dent->d_name);
						system(command);
						ret++;		// -Add [SD cache progress] 2017/08/24 Shimizu
					}
				}
			}
		}
	}
	closedir(dir);

	return ret;		// -Add [SD cache progress] 2017/08/24 Shimizu
}
// -mod SD old data delete 2017-02-15

// -modify [Option](TZCS (Base))  2017/03/08 
int	sd_old_option_del(char *option_data_name)		// -Mod [SD cache progress] 2017/08/24 Shimizu
{
  DIR *dir;
  struct dirent *dent;
  char sd_option_name[512];
  char command[512];
  char *p;
  int len;
  int ret = 0;		// -Add [SD cache progress] 2017/08/24 Shimizu

	sprintf(sd_option_name   ,"TZCS-%s-Option.tcf"   ,option_data_name);

  // SD directory open
  dir = opendir("/mnt");
  if (dir == NULL) {
	return 0;		// -Mod [SD cache progress] 2017/08/24 Shimizu
  }

	// SD cache data check
	while ((dent = readdir(dir)) != NULL) {

		if (dent->d_type != DT_DIR) {
			// not directork
			p = &dent->d_name[0];
			len = strlen((const char *)dent->d_name);
			if (memcmp(dent->d_name,"TZCS-",5) == 0) {
				if (strcmp((const char *)p + (len - 11),"-Option.tcf") == 0) {
					if (strcmp(dent->d_name,sd_option_name) != 0) {
						// SD cache data delete
						sprintf(command,"rm /mnt/%s",dent->d_name);
						system(command);
						ret++;		// -Add [SD cache progress] 2017/08/24 Shimizu
					}
				}
			}
		}
	}
	closedir(dir);

	return ret;		 // -Add [SD cache progress] 2017/08/24 Shimizu
}
// -modify [Option](TZCS (Base))  2017/03/08 

// -Modify START [wait "loading"] 2018/01/09 Shimizu
void wait_loading_display(void)
{
  char p_id[100];
  int sec, ret;

//  while(1) {
  for (sec = 0; sec < 60; sec++) {		// If over 60sec then Timeout... 
	ret = get_info("pidof loading" , p_id);
	if (ret == 1) {
		break;
	}
	system("sleep 1");
  }
}
// -Modify END [wait "loading"] 2018/01/09 Shimizu


//#========================================================================
//# main
//#========================================================================
int main(int argc , char *argv[])
{
	int result = 0;
	int result1 = 0;
	ULONG hci_out = 0;
	char	input[256];
	char	DownloadDataName[64*2];
// -modify [Option](TZCS (Base))  2017/03/08 
	char	DownloadOptionName[64*2];
	char	DownloadOptionNameZip[64*2];
	int   big_core_save = 1;
// -modify [Option](TZCS (Base))  2017/03/08 
	struct stat st;
// -Add START [SD cache progress] 2017/08/24 Shimizu
	int disp_msg = SDMSG_END;	// default = "Normal END"
	ULONG hide_progress; 

	///// for GetText /////
	setlocale(LC_ALL, "");
	textdomain("check-sdcache");

	if (get_hci_value(VAL_HIDE_SD_CACHE_PROG, &hide_progress) == HCI_FAIL) {
		hide_progress = NOT_HIDE_PROGRESS;
	}
// -Add END [SD cache progress] 2017/08/24 Shimizu
	
	memset(DownloadDataName, 0, sizeof(DownloadDataName));
	hci_out = get_hci_string(VAL_FTP_DATANAME,DownloadDataName );
// -modify [Option](TZCS (Base))  2017/03/08 
	memset(DownloadOptionName, 0, sizeof(DownloadOptionName));
	hci_out = get_hci_string(VAL_OPTION_DATA_NAME,DownloadOptionName );
// -modify [Option](TZCS (Base))  2017/03/08 

	memset(input, 0, sizeof(input));
// -modify [Option](TZCS (Base))  2017/03/08 
//	result = stat("/tmp/bigcore/data.tar.gz",&st);
//	if(result == 0){
//		result = stat("/tmp/bigcore/modules.tgz",&st);
//		if(result == 0){
	//***** SD save folder check *****
	if(stat("/tmp/bigcore",&st) == 0){
// -modify [Option](TZCS (Base))  2017/03/08 
// -Add START [SD cache progress] 2017/08/24 Shimizu
		if (hide_progress == NOT_HIDE_PROGRESS) {
// -Modify START [wait "loading"] 2018/01/09 Shimizu
			system("sleep 3");
			wait_loading_display();
// -Modify END [wait "loading"] 2018/01/09 Shimizu
			exe_disp_sts(SDMSG_START);
		}
// -Add END [SD cache progress] 2017/08/24 Shimizu
		result = system("mount -t vfat /dev/mmcblk0p1 /mnt > /dev/null 2>&1");
		if(WEXITSTATUS(result) == 0){
			//### SD mount OK ###
// -modify [Option](TZCS (Base))  2017/03/08 
			//***** Big-core data check *****
			if((stat("/tmp/bigcore/data.tar.gz",&st) == 0) && (stat("/tmp/bigcore/modules.tgz",&st) == 0)){
// -modify [Option](TZCS (Base))  2017/03/08 
				result = system("tenco ea /tmp/bigcore/data.tar.gz > /dev/null");
				result1 = system("tenco eb /tmp/bigcore/modules.tgz > /dev/null");
				if((WEXITSTATUS(result) == 0) && (WEXITSTATUS(result1) == 0)){
					//### Copy the encrypted file to SD ###
					sprintf(&input[0], "cp -v data.tar.gz.tcf /mnt/%s-data.tcf > /dev/null",  DownloadDataName);
					result = system(input);
					sprintf(&input[0], "cp -v modules.tgz.tcf /mnt/%s-modules.tcf > /dev/null", DownloadDataName);
					result1 = system(input);
					if((WEXITSTATUS(result) != 0) || (WEXITSTATUS(result1) != 0)){
						//### Error: Failed to SD WRITE ###
						sprintf(&input[0], "/mnt/%s-data.tcf", DownloadDataName);
						result = stat(input,&st);
						if(result == 0){
							remove(input);
						}
						sprintf(&input[0], "/mnt/%s-modules.tcf", DownloadDataName);
						result = stat(input,&st);
						if(result == 0){
							remove(input);
						}
//					}else{
						hci_out = 0x1001;
						set_hci_value(VAL_ERROR_LOG_RECORD,&hci_out);	//### (W001)
						disp_msg = SDMSG_ERR001;		// -Add [SD cache progress] 2017/08/24 Shimizu
// -mod SD old data delete 2017-02-15
					} else {
// -modify [Option](TZCS (Base))  2017/03/08 
						big_core_save = 0;
// -modify [Option](TZCS (Base))  2017/03/08 
// -Mod START [SD cache progress] 2017/08/24 Shimizu
//						sd_old_bigcore_del(DownloadDataName);
						result = sd_old_bigcore_del(DownloadDataName);
						if(result > 0){
							disp_msg = SDMSG_REPLACE;
						}
// -Mod END [SD cache progress] 2017/08/24 Shimizu
//					}
// -mod SD old data delete 2017-02-15
					}
				}else{
					hci_out = 0x0014;
					set_hci_value(VAL_ERROR_LOG_RECORD,&hci_out);	//### (E014)
					disp_msg = SDMSG_ERR014;		// -Add [SD cache progress] 2017/08/24 Shimizu
				}
// -modify [Option](TZCS (Base))  2017/03/08 
//				result = stat("data.tar.gz.tcf",&st);
//				if(result == 0){
//					remove("data.tar.gz.tcf");
//				}
//				result = stat("modules.tgz.tcf",&st);
//				if(result == 0){
//					remove("modules.tgz.tcf");
//				}
			}else{
				big_core_save = 0;
			}
			//***** Option data check *****
			sprintf(DownloadOptionNameZip, "/tmp/bigcore/%s.zip", DownloadOptionName);
			if((stat(DownloadOptionNameZip,&st) == 0) && (big_core_save == 0)){
				// option data save
				sprintf(input, "tenco eb %s > /dev/null", DownloadOptionNameZip);
				if(WEXITSTATUS(system(input)) == 0){
					// OK
					sprintf(input, "cp -v %s.zip.tcf /mnt/TZCS-%s-Option.tcf > /dev/null", DownloadOptionName, DownloadOptionName);
					if(WEXITSTATUS(system(input)) != 0){
						//### Error: Failed to SD WRITE ###
						sprintf(input, "/mnt/TZCS-%s-Option.tcf", DownloadOptionName);
						if(stat(input,&st) == 0){
							remove(input);
						}
						hci_out = 0x1001;
						set_hci_value(VAL_ERROR_LOG_RECORD,&hci_out);	//### (W001)
						disp_msg = SDMSG_ERR001;		// -Add [SD cache progress] 2017/08/24 Shimizu
					} else {
// -Mod START [SD cache progress] 2017/08/24 Shimizu
//						sd_old_option_del(DownloadOptionName);
						result = sd_old_option_del(DownloadOptionName);
						if(result > 0){
							disp_msg = SDMSG_REPLACE;
						}
// -Mod END [SD cache progress] 2017/08/24 Shimizu
					}
				}else{
					// NG
					hci_out = 0x0014;
					set_hci_value(VAL_ERROR_LOG_RECORD,&hci_out);	//### (E014)
					disp_msg = SDMSG_ERR014;		// -Add [SD cache progress] 2017/08/24 Shimizu
				}
			}
// -modify [Option](TZCS (Base))  2017/03/08 
			system("umount /mnt > /dev/null");
// -modify [Option](TZCS (Base))  2017/03/08 
//			}
// -modify [Option](TZCS (Base))  2017/03/08 

// -Add START [SD cache progress] 2017/08/24 Shimizu
			if (hide_progress == NOT_HIDE_PROGRESS) {
				system("killall disp-status");
				wait_loading_display();
				exe_disp_sts(disp_msg);
			}
		}
		else {
			if (hide_progress == NOT_HIDE_PROGRESS) {
				system("sleep 2");
				wait_loading_display();
				exe_disp_sts(SDMSG_ERR);		// mount NG
			}
// -Add END [SD cache progress] 2017/08/24 Shimizu
		}
	}

	result = stat("/tmp/bigcore",&st);
	if(result == 0){
// -modify [Option](TZCS (Base))  2017/03/08 
//		remove("/tmp/bigcore/data.tar.gz");
//		remove("/tmp/bigcore/modules.tgz");
//		remove("/tmp/bigcore");
		system("rm -r /tmp/bigcore");
// -modify [Option](TZCS (Base))  2017/03/08 
	}
// -modify [Option](TZCS (Base))  2017/03/08 
	if(stat("data.tar.gz.tcf",&st) == 0) remove("data.tar.gz.tcf");
	if(stat("modules.tgz.tcf",&st) == 0) remove("modules.tgz.tcf");
	sprintf(input, "%s.zip.tcf", DownloadOptionName);
	if(stat(input,&st) == 0) remove(input);
// -modify [Option](TZCS (Base))  2017/03/08 
	return 0;
}

