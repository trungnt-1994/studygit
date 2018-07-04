/************************************************
 * service.c
 *
 * Copyright 2016 Toshiba corporation
 *
 *  nakata[001] : Add Spanish and French keyboard
 *  nakata[002] : Add Arabic, Hebrew, Danish, Finnish & Swedish, Norwegian and Portuguese keyboard
 ************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h> 
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>

#include <tzcslib/TzcsUtil.h>
#include <tzcslib/TzcsDisplay.h>

void Firefox_StartPage_Set(void);
void bios_update(void);
void rdesktop_info_input_dialog(void);
void retry_rdesktop_info_input_dialog(void);
// -modify [Option](TZCS (Base))  2017/03/08 
void	option_data_set(void);
int		option_cert_data_set(const char *folder,const char *client_folder);
// -modify [B-Note 5308-8](TZCS 1.3)  2017/08/01 
//void	option_special_data_set(void);
int		option_special_data_set(void);
// -modify-end [B-Note 5308-8](TZCS 1.3)  2017/08/01 
void control_code_conversion(char *p);
// -modify [Option](TZCS (Base))  2017/03/08 

	ULONG ManufactMode;
	char DL_IP[255];
	char SERIAL[10];
	ULONG TestMode;
	ULONG VdiInfo;
	char VdiUrl[255];
	ULONG VPNOnOff;
	ULONG Language;
		
	char RdeskTop_URL[255];
	char RdeskTop_NAME[255];
	char RdeskTop_PASSWORD[255];
	char RdeskTop_DOMAIN[255];
	char RdeskTop_OPTION[255];
	
	const char *salk_cert_file = "SALK-RootCA.pem";
	const char *salk_cert_hash_file = "28330ff5.0";

#define DIALOG_OK        0
#define DIALOG_ESC       255


//#define TZCS_DEBUG

//#define TZCS_DEBUG_TEST

//#========================================================
//# function  main()
//#
//#
//#
//#========================================================
int main (int argc , char *argv[])
{
  int fd;
	FILE *fp;
	FILE *fp2;
	int result=0;
	int PcKeybord;
	int i;
	char input[300];
  char p_id[STR_BUF_SIZE];
	char tzcs_domain[STR_BUF_SIZE];
	char netbios_domain[STR_BUF_SIZE];
	const char *redirect_url_e  = "http://www.toshiba.com";
	const char *redirect_url_j  = "http://www.toshiba.co.jp";
///// -Add [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka
	struct stat st;
	unsigned char   *split;
///// -End [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka


	memset(input, 0, sizeof(input));

	//######### TVAL install #############
	// tval open
	fd = open("/dev/tvalz0", O_RDONLY);
	if (fd < 0) {
		//error
		system("/etc/init.d/dev_install.sh");
	}else{
		close(fd);
	}
//	system("cd /root");

	//######### Info get #############
	get_hci_value(VAL_MANUFACT_MODE, &ManufactMode);
//#ifdef TZCS_DEBUG_TEST
//  ManufactMode = MANUFACT_ON;
//#endif
	get_hci_value(VAL_TEST_MODE, &TestMode);
///// -Add [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka
	if (get_env_variable(VENDER_AUTH_MODE) == VENDER_AUTH_VMWARE){
		VdiInfo = VDI_VMWARE;
	} else if (get_env_variable(VENDER_AUTH_MODE) == VENDER_AUTH_CITRIX){
	VdiInfo = VDI_CITRIX;
	} else {
///// -End [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka
	get_hci_value(VAL_VDI_PROFILE, &VdiInfo);
///// -Add [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka
	}
///// -End [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka
	get_hci_value(VAL_VPN_ONOFF, &VPNOnOff);
	get_hci_value(VAL_LANGUAGE, &Language);

	get_hci_string(VAL_FTP_IP,&DL_IP[0]);
	get_hci_string(VAL_TSCM_SERIAL,&SERIAL[0]);
///// -Add [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka
	if (get_env_variable(VENDER_AUTH_MODE) == VENDER_AUTH_VMWARE){
		memset(VdiUrl, '\0', sizeof(VdiUrl));
		if (stat("/Option/VDISVINFO/VDISVURL.CFG", &st) == 0) {
			fp = fopen("/Option/VDISVINFO/VDISVURL.CFG", "r");
			if (fp != NULL) {
				while (fgets(input, sizeof(input), fp) != NULL) {
					control_code_conversion(input);
					if ((split = (unsigned char *)strstr((const char *)input, "VDI_Server_URL=")) != NULL) {
						strncpy(VdiUrl, (const char *)(split + strlen("VDI_Server_URL=")), strlen((const char *)split) - strlen("VDI_Server_URL="));
						break;
					}
				}
			}
		}
		if (stat("/Option/VDISVINFO/.pcoip.rc", &st) == 0) {
			system("cp /Option/VDISVINFO/.pcoip.rc /root/. > /dev/null 2>&1");
		}
	} else {
///// -End [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka
	get_hci_string(VAL_VDI_URL,&VdiUrl[0]);
///// -Add [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka
	}
///// -End [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka

	get_hci_string(VAL_DOMAIN_NAME,tzcs_domain);

	//######### font settings #############
    setting_font();

// -modify [Option](TZCS (Base))  2017/03/08 
	//######### Option data setting #############
	option_data_set();
// -modify [Option](TZCS (Base))  2017/03/08 

	//######### BIOS Update #############
	bios_update();

	//######### .xinitrc([vmware-view &] and [poweroff] del) #############
	if(VdiInfo == VDI_VMWARE){
		system("sed -i \'/vmware-view/d\' /root/.xinitrc");
		system("sed -i \'/poweroff/d\' /root/.xinitrc");
	}

	//######### TrebootL check #############
	if((TestMode == 1) || (TestMode == 2)){
		display_clear();
		printf("TrebootL Counter\n");
		system("setpontime");
		sleep(5);
		fp = fopen("/root/.xinitrc", "a");
		if (fp != NULL) {
			fprintf(fp,"exec heatrun &\n");
			fclose(fp);
		}
	}

	//######### Debug check #############
	if (get_env_variable(DEBUG_MODE) == DEBUG_ON ) {
		fp = fopen("/usr/share/icewm/keys", "a");
		if (fp != NULL) {
			fprintf(fp,"key \"Alt+Ctrl+t\"			xterm\n");
			fclose(fp);
		}
	}

///// -Add [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka
	if (get_env_variable(VENDER_AUTH_MODE) == VENDER_AUTH_OFF){
///// -End [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka
	if(ManufactMode == MANUFACT_OFF){
		//######### tzcs-polling setting #############
		system("sed -i \'/eula /i\\tzcs-polling &\' /root/.xinitrc");
		fp = fopen("/root/.xinitrc", "a");
		if (fp != NULL) {
			fprintf(fp,"batteryalarm &\n");
			fclose(fp);
		}

		//######### VDI mode check #############
		if (get_env_variable(WEB_AUTH_MODE) == WEB_AUTH_ON) {
			system("sed -i \'/eula /a\\check-webauth &\' /root/.xinitrc");
//			fprintf(fp,"check-webauth &\n");
			if(Language == JAPANESE){
				sprintf(input, "sed -i \'/check-webauth &/a\\firefox %s\' /root/.xinitrc", redirect_url_j);
				fprintf(fp,"firefox %s\n",redirect_url_j);
			}else{
				sprintf(input, "sed -i \'/check-webauth &/a\\firefox %s\' /root/.xinitrc", redirect_url_e);
				fprintf(fp,"firefox %s\n",redirect_url_e);
			}
			system(input);
		}
	}
///// -Add [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka
	}
///// -End [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka
	if(VdiInfo == VDI_RDP){
		system("cp /root/.xinitrc /root/xinitrc2 > /dev/null");
		system("clear");
		rdesktop_info_input_dialog();
		system("clear");
	}
	fp = fopen("/root/.xinitrc", "a");
	if (fp != NULL) {
		if(VdiInfo == VDI_VMWARE){
			// VNware
			Firefox_StartPage_Set();
			result = system("grep \"browser.startup.homepage\" /root/.mozilla/firefox/v18xwu6c.default/user.js > /dev/null");
			if(WEXITSTATUS(result) == 0){
				fprintf(fp,"firefox &\n");
			}
			fprintf(fp,"tzcs-vmware-view\n");
//			if (netbios_domain[0] == '\0') {
//				fprintf(fp,"vmware-view -s %s\n",VdiUrl);
//			} else {
//				fprintf(fp,"vmware-view -d %s -s %s\n",netbios_domain,VdiUrl);
//			}
			// default server set
//			fp2 = fopen("/root/.vmware/view-recent-brokers", "w");
			fp2 = fopen("/root/.vmware/view-recent-brokers", "a");
			if (fp2 != NULL) {
				fprintf(fp2,"%s\n",VdiUrl);
				fclose(fp2);
			}
			// Net bios domain name set
			memset(netbios_domain, '\0', sizeof(netbios_domain));
			for (i = 0; i < STR_BUF_SIZE; i++) {
				if (tzcs_domain[i] == '\0' || tzcs_domain[i] == '.') break;
				netbios_domain[i] = tzcs_domain[i];
			}
			if (netbios_domain[0] != '\0') {
//				fp2 = fopen("/root/.vmware/view-preferences", "w");
				fp2 = fopen("/root/.vmware/view-preferences", "a");
				if (fp2 != NULL) {
					fprintf(fp2,"view.defaultDomain = \"%s\"\n",netbios_domain);
					fclose(fp2);
				}
			}
			if (stat("log_get_mode", &st) != 0) {
				fprintf(fp,"poweroff\n");
			}







#if 0 //del haya 
/***** -add [EAP-TLS](ZCS (1.2)) 2016/12/15 Shimizu *****/
			if(Language != GERMAN){
				system("rm /usr/share/locale/en_GB/LC_MESSAGES/vmware-view.mo  > /dev/null 2>&1");
			}
