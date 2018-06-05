/************************************************
 * bios_update.c
 *
 * Copyright 2016 Toshiba corporation
 *
 ************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <tzcslib/TzcsUtil.h>
#include <tzcslib/TzcsDisplay.h>

//nakata modify 170720
// for LE+
	const char* TargetBiosVer1_LE="v2.1Zst00";
	const char* TargetBiosVer2_LE="v2.1Zst01";
	const char* TargetBiosVer3_LE="v2.1Zst02";
	const char* TargetBiosVer4_LE="v2.1Zst03";
	const char* TargetBiosVer5_LE="v2.1Zst04";
	const char* TargetBiosVer6_LE="v2.1Zst05";
	const char* TargetBiosVer7_LE="v2.1Zst06";
	const char* TargetBiosVer8_LE="v2.1Zst08";
	const char* TargetBiosVer9_LE="v2.1Zst09";
	const char* TargetBiosVer10_LE="v2.1Zst10";
	const char* TargetBiosVer11_LE="v2.1Zst11";
	const char* TargetBiosVer12_LE="v2.1Zst12";
	const char* TargetBiosVer13_LE="v2.1Zst13";
	const char* TargetBiosVer14_LE="v2.1Zst14";
	const char* TargetBiosVer15_LE="v2.1Zst17";
	const char* TargetBiosVer16_LE="v2.1Zst19";
	const char* TargetBiosVer17_LE="v2.1Zst20";
	const char* TargetBiosVer18_LE="v2.1Zst21";
	const char* TargetBiosVer19_LE="v2.1Zst22";
	const char* TargetBiosVer20_LE="v2.1Zst23";
	const char* TargetBiosVer21_LE="v2.1Zst24";
	const char* TargetBiosVer1_LE_SS="v2.1Zss03";
	const char* TargetBiosVer2_LE_SS="v2.1Zss04";
//	const char* TargetBiosVer5="v2.1ZAZ05";
	const char* TargetBiosID_LE="00A5";
	const char* BiosUpFileName_LE="Z00A5v21Zss05";
//nakata end 170720

//nakata add 170720
// for UX20
	const char* TargetBiosVer1_UX20="v3.1Zst16";
	const char* TargetBiosVer2_UX20="v3.1Zst17";
	const char* TargetBiosVer3_UX20="v3.1Zst19";
	const char* TargetBiosVer4_UX20="v3.1Zst20";
	const char* TargetBiosVer5_UX20="v3.1Zst21";
	const char* TargetBiosVer6_UX20="v3.1Zst22";
	const char* TargetBiosVer7_UX20="v3.1Zst23";
	const char* TargetBiosVer8_UX20="v3.1Zst24";
	const char* TargetBiosVer1_UX20_SS="v3.1Zss03";
	const char* TargetBiosVer2_UX20_SS="v3.1Zss04";
//	const char* TargetBiosVer2="v3.1Zst17";
	const char* TargetBiosID_UX20="00A4";
	const char* BiosUpFileName_UX20="Z00A4v31Zss05";
//nakata end 170720

// for new SE30/LE30
	const char* TargetBiosVer1_E30_SS="v4.4Zst01";
	const char* TargetBiosVer2_E30_SS="v4.4Zst02";
	const char* TargetBiosVer3_E30_SS="v4.4Zst03";
	const char* TargetBiosVer4_E30_SS="v4.4Zss04";
	const char* TargetBiosID_E30_SS="00B3";
	const char* BiosUpFileName_E30_SS="Z00B3v44Zss05";

	const char* ac_adapter = "cat /proc/acpi/ac_adapter/ADP1/state | awk \'{print $2}\'";
    const char* bat_current = "cat /proc/acpi/battery/BAT1/state | awk \'{if (NR==5) {print $3}}\'";
    const char* batfull = "cat /proc/acpi/battery/BAT1/info | awk \'{if (NR==3) {print $4}}\'";

	enum WarningType{
		W_BiosUp_Power_Adapter = 0,
		W_BiosUp_Battery_Level,
		W_BiosUp_Download_failed,
		W_BiosUp_Unzip_failed,
		W_BiosUp_Decryption_failed,
		W_BiosUp_failed
	};

	int InputKey = 0;

//#========================================================
//# function  get_data()
//#
//#
//#   arg1 = cmd
//#========================================================
int get_data(const char *cmd)
{
	int result=0;
	FILE *fp;
	char path[100];

	/* Open the command for reading. */
	fp = popen(cmd, "r");
	if (fp == NULL) {
		printf("Failed to run command #001\n");
		return 0;
	}
	/* Read the output a line at a time - output it. */
	while (fgets(path, sizeof (path) - 1, fp) != NULL) {
				result = atoi(path);
	}
	/* close */
	pclose(fp);
	return result;
}