/***** -end [EAP-TLS](ZCS (1.2)) 2016/12/15 Shimizu *****/
#endif //del haya 
		} else if(VdiInfo == VDI_RDP){
			if (strlen(RdeskTop_OPTION) == 0) {
				if (strlen(RdeskTop_DOMAIN) == 0) {
					fprintf(fp,"xfreerdp /u:\"%s\" /p:\"%s\" /v:\"%s\" /f /bpp:32 /gdi:hw /sound:sys:alsa /sound:latency:400 /cert-ignore +fonts -glyph-cache\n",RdeskTop_NAME,RdeskTop_PASSWORD,RdeskTop_URL);
				} else {
					fprintf(fp,"xfreerdp /d:\"%s\" /u:\"%s\" /p:\"%s\" /v:\"%s\" /f /bpp:32 /gdi:hw /sound:sys:alsa /sound:latency:400 /cert-ignore +fonts -glyph-cache\n",RdeskTop_DOMAIN,RdeskTop_NAME,RdeskTop_PASSWORD,RdeskTop_URL);
				}
			} else {
				if (strlen(RdeskTop_DOMAIN) == 0) {
					fprintf(fp,"xfreerdp /u:\"%s\" /p:\"%s\" /v:\"%s\" %s\n",RdeskTop_NAME,RdeskTop_PASSWORD,RdeskTop_URL,RdeskTop_OPTION);
				} else {
					fprintf(fp,"xfreerdp /d:\"%s\" /u:\"%s\" /p:\"%s\" /v:\"%s\" %s\n",RdeskTop_DOMAIN,RdeskTop_NAME,RdeskTop_PASSWORD,RdeskTop_URL,RdeskTop_OPTION);
				}
			}
		} else {
			// Citrix
			fprintf(fp,"firefox \n");
			Firefox_StartPage_Set();
		}
		fclose(fp);
	}

	//### font default setting ###
    setting_font();

	//######### Keybord setting #############
    if (get_env_variable(DEBUG_MODE) == DEBUG_ON) {
        key_setting(KEY_ALL_ENABLE);
    } else {
        key_setting(KEY_CTL_DISABLE);
    }

	PcKeybord = get_env_variable(KEYBORD_TYPE);
	if(PcKeybord == KEYBORD_JAPAN){
		system("sed -i -e 's/\"XkbLayout\"\t\"us\"/\"XkbLayout\"\t\"jp\"/' /etc/X11/xorg.conf");
//		system("sed -i '/\"kbd\"/'a\'Option \"XkbModel\" \"jp106\"' /etc/X11/xorg.conf");
//		system("sed -i '/\"jp106\"/'a\'Option \"XkbLayout\" \"jp\"' /etc/X11/xorg.conf");
	}else if(PcKeybord == KEYBORD_UK){
		system("sed -i -e 's/\"XkbLayout\"\t\"us\"/\"XkbLayout\"\t\"gb\"/' /etc/X11/xorg.conf");
//		system("sed -i -e 's/\"XkbLayout\"\t\"us\"/\"XkbLayout\"\t\"uk\"/' /etc/X11/xorg.conf");
	}else if(PcKeybord == KEYBORD_GERMAN){
		system("sed -i -e 's/\"XkbLayout\"\t\"us\"/\"XkbLayout\"\t\"de\"/' /etc/X11/xorg.conf");
//nakata[001] add 17/10/10
	}else if(PcKeybord == KEYBOARD_SPANISH){
		system("sed -i -e 's/\"XkbLayout\"\t\"us\"/\"XkbLayout\"\t\"es\"/' /etc/X11/xorg.conf");
	}else if(PcKeybord == KEYBOARD_FRENCH){
		system("sed -i -e 's/\"XkbLayout\"\t\"us\"/\"XkbLayout\"\t\"fr\"/' /etc/X11/xorg.conf");
//nakata[001] add end 17/10/10
//nakata[002] add 18/03/08
	}else if(PcKeybord == KEYBOARD_ARABIC){
              system("sed -i -e 's/\"XkbLayout\"\t\"us\"/\"XkbLayout\"\t\"ara\"/' /etc/X11/xorg.conf");
              system("sed -i -e '/\"XkbLayout\"/ a \        Option      \"XkbVariant\" \"qwerty_digits\"' /etc/X11/xorg.conf");
	}else if(PcKeybord == KEYBOARD_DANISH){
		system("sed -i -e 's/\"XkbLayout\"\t\"us\"/\"XkbLayout\"\t\"dk\"/' /etc/X11/xorg.conf");
	}else if(PcKeybord == KEYBOARD_FINNISH_SWEDISH){
		system("sed -i -e 's/\"XkbLayout\"\t\"us\"/\"XkbLayout\"\t\"se\"/' /etc/X11/xorg.conf");
	}else if(PcKeybord == KEYBOARD_HEBREW){
		system("sed -i -e 's/\"XkbLayout\"\t\"us\"/\"XkbLayout\"\t\"il\"/' /etc/X11/xorg.conf");
	}else if(PcKeybord == KEYBOARD_NORWEGIAN){
		system("sed -i -e 's/\"XkbLayout\"\t\"us\"/\"XkbLayout\"\t\"no\"/' /etc/X11/xorg.conf");
	}else if(PcKeybord == KEYBOARD_PORTUGUESE){
		system("sed -i -e 's/\"XkbLayout\"\t\"us\"/\"XkbLayout\"\t\"pt\"/' /etc/X11/xorg.conf");
//nakata[002] add end
	}

	//### screen OFF 10sec set ###
	system("setterm -blank 10");
	
	//######### Language setting #############
	if(Language == JAPANESE){
		system("printf \"user_pref(\\\"intl.accept_languages\\\", \\\"ja,en-us,en\\\");\\n\" >> /root/.mozilla/firefox/v18xwu6c.default/user.js");
/***** -add [EAP-TLS](ZCS (1.2)) 2016/12/15 Shimizu *****/
	} else if(Language == GERMAN){
		system("printf \"user_pref(\\\"intl.accept_languages\\\", \\\"de,en-us,en\\\");\\n\" >> /root/.mozilla/firefox/v18xwu6c.default/user.js");
/***** -end [EAP-TLS](ZCS (1.2)) 2016/12/15 Shimizu *****/
	}
// -modify [B-Note 5308-6](TZCS 1.3)  2017/08/01 
 
	//######### display clear #############
	display_clear();

// -modify-end [B-Note 5308-6](TZCS 1.3)  2017/08/01 
	//######### X start #############
	if(ManufactMode == MANUFACT_OFF){
		if(VdiInfo != VDI_RDP){
#if 0 //change haya
			system("startx  > /dev/null 2>&1");
#else //change haya
			if(Language == JAPANESE){
				system("LANG=ja_JP.UTF-8 startx  > /dev/null 2>&1");
			} else if(Language == GERMAN){
				system("LANG=de_DE.UTF-8 startx  > /dev/null 2>&1");
			} else {
				system("startx  > /dev/null 2>&1");
			}
#endif //change haya
		} else {
    		while(1){
#if 0 //change haya
				system("startx  > /dev/null 2>&1");
#else //change haya
				if(Language == JAPANESE){
					system("LANG=ja_JP.UTF-8 startx  > /dev/null 2>&1");
				} else if(Language == GERMAN){
					system("LANG=de_DE.UTF-8 startx  > /dev/null 2>&1");
				} else {
				system("startx  > /dev/null 2>&1");
				}
#endif //change haya
      			get_info("ps | grep indicator | sed -e \'s/^ *//\' | cut -d \' \' -f1" , p_id );
      			if (strlen(p_id) != 0) {
        			memset(input, '\0', sizeof(input));
        			sprintf(input, "kill %s > /dev/null 2>&1",p_id);
        			system(input);
      			}
				system("cp /root/xinitrc2 /root/.xinitrc > /dev/null");
				system("clear");
				retry_rdesktop_info_input_dialog();
				system("clear");
				fp = fopen("/root/.xinitrc", "a");
				if (fp != NULL) {
					if (strlen(RdeskTop_OPTION) == 0) {
						if (strlen(RdeskTop_DOMAIN) == 0) {
							fprintf(fp,"xfreerdp /u:\"%s\" /p:\"%s\" /v:\"%s\" /f /bpp:32 /gdi:hw /sound:sys:alsa /sound:latency:400 /cert-ignore +fonts -glyph-cache\n",RdeskTop_NAME,RdeskTop_PASSWORD,RdeskTop_URL);
						} else {
							fprintf(fp,"xfreerdp /d:\"%s\" /u:\"%s\" /p:\"%s\" /v:\"%s\" /f /bpp:32 /gdi:hw /sound:sys:alsa /sound:latency:400 /cert-ignore +fonts -glyph-cache\n",RdeskTop_DOMAIN,RdeskTop_NAME,RdeskTop_PASSWORD,RdeskTop_URL);
						}
					} else {
						if (strlen(RdeskTop_DOMAIN) == 0) {
							fprintf(fp,"xfreerdp /u:\"%s\" /p:\"%s\" /v:\"%s\" \"%s\"\n",RdeskTop_NAME,RdeskTop_PASSWORD,RdeskTop_URL,RdeskTop_OPTION);
						} else {
							fprintf(fp,"xfreerdp /d:\"%s\" /u:\"%s\" /p:\"%s\" /v:\"%s\" \"%s\"\n",RdeskTop_DOMAIN,RdeskTop_NAME,RdeskTop_PASSWORD,RdeskTop_URL,RdeskTop_OPTION);
						}
					}
					fclose(fp);
				} else {
					system("poweroff");
				}
    		}
		}
	} else {
//		system("cd ..");
		system("echo pwd=$PWD");
		system("/bin/bash /root/TZCSCheckSystem/TZCSCheckSystem.sh");
	}
	return 0;
}

//#========================================================
//# function  Firefox_StartPage_Set()
//#
//#
//#========================================================
void Firefox_StartPage_Set()
{
	ULONG	cnt = 0;

	char	str[1600];
	char	vpnurl[255];
//	char	VdiUrl[255];
	char 	input[1700];

	memset(str, 0, sizeof(str));
	memset(vpnurl, 0, sizeof(vpnurl));
//	memset(VdiUrl, 0, sizeof(VdiUrl));
	memset(input, 0, sizeof(input));

///// -Del [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka
//	get_hci_string(VAL_VDI_URL,&VdiUrl[0]);
///// -Add [[VENDOR_AUTH_MODE](TMZC) 2016/07/10 Tanaka

	if(VdiInfo != VDI_VMWARE){
		//### Citrix URL setting ###
		strcpy(&str[0], &VdiUrl[0]);
	}
	cnt = 1;
	if(get_env_variable(WEB_AUTH_MODE) == WEB_AUTH_ON){
		while(1){
			//### Vpn URL get ###
			set_hci_value(VAL_BIG_IP_INFO_NO, &cnt);
			get_hci_string(VAL_BIG_IP_URL, &vpnurl[0]);
			if(strcmp(&vpnurl[0], "") == 0){
				//### non ###
				break;
			}
			if((VdiInfo == VDI_VMWARE) && (cnt == 1)){
				strcpy(&str[0], &vpnurl[0]);
			}else{

				//### Data set ###
				//###str="${str}|${VpnUrl}"###
				strcat(&str[0], "|");
				strcat(&str[0], &vpnurl[0]);
			}
			if(cnt >= 7){
				break;
			}
			cnt++;
		}	
	}
	if((VdiInfo != VDI_VMWARE) || (cnt != 1)){
		//###printf "user_pref(\"browser.startup.homepage\",\"%s\");\n" "${str}" >> /root/.mozilla/firefox/v18xwu6c.default/user.js###
		sprintf(&input[0], "printf \"user_pref(\\\"browser.startup.homepage\\\",\\\"%%s\\\");\\n\" \"%s\" >> /root/.mozilla/firefox/v18xwu6c.default/user.js", str);
		system(input);
	}
}