//#========================================================
//# function  disp_warning()
//#   warning message 
//#
//#   arg1 = Warning type
//#========================================================
void disp_warning(enum WarningType warning)
{

	//### display clear ###
	system("clear");

	//### Warning type check ###
	switch(warning){
		case W_BiosUp_Power_Adapter:
				//### Bios update : Power adapter ###
				warning_display(DISPLAY_WARNING_004);
				break;
		case W_BiosUp_Battery_Level:
				//### Bios update : Battery Level###
				warning_display(DISPLAY_WARNING_005);
				//###set_hci err_log_record 1005###
				break;
		case W_BiosUp_Download_failed:
				//### Bios update : Download failed ###
				warning_display(DISPLAY_WARNING_006);
				//###set_hci err_log_record 1006###
				break;
		case W_BiosUp_Unzip_failed:
				//### Bios update : unzip failed ###
				warning_display(DISPLAY_WARNING_007);
				//###set_hci err_log_record 1007###
				break;
		case W_BiosUp_Decryption_failed:
				//### Bios update : ###
				warning_display(DISPLAY_WARNING_008);
				//###set_hci err_log_record 1008###
				break;
		case W_BiosUp_failed:
				//### Bios update : ###
				warning_display(DISPLAY_WARNING_009);
				//###set_hci err_log_record 1009###
				break;
		default:
				break;
	}

	system("stty -echo -icanon"); 
 	while(1){
		//###system("read -n 1 -s InputKey");###
		InputKey = getchar(); 
		if(InputKey == 'r' || InputKey == 's'){
			break;
		}else if(InputKey == 'h'){
			system("poweroff");
		}
	}
	system("stty echo icanon");
}



//#========================================================
//# function  bios_update()
//#   bios update
//#
//#
//#========================================================
void bios_update (void)
{
	int result = 0;
	unsigned int Battery;

	ULONG Language;
	char DL_IP[255];
	char BIOS_ID[20];

	char BIOS_VER[10];
	char input[100];
	int  KeyDebug;

//nakata add 170720
    char BiosUpFileName[255];
    memset(BiosUpFileName,'\0', sizeof(BiosUpFileName));
//nakata end 170720
 
//######### Info Get #############
	get_hci_value(VAL_LANGUAGE, &Language);
	get_hci_string(VAL_FTP_IP, &DL_IP[0]);
	get_hci_string(VAL_BIOS_ID, &BIOS_ID[0]);
	get_hci_string(VAL_BIOS_VER, &BIOS_VER[0]);

//######### Bios Update #############

	//### Bios ID Check ###
//nakata modify 170720
	if( (strcmp(&BIOS_ID[0], TargetBiosID_LE) == 0) ){
		if( (strcmp(&BIOS_VER[0], TargetBiosVer1_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer2_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer3_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer4_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer5_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer6_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer7_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer8_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer9_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer10_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer11_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer12_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer13_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer14_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer15_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer16_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer17_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer18_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer19_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer20_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer21_LE) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer1_LE_SS) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer2_LE_SS) == 0) ){
			strcpy(BiosUpFileName,BiosUpFileName_LE);
		}
		//### Bios ID Check ###
	} else if( (strcmp(&BIOS_ID[0], TargetBiosID_UX20) == 0) ){
		if( (strcmp(&BIOS_VER[0], TargetBiosVer1_UX20) == 0 )
			|| (strcmp(&BIOS_VER[0], TargetBiosVer2_UX20) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer3_UX20) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer4_UX20) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer5_UX20) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer6_UX20) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer7_UX20) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer8_UX20) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer1_UX20_SS) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer2_UX20_SS) == 0) ){
			strcpy(BiosUpFileName,BiosUpFileName_UX20);
		}
	} else if( (strcmp(&BIOS_ID[0], TargetBiosID_E30_SS) == 0) ){
		if( (strcmp(&BIOS_VER[0], TargetBiosVer1_E30_SS) == 0 )
			|| (strcmp(&BIOS_VER[0], TargetBiosVer2_E30_SS) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer3_E30_SS) == 0)
			|| (strcmp(&BIOS_VER[0], TargetBiosVer4_E30_SS) == 0) ){
			strcpy(BiosUpFileName,BiosUpFileName_E30_SS);
		}
	}