const char *dlgTitle = "TMZC Remote Desktop Connection";
const char *dlgMsg   = "Enter the following information and select <Submit> to connect.";

void rdesktop_info_input_dialog(void)
{
  char dialogCommand[1024];
  FILE *pf;
  int pret;

  if (get_env_variable(DEBUG_MODE) == DEBUG_ON ){
    key_setting(KEY_ALL_ENABLE);
  } else if(get_env_variable(DEBUG_MODE) == DEBUG_OFF ){
    key_setting(KEY_CTL_DISABLE);
  }

  memset(RdeskTop_URL, '\0', sizeof(RdeskTop_URL));
  memset(RdeskTop_NAME, '\0', sizeof(RdeskTop_NAME));
  memset(RdeskTop_PASSWORD, '\0', sizeof(RdeskTop_PASSWORD));
  memset(RdeskTop_DOMAIN, '\0', sizeof(RdeskTop_DOMAIN));
  memset(RdeskTop_OPTION, '\0', sizeof(RdeskTop_OPTION));

  while(1){
    sprintf(dialogCommand, 
      "dialog "
      "--title '[%s]' "
      "--ok-label 'Submit' "
      "--nocancel "
      "--insecure "
      "--mixedform '%s' "
      "18 82 0 "
      "'Server[:port] :' 1 1 -- '%s' 1 15 63 0 0 "
      "'Username      :' 3 1 '' 3 15 63 0 0 "
      "'Password      :' 5 1 '' 5 15 63 0 1 "
      "'Domain        :' 7 1 '' 7 15 63 0 0 "
      "'Option        :' 9 1 '' 9 15 200 0 0 "
      "2>&1 >/dev/tty",
      dlgTitle, dlgMsg, VdiUrl);

    /* display dialog and key wait */
    pf = popen(dialogCommand, "r");
    if(pf){
      fgets(RdeskTop_URL     , sizeof(RdeskTop_URL)-1     , pf);
      fgets(RdeskTop_NAME    , sizeof(RdeskTop_NAME)-1    , pf);
      fgets(RdeskTop_PASSWORD, sizeof(RdeskTop_PASSWORD)-1, pf);
      fgets(RdeskTop_DOMAIN  , sizeof(RdeskTop_DOMAIN)-1  , pf);
      fgets(RdeskTop_OPTION  , sizeof(RdeskTop_OPTION)-1  , pf);

      pret = pclose(pf);
      if(WIFEXITED(pret)){
        /* Success */
        if(WEXITSTATUS(pret) == DIALOG_ESC){
          /* [Esc]Pressed. */
          continue;
        } else if(WEXITSTATUS(pret) == DIALOG_OK){
          /* <Submit> as OK */
          char *p;
          if((p = strchr(RdeskTop_URL, '\n')) != NULL){
            *p = '\0';
          }
          if((p = strchr(RdeskTop_NAME, '\n')) != NULL){
            *p = '\0';
          }
          if((p = strchr(RdeskTop_PASSWORD, '\n')) != NULL){
            *p = '\0';
          }
          if((p = strchr(RdeskTop_DOMAIN, '\n')) != NULL){
            *p = '\0';
          }
          if((p = strchr(RdeskTop_OPTION, '\n')) != NULL){
            *p = '\0';
          }

          if(strlen(RdeskTop_URL) == 0){
            if (strlen(VdiUrl) == 0) {
              continue;
            } else {
              strcpy(RdeskTop_URL, VdiUrl);
            }
          }
          break;  /* break for while */
        } else {
          continue;
        }
      } else {
        perror("pclose");
				system("poweroff");
      }
    } else {
      perror("popen");
			system("poweroff");
    }
  }
  if(get_env_variable(DEBUG_MODE) == DEBUG_OFF ){
    key_setting(KEY_ALL_DISABLE);
  }
}

void retry_rdesktop_info_input_dialog(void)
{
  char dialogCommand[1024];
  FILE *pf;
  int pret;

  if (get_env_variable(DEBUG_MODE) == DEBUG_ON ){
    key_setting(KEY_ALL_ENABLE);
  } else if(get_env_variable(DEBUG_MODE) == DEBUG_OFF ){
    key_setting(KEY_CTL_DISABLE);
  }

//  memset(RdeskTop_URL, '\0', sizeof(RdeskTop_URL));
//  memset(RdeskTop_NAME, '\0', sizeof(RdeskTop_NAME));
  memset(RdeskTop_PASSWORD, '\0', sizeof(RdeskTop_PASSWORD));
//  memset(RdeskTop_DOMAIN, '\0', sizeof(RdeskTop_DOMAIN));
//  memset(RdeskTop_OPTION, '\0', sizeof(RdeskTop_OPTION));

  while(1){
    sprintf(dialogCommand, 
      "dialog "
      "--title '[%s]' "
      "--ok-label 'Submit' "
      "--nocancel "
      "--insecure "
      "--mixedform '%s' "
      "18 82 0 "
      "'Server[:port] :' 1 1 -- '%s' 1 15 63 0 0 "
      "'Username      :' 3 1 -- '%s' 3 15 63 0 0 "
      "'Password      :' 5 1 -- '%s' 5 15 63 0 1 "
      "'Domain        :' 7 1 -- '%s' 7 15 63 0 0 "
      "'Option        :' 9 1 -- '%s' 9 15 200 0 0 "
      "2>&1 >/dev/tty",
      dlgTitle, dlgMsg, RdeskTop_URL, RdeskTop_NAME, RdeskTop_PASSWORD, RdeskTop_DOMAIN, RdeskTop_OPTION);

    /* display dialog and key wait */
    pf = popen(dialogCommand, "r");
    if(pf){

      fgets(RdeskTop_URL     , sizeof(RdeskTop_URL)-1     , pf);
      fgets(RdeskTop_NAME    , sizeof(RdeskTop_NAME)-1    , pf);
      fgets(RdeskTop_PASSWORD, sizeof(RdeskTop_PASSWORD)-1, pf);
      fgets(RdeskTop_DOMAIN  , sizeof(RdeskTop_DOMAIN)-1  , pf);
      fgets(RdeskTop_OPTION  , sizeof(RdeskTop_OPTION)-1  , pf);

      pret = pclose(pf);
      if(WIFEXITED(pret)){
        /* Success */
        if(WEXITSTATUS(pret) == DIALOG_ESC){
          /* [Esc]Pressed. */
          continue;
        } else if(WEXITSTATUS(pret) == DIALOG_OK){
          /* <Submit> as OK */
          char *p;
          if((p = strchr(RdeskTop_URL, '\n')) != NULL){
            *p = '\0';
          }
          if((p = strchr(RdeskTop_NAME, '\n')) != NULL){
            *p = '\0';
          }
          if((p = strchr(RdeskTop_PASSWORD, '\n')) != NULL){
            *p = '\0';
          }
          if((p = strchr(RdeskTop_DOMAIN, '\n')) != NULL){
            *p = '\0';
          }
          if((p = strchr(RdeskTop_OPTION, '\n')) != NULL){
            *p = '\0';
          }

          if(strlen(RdeskTop_URL) == 0){
              continue;
          }
          break;  /* break for while */
        } else {
          continue;
        }
      } else {
        perror("pclose");
				system("poweroff");
      }
    } else {
      perror("popen");
			system("poweroff");
    }
  }
  if(get_env_variable(DEBUG_MODE) == DEBUG_OFF ){
    key_setting(KEY_ALL_DISABLE);
  }
}
// -modify [Option](TZCS (Base))  2017/03/08 
void control_code_conversion(char *p)
{
	// 0x01 ~ 0x1f ---> NULL
	while(*p != '\0') {
		if (*p <= 0x1f) {
			*p = '\0';
			break;
		}
		p++;
	}
}


// -modify [B-Note 5308-8](TZCS 1.3)  2017/08/01 
//void	option_special_data_set(void)
int		option_special_data_set(void)
// -modify-end [B-Note 5308-8](TZCS 1.3)  2017/08/01 
{

	const char *option_data_version = "1.0";
	int i,len;
	int version_check_flg = 1;
	int tcf_file_check_flg;
	char string[1000];
	char command[1000];
	char path_folder[1000];
	char b_cert_name[512];
	unsigned char   *split;
	FILE *fp = NULL;
	struct stat st;
	typedef struct OPTDATAINFO {
		char	folder[1000];
		int 	file_type;	// 0xFF : Initial value (not set)
										 	// 0x00 : Normal file
										 	// 0x01 : Encrypted file
										 	// 0x02 : Encrypted zip file
		char 	file_type_work[10];
		char	client_folder[1000];
	} OPTDATAINFO;
	OPTDATAINFO opt_data_info;
	DIR *dir = NULL;
	struct dirent *dent;
	char *p;

	//**************************************
	//*** special option data check      ***
	//**************************************
	if (stat("/Option/OptionData.tcf", &st) == 0) {

		//**************************************
		//*** Decrypt OptionData.tcf file    ***
		//**************************************
		if (stat("/Option/OptionData.ini", &st) == 0) {
			system("rm /Option/OptionData.ini");
		}
		if(WEXITSTATUS(system("cd /Option ; tenco d3 OptionData.tcf > /dev/null")) != 0){
//		if(WEXITSTATUS(system("tenco d3 /Option/OptionData.tcf > /dev/null")) != 0){
			// Decrypt failure
#ifdef TZCS_DEBUG
			printf("[option_special_data_set] Decrypt failure\n");
			sleep(5);
#endif
//			error_display(DISPLAY_ERROR_045 ,0);
			warning_display(DISPLAY_WARNING_024);
			goto special_option_data_error;
		}
//                system("mv OptionData.ini /Option/. > /dev/null");

#ifdef TZCS_DEBUG
		system("ls /Option");
#endif
		//**************************************
		//*** OptionData.ini file check     ***
		//**************************************
		if (stat("/Option/OptionData.ini", &st) != 0) {
			// file non
#ifdef TZCS_DEBUG
			printf("[option_special_data_set] OptionData.ini file non\n");
			sleep(5);
#endif
//			error_display(DISPLAY_ERROR_045 ,0);
			warning_display(DISPLAY_WARNING_024);
			goto special_option_data_error;
		}

		//**************************************
		//*** OptionData.ini file open       ***
		//**************************************
		fp = fopen("/Option/OptionData.ini", "r");
		if (fp == NULL) {
			// file open failed
#ifdef TZCS_DEBUG
			printf("[option_special_data_set] OptionData.ini file open failed\n");
			sleep(5);
#endif
//			error_display(DISPLAY_ERROR_045 ,0);
			warning_display(DISPLAY_WARNING_024);
			goto special_option_data_error;
		}

		//******************************************
		//*** OptionData.ini file contents check ***
		//******************************************
		while(fgets(string,sizeof(string), fp) != NULL) {
			control_code_conversion(string);

			//******************************************
			//*** Option data version tag check      ***
			//******************************************
//			if ((split = (unsigned char *)strstr((const char *)string, "[version]")) != NULL) {
			if(strcmp(string, "[version]") == 0){
				fgets(string,sizeof(string), fp);
				control_code_conversion(string);
				if(strcmp(string, option_data_version) == 0){
					version_check_flg = 0;
				}else{
					// option version error
#ifdef TZCS_DEBUG
					printf("[option_special_data_set] option version error\n");
					sleep(5);
#endif
//					error_display(DISPLAY_ERROR_046 ,0);
					warning_display(DISPLAY_WARNING_025);
					goto special_option_data_error;
				}
			}

			//******************************************
			//*** Option data tag check              ***
			//******************************************
//			if ((split = (unsigned char *)strstr((const char *)string, "[Option data]")) != NULL) {
			if(strcmp(string, "[Option data]") == 0){
				// Option data information area initialization
				memset(opt_data_info.folder, '\0', sizeof(opt_data_info.folder));
				memset(opt_data_info.client_folder, '\0', sizeof(opt_data_info.client_folder));
				memset(opt_data_info.client_folder, '\0', sizeof(opt_data_info.client_folder));
				opt_data_info.file_type = 0xFF;

				//******************************************
				//*** Option data information get        ***
				//******************************************
				for(i=0;i<3;i++){
					fgets(string,sizeof(string), fp);
					control_code_conversion(string);
					// folder information get
					if ((split = (unsigned char *)strstr((const char *)string, "Folder=")) != NULL) {
						strncpy(opt_data_info.folder, (const char *)(split + strlen("Folder=")), strlen((const char *)split) - strlen("Folder="));
					}
					// Client folder information get
					if ((split = (unsigned char *)strstr((const char *)string, "Client folder=")) != NULL) {
						strncpy(opt_data_info.client_folder, (const char *)(split + strlen("Client folder=")), strlen((const char *)split) - strlen("Client folder="));
					}
					// File type information get
					if ((split = (unsigned char *)strstr((const char *)string, "File type=")) != NULL) {
						strncpy(opt_data_info.file_type_work, (const char *)(split + strlen("File type=")), strlen((const char *)split) - strlen("File type="));
						opt_data_info.file_type = atoi((const char *)opt_data_info.file_type_work);
					}
				}
#ifdef TZCS_DEBUG
				printf("[option_special_data_set] Option data information\n");
				printf("[option_special_data_set] Folder        = %s\n",opt_data_info.folder);
				printf("[option_special_data_set] Client folder = %s\n",opt_data_info.client_folder);
				printf("[option_special_data_set] File type     = %d\n",opt_data_info.file_type);
				sleep(5);
#endif

				//*********************************************
				//*** Option data information result check  ***
				//*********************************************
				sprintf(path_folder, "/Option/%s",opt_data_info.folder);
				if((strlen(opt_data_info.folder) == 0) || (stat(path_folder, &st) != 0) ||
					 (strlen(opt_data_info.client_folder) == 0) || (stat(opt_data_info.client_folder, &st) != 0) ||
					 (opt_data_info.file_type > 2)){
							// Option data information contents error
#ifdef TZCS_DEBUG
							printf("[option_special_data_set] option data information contents error\n");
							sleep(5);
#endif
//							error_display(DISPLAY_ERROR_046 ,0);
							warning_display(DISPLAY_WARNING_025);
							goto special_option_data_error;
				}
				
				//******************************************
				//*** Special option data expansion      ***
				//******************************************
				if((opt_data_info.file_type == 1) || (opt_data_info.file_type == 2)){
					// Encrypted data
					tcf_file_check_flg = 1;
					//******************************************
					//*** Special option data tcf file check ***
					//******************************************
					dir = opendir(path_folder);
					if (dir == NULL) {
						// directory open error
#ifdef TZCS_DEBUG
						printf("[option_special_data_set] special option data directory open error\n");
						sleep(5);
#endif
//						error_display(DISPLAY_ERROR_047 ,0);
						warning_display(DISPLAY_WARNING_026);
						goto special_option_data_error;
					}
					
					while ((dent = readdir(dir)) != NULL) {
						if (dent->d_type != DT_DIR) {
							control_code_conversion(dent->d_name);
							p = &dent->d_name[0];
							len = strlen((const char *)dent->d_name);
							if (strcmp((const char *)p + (len - 4),".tcf") == 0) {
								strcpy(string,dent->d_name);
								tcf_file_check_flg = 0;
								break;
							}
						}
					}
					closedir(dir);
					dir = NULL;
					if(tcf_file_check_flg != 0){
						// tcf file non
#ifdef TZCS_DEBUG
						printf("[option_special_data_set] special option data tcf file non\n");
						sleep(5);
#endif
//						error_display(DISPLAY_ERROR_047 ,0);
						warning_display(DISPLAY_WARNING_026);
						goto special_option_data_error;
					}
#ifdef TZCS_DEBUG
					printf("[option_special_data_set] tcf file name = %s\n",string);
					sleep(5);
#endif

					//******************************************
					//*** Special option data decryption     ***
					//******************************************
//					sprintf(command, "cd %s",opt_data_info.folder);
//					if(WEXITSTATUS(system(command)) != 0){
//#ifdef TZCS_DEBUG
//						printf("[option_special_data_set] option data directory change error\n");
//						sleep(5);
//#endif
//						error_display(DISPLAY_ERROR_047 ,0);	
//					}
					sprintf(command, "cd %s ; tenco d3 %s > /dev/null 2>&1",path_folder,string);
					if(WEXITSTATUS(system(command)) != 0){
						// Decoding failure
#ifdef TZCS_DEBUG
						printf("[option_special_data_set] special option data decoding failure\n");
						sleep(5);
#endif
//						error_display(DISPLAY_ERROR_047 ,0);
						warning_display(DISPLAY_WARNING_026);
						goto special_option_data_error;
					}
					sprintf(command, "rm %s/%s > /dev/null",path_folder,string);
					system(command);
					
					//******************************************************
					//*** Special option data zip file decompression     ***
					//******************************************************
					if(opt_data_info.file_type == 2){
						len = strlen((const char *)string);
						string[len - 4] = '\0';
						sprintf(command, "cd %s ; unzip %s.zip > /dev/null 2>&1",path_folder,string);
						if(WEXITSTATUS(system(command)) != 0){
							// zip file decompression failure
#ifdef TZCS_DEBUG
							printf("[option_special_data_set] special option data zip file decompression failure\n");
							sleep(5);
#endif
//							error_display(DISPLAY_ERROR_047 ,0);
							warning_display(DISPLAY_WARNING_026);
							goto special_option_data_error;
						}
						sprintf(command, "rm %s/%s.zip > /dev/null",path_folder,string);
						system(command);
					}
//					system("cd ..");
				}

				//******************************************
				//*** Special option data copy		       ***
				//******************************************
#ifdef TZCS_DEBUG
				printf("[option_special_data_set] ***** copy file *****\n");
#endif
				dir = opendir(path_folder);
				if (dir == NULL) {
					// directory open error
#ifdef TZCS_DEBUG
					printf("[option_special_data_set] special option data directory open error\n");
					sleep(5);
#endif
//					error_display(DISPLAY_ERROR_047 ,0);
					warning_display(DISPLAY_WARNING_026);
					goto special_option_data_error;
				}
					
				while ((dent = readdir(dir)) != NULL) {
					if (dent->d_type != DT_DIR) {
						control_code_conversion(dent->d_name);
						add_backslash(b_cert_name ,sizeof(b_cert_name) ,dent->d_name);
#ifdef TZCS_DEBUG
						printf("[option_special_data_set] %s\n",b_cert_name);
#endif
//    system("chmod -R 644 /chroot/Option");
						sprintf(command, "chmod a-x %s/\"%s\" > /dev/null 2>&1",path_folder,b_cert_name);
						system(command);
						sprintf(command, "cp %s/\"%s\" %s > /dev/null 2>&1",path_folder,b_cert_name,opt_data_info.client_folder);
						if(WEXITSTATUS(system(command)) != 0){
							// copy failure
#ifdef TZCS_DEBUG
							printf("[option_special_data_set] special option data copy failure\n");
							sleep(5);
#endif
//							error_display(DISPLAY_ERROR_047 ,0);
							warning_display(DISPLAY_WARNING_026);
							goto special_option_data_error;
						}
					}
				}
				closedir(dir);
				dir = NULL;
#ifdef TZCS_DEBUG
				sleep(5);
#endif
			}
		}
		//**************************************
		//*** option version result check    ***
		//**************************************
		if(version_check_flg != 0) {
			// option version non
#ifdef TZCS_DEBUG
			printf("[option_special_data_set] option version non\n");
			sleep(5);
#endif
//			error_display(DISPLAY_ERROR_046 ,0);
			warning_display(DISPLAY_WARNING_025);
			goto special_option_data_error;
		}


		//**************************************
		//*** OptionData.ini file close      ***
		//**************************************
		fclose(fp);
	}
// -modify [B-Note 5308-8](TZCS 1.3)  2017/08/01 
//	return;
	return(0);
// -modify-end [B-Note 5308-8](TZCS 1.3)  2017/08/01 
	
special_option_data_error:
	if (fp != NULL) {
		fclose(fp);
	}
	if (dir != NULL) {
		closedir(dir);
	}
// -modify [B-Note 5308-8](TZCS 1.3)  2017/08/01 
//	return;
	return(1);
// -modify-end [B-Note 5308-8](TZCS 1.3)  2017/08/01 
	
}