//nakata end 170720

//nakata add 170720
	if( strlen(BiosUpFileName) != 0 ){
//nakata end 170720

		//### Screen off disable ###
		system("setterm -blank 0");

		//### Key enable ###
		if( get_env_variable(DEBUG_MODE) == DEBUG_ON){
			key_setting(KEY_ALL_ENABLE);
		}else{
			key_setting(KEY_CTL_DISABLE);
		}
		while(1){
			//### Display clear ###
			system("clear");
			//### BIOS update confirmation screen ###
			bmp_display(BMP_UPDATE_FIRMWARE);
			//### [Enter]key is pressed wait ###
			system("read -s");
			//### Power Adapter Check ###
			get_info(ac_adapter, input);
			if(strstr((const char *)input, "on-line") == NULL){
				//### Off-line : Warning Display ###
				disp_warning(W_BiosUp_Power_Adapter);
			}else{
				//### Battery level check ###
				Battery = (get_data(bat_current)*100)/get_data(batfull);

				if( Battery <= 10){
					//### More than 10% : Warning Display ###
					disp_warning(W_BiosUp_Battery_Level);
				}else{
					//### Display clear ###
					system("clear");
		            //### Screen off enable ###
		            system("setterm -blank 5");
		            //### Display the BIOS updating ###
					bmp_display(BMP_UPDATING_FIRMWARE);
					//### Bios data downloard ###
					//###wget -q -T 60 -t 3 ${DL_IP}/${BiosUpFileName}.zip > /dev/null###
					sprintf(&input[0], "wget -q -T 60 -t 3 %s/%s.zip > /dev/null", DL_IP, BiosUpFileName);
					result = system(input);
					if(WEXITSTATUS(result) != 0){
						//### Download failed : Warning Display ###
						disp_warning(W_BiosUp_Download_failed);
					}else{
						//### Bios data unzip ###
						//###unzip -P Toshiba! ${BiosUpFileName}.zip > /dev/null###
						sprintf(&input[0], "unzip -P Toshiba! %s.zip > /dev/null", BiosUpFileName);
						result = system(input);
						if(WEXITSTATUS(result) != 0){
							//###rm ${BiosUpFileName}.* > /dev/null 2>&1###
							sprintf(&input[0], "rm %s.* > /dev/null 2>&1", BiosUpFileName);
							system(input);
							//### Unzip failed : Warning Display ###
							disp_warning(W_BiosUp_Unzip_failed);
						}else{
							//###rm ${BiosUpFileName}.zip###
							sprintf(&input[0], "rm %s.zip", BiosUpFileName);
							system(input);
							//### Bios data Decryption ###
			                //###tenco d1 ${BiosUpFileName}.tcf > /dev/null###
							sprintf(&input[0], "tenco d1 %s.tcf > /dev/null", BiosUpFileName);
							result = system(input);
							if(WEXITSTATUS(result) != 0){
								//###rm ${BiosUpFileName}.* > /dev/null 2>&1###
								sprintf(&input[0], "rm %s.* > /dev/null 2>&1", BiosUpFileName);
								system(input);
								//### Decryption failed : Warning Display ###
								disp_warning(W_BiosUp_Decryption_failed);
							}else{
								//### Reboot log no screen ###
								//### Bios update start ###
								//###tenco bupr ${BiosUpFileName}.bin > /dev/null 2>&1###
								sprintf(&input[0], "tenco bupr %s.bin > /dev/null 2>&1", BiosUpFileName);
								system(input);
								//### error wait ###
								system("sleep 5");
								//### Bios update failed : Warning Display ###
								disp_warning(W_BiosUp_failed);
								//###rm ${BiosUpFileName}.* > /dev/null 2>&1###
								sprintf(&input[0], "rm %s.* > /dev/null 2>&1", BiosUpFileName);
								system(input);
							}
						}
					}
				}
			}
			if(InputKey == 's'){
				system("clear");
				break;
			}
		}
		//### Key disable ###
		KeyDebug = get_env_variable(DEBUG_MODE);
		if(KeyDebug != DEBUG_ON){
			//###loadkmap < /etc/keymaps/dis-key.bmap###
			key_setting(KEY_ALL_DISABLE);
		}
	}
}