int	option_cert_data_set(const char *folder,const char *client_folder)
{
	DIR *dir;
	struct dirent *dent;
	char command[1000];
	char string[1000];
	int ret,i;
	struct stat st;
// -modify [Option-Browser](TZCS 1.3)  2017/08/10 
	char b_cert_name[512];
	char b_cert_nickname[512];
	char *p;
  FILE *fp;
// -modify [Option-Browser](TZCS 1.3)  2017/08/10 
	int  salk_cert_del_flg = 0;    // 0:No deletion 1:deletion 2:No deletion(SALK certificate updated)

#ifdef TZCS_DEBUG
	printf("[option_cert_data_set] start\n");
	printf("[option_cert_data_set] folder        = %s\n",folder);
	printf("[option_cert_data_set] client folder = %s\n",client_folder);
#endif
	//**************************************
	//***   option data folder open      ***
	//**************************************
	dir = opendir(folder);
	if (dir == NULL) {
		// directory open error
#ifdef TZCS_DEBUG
		printf("[option_cert_data_set] directory open error\n");
		sleep(5);
#endif
//		error_display(DISPLAY_ERROR_043 ,0);
		warning_display(DISPLAY_WARNING_022);
		goto cert_option_data_error;
	}
	
	//********************************************
	//*** option data folder file check & copy ***
	//********************************************
	while ((dent = readdir(dir)) != NULL) {
		if (dent->d_type != DT_DIR) {
			control_code_conversion(dent->d_name);
			add_backslash(b_cert_name ,sizeof(b_cert_name) ,dent->d_name);
// -modify [Option-Browser](TZCS 1.3)  2017/08/10 
//			if (strcmp(folder,"/Option/BrowserCert") != 0) {
// -modify-end [Option-Browser](TZCS 1.3)  2017/08/10 
			//*****************************************
			//*** Format check of certificate file  ***
			//*****************************************
			//*** pem file check ***
			sprintf(command, "openssl x509 -in %s/\"%s\" -text -noout > /dev/null 2>&1",folder,b_cert_name);
			if(WEXITSTATUS(system(command)) != 0){
				// not pem file
				//*** der file check ***
				sprintf(command, "openssl x509 -in %s/\"%s\" -inform der -text -noout > /dev/null 2>&1",folder,b_cert_name);
				if(WEXITSTATUS(system(command)) == 0){
					// der file
					// Convert from der to pem
					sprintf(command, "openssl x509 -in %s/\"%s\" -inform der -out %s/\"%s\" -outform pem > /dev/null 2>&1",folder,b_cert_name,folder,b_cert_name);
				  if(WEXITSTATUS(system(command)) != 0){
//						error_display(DISPLAY_ERROR_044 ,0);
						warning_display(DISPLAY_WARNING_023);
						goto cert_option_data_error;
					}
				}else{
					// not der file
					//*** net file check ***
					sprintf(command, "openssl x509 -in %s/\"%s\" -inform net -text -noout > /dev/null 2>&1",folder,b_cert_name);
					if(WEXITSTATUS(system(command)) == 0){
						// net file
						// Convert from net to pem
						sprintf(command, "openssl x509 -in %s/\"%s\" -inform net -out %s/\"%s\" -outform pem > /dev/null 2>&1",folder,b_cert_name,folder,b_cert_name);
					  if(WEXITSTATUS(system(command)) != 0){
//							error_display(DISPLAY_ERROR_044 ,0);
							warning_display(DISPLAY_WARNING_023);
							goto cert_option_data_error;
						}
					}else{
//							error_display(DISPLAY_ERROR_044 ,0);
						warning_display(DISPLAY_WARNING_023);
						goto cert_option_data_error;
					}
				}
			}

// -modify [Option-Browser](TZCS 1.3)  2017/08/10 
			if (strcmp(folder,"/Option/BrowserCert") == 0) {
				//********************************************
				//*** Add to firefox certificate database ***
				//********************************************
//				/**** cert name change ***/
//				add_backslash(b_cert_name ,sizeof(b_cert_name) ,dent->d_name);
				/**** Get cert nickname ***/
				strcpy(b_cert_nickname, b_cert_name);
				if ((p = strrchr(b_cert_nickname, '.'))!=NULL){
					*p = '\0';
				}
				/*** Add to firefox certificate database ***/
// cert9.db
//				sprintf(command,"certutil -A -n \"%s\" -t \"C,C,C\" -a -d \"sql:%s\" -i \"%s/%s\""
//									,b_cert_nickname,client_folder,folder,b_cert_name);
// cert8.db
				sprintf(command,"certutil -A -n \"%s\" -t \"C,C,C\" -a -d \"%s\" -i \"%s/%s\""
									,b_cert_nickname,client_folder,folder,b_cert_name);
//				ret = get_info(command,string );
//				if((ret != TZCS_TRUE) || (strlen(string) != 0)){
				fp = popen(command, "r");
				if (fp == NULL) {
					warning_display(DISPLAY_WARNING_022);
					goto cert_option_data_error;
				}else{
					p = fgets(string, 1000, fp);
					pclose(fp);
					if (p != NULL) {
						warning_display(DISPLAY_WARNING_022);
						goto cert_option_data_error;
					}
				}
			} else {
// -modify-end [Option-Browser](TZCS 1.3)  2017/08/10 
				//*******************************************************************
				//*** Check whether the copy file is already in the client folder ***
				//*******************************************************************
				sprintf(command, "%s/%s",client_folder,dent->d_name);
				if (stat(command, &st) == 0) {
					//*** Delete hash value file ***
					sprintf(command, "openssl x509 -in %s/\"%s\" -hash -noout",client_folder,b_cert_name);
					ret = get_info(command,string );
					if(ret == TZCS_TRUE){
						sprintf(command, "rm %s/%s.0 > /dev/null 2>&1",client_folder,string);
						system(command);
#ifdef TZCS_DEBUG
						printf("[option_cert_data_set] Delete Hash value file\n");
						sleep(5);
#endif
					}else{
#ifdef TZCS_DEBUG
						printf("[option_cert_data_set] Delete Hash value get failed\n");
						sleep(5);
#endif
//						error_display(DISPLAY_ERROR_044 ,0);
						warning_display(DISPLAY_WARNING_023);
						goto cert_option_data_error;
					}
				}
// -modify [Option-Browser](TZCS 1.3)  2017/08/10 
//			}
// -modify-end [Option-Browser](TZCS 1.3)  2017/08/10 

				//**********************************
				//***  Copy certificate file     ***
				//**********************************
				sprintf(command, "chmod a-x %s/\"%s\" > /dev/null",folder,b_cert_name);
				system(command);
				sprintf(command, "cp %s/\"%s\" %s/. > /dev/null 2>&1",folder,b_cert_name,client_folder);
				if(WEXITSTATUS(system(command)) != 0){
					// copy failed
#ifdef TZCS_DEBUG
					printf("[option_cert_data_set] copy failed\n");
					sleep(5);
#endif
//					error_display(DISPLAY_ERROR_043 ,0);
					warning_display(DISPLAY_WARNING_022);
					goto cert_option_data_error;
				}

			//**********************************
			//***  Hash value file creation  ***
			//**********************************
// -modify [Option-Browser](TZCS 1.3)  2017/08/10 
//			if(strcmp(folder,"/Option/BrowserCert") != 0){
// -modify-end [Option-Browser](TZCS 1.3)  2017/08/10 
				sprintf(command, "openssl x509 -in %s/\"%s\" -hash -noout",folder,b_cert_name);
				ret = get_info(command,string );
				if(ret == TZCS_FALSE){
#ifdef TZCS_DEBUG
					printf("[option_cert_data_set] Hash value get failed\n");
					sleep(5);
#endif
//					error_display(DISPLAY_ERROR_044 ,0);
					warning_display(DISPLAY_WARNING_023);
					goto cert_option_data_error;
				}
				control_code_conversion(string);
				// Check if there is a hash value file to be created in the client folder
				for(i=0;i<3;i++){
					sprintf(command, "%s/%s.%d",client_folder,string,i);
					if (stat(command, &st) != 0) {
						// Non hash value file to be created
						break;
					}
				}
				if(i >= 3){
#ifdef TZCS_DEBUG
					printf("[option_cert_data_set] Hash value file already has *. 0 - 2\n");
					sleep(5);
#endif
//					error_display(DISPLAY_ERROR_044 ,0);
					warning_display(DISPLAY_WARNING_023);
					goto cert_option_data_error;
				}
				sprintf(command, "ln -s %s/\"%s\" %s/%s.%d",client_folder ,b_cert_name ,client_folder ,string ,i);
				if(WEXITSTATUS(system(command)) != 0){
#ifdef TZCS_DEBUG
					printf("[option_cert_data_set] Hash value file create failed\n");
					sleep(5);
#endif
//					error_display(DISPLAY_ERROR_044 ,0);
					warning_display(DISPLAY_WARNING_023);
					goto cert_option_data_error;
				}
				//*************** SALK Certificate deletion check ****************
				if (strcmp(folder,"/Option/RootCert") == 0) {
					if (salk_cert_del_flg != 2) {
						if (strcmp(salk_cert_file,"dent->d_name") == 0) {
							salk_cert_del_flg = 2;
						} else {
							salk_cert_del_flg = 1;
						}
					}
				}
			}	
		}
	}
	//*************** SALK Certificate deletion ****************
	if (salk_cert_del_flg == 1) {
		sprintf(command, "rm /etc/ssl/certs/%s  > /dev/null 2>&1",salk_cert_file);
		system(command);
		sprintf(command, "rm /etc/ssl/certs/%s  > /dev/null 2>&1",salk_cert_hash_file);
		system(command);
	}
	closedir(dir);
	return(0);


cert_option_data_error:
	if (dir != NULL) {
		closedir(dir);
	}
	return(1);

}

void	option_data_set(void)
{
	struct stat st;
	int ret;
// -modify [B-Note 5308-8](TZCS 1.3)  2017/08/01 
	char	DownloadOptionName[64*2];
	char	DownloadOptionNameZip[64*2];
	char	command[1024];
// -modify-end [B-Note 5308-8](TZCS 1.3)  2017/08/01 

	
	//**************************************
	//*** Option data folder check       ***
	//**************************************
#ifdef TZCS_DEBUG
	printf("option_data_set start \n");
#endif
//	system("cd /");
	if (stat("/Option", &st) == 0) {
#ifdef TZCS_DEBUG
		printf("option_data_set Option folder OK \n");
#endif

		//**************************************
	 	//***   Root cert folder            ***
		//**************************************
		if (stat("/Option/RootCert", &st) == 0) {
#ifdef TZCS_DEBUG
			printf("RootCert call \n");
#endif
			ret = option_cert_data_set("/Option/RootCert","/etc/ssl/certs");
		}

		//**************************************
	 	//***   Citrix cert folder          ***
		//**************************************
		if (ret == 0) {
			if (stat("/Option/CitrixCert", &st) == 0) {
				ret = option_cert_data_set("/Option/CitrixCert","/opt/Citrix/ICAClient/keystore/cacerts");
			}
		}
		//**************************************
	 	//***   Browser cert folder          ***
		//**************************************
		if (ret == 0) {
			if (stat("/Option/BrowserCert", &st) == 0) {
#ifdef TZCS_DEBUG
				printf("/Option/BrowserCert call \n");
#endif
// -modify [Option-Browser](TZCS 1.3)  2017/08/10 
//				ret = option_cert_data_set("/Option/BrowserCert","/root/.mozilla/firefox/v18xwu6c.default");
				ret = option_cert_data_set("/Option/BrowserCert","/root/.mozilla/firefox/v18xwu6c.default/");
// -modify-end [Option-Browser](TZCS 1.3)  2017/08/10 
			}
		}
		//**************************************
	 	//***   Special option data          ***
		//**************************************
#ifdef TZCS_DEBUG
		printf("option_special_data call \n");
#endif
		if (ret == 0) {
// -modify [B-Note 5308-8](TZCS 1.3)  2017/08/01 
//			option_special_data_set();
			ret = option_special_data_set();
// -modify-end [B-Note 5308-8](TZCS 1.3)  2017/08/01 
		}
		system("rm -r /Option");
//		system("cd /root");
// -modify [B-Note 5308-8](TZCS 1.3)  2017/08/01 
		//***** Delete SD cache save data when failing to capture option data. *****/
		if (ret != 0) {
			memset(DownloadOptionName, 0, sizeof(DownloadOptionName));
			get_hci_string(VAL_OPTION_DATA_NAME,DownloadOptionName );
			sprintf(DownloadOptionNameZip, "/tmp/bigcore/%s.zip", DownloadOptionName);
			if (stat(DownloadOptionNameZip,&st) == 0) {
				sprintf(command, "rm %s > /dev/null 2>&1", DownloadOptionNameZip);
				system(command);
			}
		}
// -modify-end [B-Note 5308-8](TZCS 1.3)  2017/08/01 
	}
#ifdef TZCS_DEBUG
	printf("************* option result ********************\n");
	printf("******* root  ***************\n");
	system("ls /root/");
	printf("******* etc  ***************\n");
	system("ls /etc/");
	printf("******* opt  ***************\n");
	system("ls /opt/");
	sleep(10);
#endif

}
// -modify [Option](TZCS (Base))  2017/03/08 


