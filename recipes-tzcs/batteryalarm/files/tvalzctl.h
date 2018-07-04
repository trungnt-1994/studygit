/*
Copyright (C) 2016 TOSHIBA Corporation

Module Name:
    tvalzctl.h

Abstract:
   This module is a header file for the call sides which uses a Toshiba peculiar driver (tvalz.ko).

Author:
   TIH/SWT
   ZYQ

Environment:
   Operates only in the kernel mode of ACPI OS.

Notes:
   Note.

Revision History:
   18-sep-2012 Version 1.0.0.1 ZYQ Creation.
*/

/***********************************************
 *
 * Change history
 *
 * ZCS (Phase1)	 2014/10/01	Tanaka & Shimizu
 *  - New
 * 1.[S-001](ZCS (Phase1)) 2014/10/09 Tanaka
 *  - Porting the header information of the GetHci and SetHci
 * 2.[S-002](ZCS (Phase1)) 2014/10/09 Tanaka
 *  - Add heat run support
 * 3.[S-003](ZCS (Phase1)) 2014/10/09 Tanaka
 *  - Add DHCP ON-OFF support
 * 4.[S-004](ZCS (Phase1)) 2014/10/15 Tanaka
 *  - Add Wifi AP change support
 *-----------------------------------------
 * ZCS (Phase2)	 2014/12/08	Tanaka 
 * 1.[S-001](ZCS (Phase2)) 2014/12/08 Tanaka
 *  - Modify for TSCM
 * 2.[S-002](ZCS (Phase2)) 2015/02/13 Tanaka
 *  - Modify for Fuchu power company
 * 3.[S-004](ZCS (Phase2)) 2015/03/02 Tanaka
 *  - Modify for Azusa
 * 4.[S-005](ZCS (Phase2)) 2015/03/15 Tanaka
 *  - Added LAN Link Status
 * 5.[S-xx1](ZCS (Phase2-azusa)) 2015/04/02 Shimizu
 *  - Added VAL_DHCP_WAIT_TIME
 *-----------------------------------------
 * TZCS (Phase2-azusa2)	
 *    - New               2015/05/12	Tanaka 
 * 1.[S-005](ZCS (Phase2-azusa2)) 2015/06/20 Shimizu
 *  - Added Error Log Record
 * 2.[S-xx1](ZCS (Phase2)) 2015/07/09 Matsumoto
 *  - Added VAL_EC_VER,VAL_KEYBLIGHT_MODE,VAL_KEYBLIGHT_TIME
 * 3.[S-xx3](ZCS (Phase2-base)) 2015/08/17 Tanaka
 *  - Added Watchdog timer
 *
 *-----------------------------------------
 * 1.[S-001](ZCS (1.1)) 2016/01/12 Tanaka
 *   - Add for web authentication function
 * 2.[S-002](ZCS (1.1)) 2016/01/12 Tanaka
 *   - Add for BIG IP(VPN) function
 *
 ************************************************/


#ifndef TVALZCTL_H
#define TVALZCTL_H

#define ULONG unsigned int

// Define GHCI buffer struct.
typedef struct _ghci_interface {
        ULONG ghci_eax;
        ULONG ghci_ebx;
        ULONG ghci_ecx;
        ULONG ghci_edx;
        ULONG ghci_esi;
        ULONG ghci_edi;
}ghci_interface, *pghci_interface;

// Read mode of input device status.
enum
{   
        TVALZ_HCI_RAW_MODE,
        TVALZ_INFO_MODE,
};

// Define GHCI buffer struct.
typedef struct _info_interface {
        ULONG info_arg0;
        ULONG info_arg1;
        ULONG info_arg2;
        ULONG info_arg3;
        ULONG info_arg4;
        ULONG info_arg5;
        ULONG info_arg6;
        ULONG info_arg7;
        ULONG info_arg8;
        ULONG info_arg9;
}info_interface, *pinfo_interface;

// Define scan code and key code struct.
typedef struct _scan_key_code{
        unsigned int scancode;
        unsigned int keycode;
}scan_key_code, *pscan_key_code;

#define IOCTL_TVALZ_DISABLE_INPUTDEV         _IOR(0x2A,  0x8, int)
#define IOCTL_TVALZ_ENABLE_INPUTDEV          _IOR(0x2A,  0x9, int)
#define IOCTL_TVALZ_INFO                     _IOR(0x2A,  0x10, info_interface)
#define IOCTL_TVALZ_CANCEL_INFO              _IOR(0x2A,  0x11, int)
#define IOCTL_TVALZ_GHCI                     _IOWR(0x2A, 0x20, ghci_interface)
#define IOCTL_TVALZ_ENAB                     _IOR(0x2A,  0x30, int)
#define IOCTL_TVALZ_RUN_METHOD               _IOWR(0x2A, 0x50, ghci_interface)
#define IOCTL_TVALZ_RETRIEVE_EVENT           _IOWR(0x2A, 0x51, ghci_interface)
#define IOCTL_TVALZ_GET_RETRIEVE_EVENT       _IOWR(0x2A, 0x52, ghci_interface)
#define IOCTL_TVALZ_CTL_EXT                  _IOWR(0x2A, 0x60, ghci_interface)
#define IOCTL_TVALZ_SET_KEYCODE              _IOW(0x2A,  0x61, scan_key_code)


// Hci information
// 読み込み値種別(※BIOS側への引数)
#define VAL_WIFI_SSID									0x0000			// WiFi AP SSID
#define VAL_WIFI_UNAME									0x0001			// WiFi AP UserName
#define VAL_WIFI_PASS									0x0002			// WiFi AP Password
#define VAL_SCREEN_LOCK_MODE							0x0003			// Screen Lock Mode(ScreenLock or PowerOff)
#define VAL_TSCM_IP										0x0004			// TSCM server IP address
#define VAL_FTP_IP										0x0005			// FTP server IP address
#define VAL_FTP_UNAME									0x0006			// FTP server UserName
#define VAL_FTP_PASS									0x0007			// FTP server Password
#define VAL_FTP_DATANAME								0x0008			// FTP Download Data Name
#define VAL_BT_ONOFF									0x0009			// Bluetooth device status
#define VAL_VPN_ONOFF									0x000A			// VPN status
//#define VAL_FTP_DISP_ONOFF							0x000B			// FTP Download ON-OFF
#define VAL_TEST_MODE									0x000B			// TEST Mode
#define VAL_TSCM_PAYLOAD								0x000C			// TSCM Request Payload Data
#define VAL_TEST_HEAT_RUN_CNT							0x000D			// Heat Run Counter
#define VAL_NET_DHCP_ONOFF								0x000E			// DHCP on-off
#define VAL_NET_IP										0x000F			// Static IP IP
#define VAL_NET_SUBNET									0x0010			// Static IP Subnet
#define VAL_NET_GATEWAY									0x0011			// Static IP Gateway
#define VAL_NET_DNS										0x0012			// Static IP DNS
#define VAL_WIFI_AP										0x0013			// Wifi-AP 
#define VAL_TSCM_URL									0x0014			// TSCM server URL
#define VAL_TSCM_COM_KEY								0x0015			// TSCM server ComKey
#define VAL_TSCM_CKID									0x0016			// TSCM server Ckid
#define VAL_TSCM_TENANT_ID								0x0017			// TSCM server TenantID
#define VAL_TSCM_SERIAL									0x0018			// TSCM PC serial
#define VAL_VPN_SERVER_IP								0x0019			// VPN Server IP
#define VAL_VPN_GROUP_NAME								0x001A			// VPN Group Name
#define VAL_VPN_SECRET_KEY								0x001B			// VPN Secret Key
#define VAL_VPN_DEFAULT_USER_NAME						0x001C			// VPN Default User Name
#define VAL_VPN_DEFAULT_USER_PASSWORD					0x001D			// VPN Default User Password
#define VAL_VPN_ONE_TIME_PASSWORD						0x001E			// VPN One-Time Password
#define VAL_FTP_UNZIP_PASSWORD							0x001F			// FTP Unzip Password
#define VAL_VPN_DOMAIN									0x0020			// VPN Domain
#define VAL_SATA_DEV_EXISTENCE							0x0021			// SATA Device Existence
#define VAL_BOOT_KEY_INFO								0x0022			// Boot Key Information
#define VAL_MANUFACT_MODE								0x0023			// Manufacturing Mode
#define VAL_CUSTOMER_ID									0x0024			// Customer ID
#define VAL_DOWNLOAD_PROTOCOL							0x0025			// Download Protocol
#define VAL_HOST_NAME									0x0026			// Host Name
#define VAL_SECOND_TSCM_URL								0x0027			// Secondary TSCM(TZCS) Server Address
#define VAL_SECOND_DOWNLOAD_SERVER_IP					0x0028			// Secondary Download Server Address
#define VAL_SECOND_TSCM_COM_KEY							0x0029			// secondary TSCM server ComKey
#define VAL_SECOND_TSCM_CKID							0x002A			// secondary TSCM server Ckid
#define VAL_SECOND_TSCM_TENANT_ID						0x002B			// secondary TSCM server TenantID
#define VAL_LAN_LINK_STATUS								0x002C			// secondary TSCM server TenantID
#define VAL_DHCP_WAIT_TIME								0x002D			// DHCP Response Timeout
//#define VAL_BIOS_VERSION								0x002E			// Bios version
#define VAL_EULA_ACCEPTANCE_STATE						0x002F			// EULA Acceptance State
#define VAL_CACHE_ENC_MAGIC_NUM 						0x0030			// Cache Encryption Magic Number
#define VAL_CACHE_ENC_KEY0 								0x0031			// Cache Encryption Key 0
#define VAL_CACHE_ENC_KEY1 								0x0032			// Cache Encryption Key 1
#define VAL_CACHE_ENC_KEY2 								0x0033			// Cache Encryption Key 2
#define VAL_CACHE_ENC_KEY3 								0x0034			// Cache Encryption Key 3
#define VAL_CACHE_DEC_MAGIC_NUM 						0x0035 			// Cache Decryption Magic Number
#define VAL_CACHE_DEC_KEY0 								0x0036 			// Cache Decryption Key 0
#define VAL_CACHE_DEC_KEY1 								0x0037 			// Cache Decryption Key 1
#define VAL_CACHE_DEC_KEY2 								0x0038 			// Cache Decryption Key 2
#define VAL_CACHE_DEC_KEY3 								0x0039 			// Cache Decryption Key 3
#define VAL_NETWORK_MODE 								0x003A			// Network Mode
#define VAL_TZCS_INI_PACK_URL							0x003B			// TZCS Initial Package URL
#define VAL_TZCS_INI_PACK_PASS							0x003C			// TZCS Initial Package Password
#define VAL_SD_CACHE_FUNC								0x003D			// SD Cache Function 0: OFF (デフォルト) 1: ON
#define VAL_TZCS_INI_PACK_NAME							0x003E			// TZCS Initial Package Name
#define VAL_DHCP_WAIT_TIME_WLAN							0x003F			// DHCP Response Timeout (WLAN)
#define VAL_LANGUAGE		 							0x0040			// Language 0: <English>（デフォルト）1: <Japanese>
#define VAL_BIOS_VER									0x0041			// BIOS Version
#define VAL_BIOS_ID			 							0x0042			// BIOS-ID 戻り値(テキストで16進数4桁)
#define VAL_EC_VER			 							0x0043			// EC Version
#define VAL_PROXY_INFO_SPEC_VER							0x0050			// Proxy Information Spec Version
#define VAL_PROXY_SETTING								0x0051			// Proxy Settings 0: OFF 1: ON
#define VAL_PROXY_URL		 							0x0052			// Proxy Server URL
#define VAL_PROXY_PORT	 								0x0053			// Proxy Port
#define VAL_PROXY_AUTH_SETTING							0x0054			// Proxy Authentication 0: OFF 1: ON
#define VAL_PROXY_AUTH_UNAME							0x0055			// Proxy Authentication User Name
#define VAL_PROXY_AUTH_PASS								0x0056			// Proxy Authentication Password
#define VAL_POLICY_VER	 								0x0060			// Policy version
#define VAL_RECONNECT_RETRY_CNT							0x0061			// Retry count for re-connection
#define VAL_RECONNECT_INTERVAL_LAN						0x0062			// Wired LAN retry interval for re-connection
#define VAL_RECONNECT_INTERVAL_WLAN						0x0063			// Wireless LAN retry interval for re-connection
#define VAL_POLLING_INTERVAL							0x0064			// Polling interval of the authentication server
#define VAL_VDI_PROFILE 								0x0065			// VDI profile 0:Vmware 1:Citrix
#define VAL_VDI_URL			 							0x0066			// VDI connection URL
#define VAL_WIFI_AP_WAIT_TIME							0x0070			// Wifi AP Connect Wait Timer
// -add [S-001](ZCS (1.1)) 2016/01/12 Tanaka
#define VAL_WEB_AUTH											0x0073			// web authentication setting
#define VAL_WEB_AUTH_TIME									0x0074			// web authentication timeout period
// -end [S-001](ZCS (1.1))
// -add [S-002](ZCS (1.1)) 2016/01/12 Tanaka
#define VAL_BIG_IP_INFO_NO								0x0075			// BIG IP（VPN） #n information
#define VAL_BIG_IP_NAME										0x0076			// BIG IP（VPN） #n name
#define VAL_BIG_IP_URL										0x0077			// BIG IP（VPN） #n url
// -end [S-002](ZCS (1.1))
///// -Add [S-005](ZCS (Phase2-azusa2)) 2015/06/20 Shimizu
#define VAL_ERROR_LOG_RECORD							0x0082			// Error Log Recording
///// -End [S-005](ZCS (Phase2-azusa2))
///// -Add [[S-xx3](ZCS (Phase2-base)) 2015/08/17 Tanaka
#define VAL_WATCHDOG_T_START							0x0086			// Watchdog Timer Start
#define VAL_WATCHDOG_T_STOP							  0x0087			// Watchdog Timer Stop
///// -End [[S-xx3](ZCS (Phase2-base)) 
#define VAL_WIFI_INFO_SPEC_VER							0x0100			// WiFi AP Information Spec Version
#define VAL_WIFI_AP_SSID_SET							0x0101			// Latest WiFi AP SSID
#define VAL_WIFI_AP_UNAME_SET							0x0102			// Latest WiFi AP UserName
#define VAL_WIFI_AP_PASS_SET							0x0103			// Latest WiFi AP Password
#define VAL_WIFI_AP_CHAN_SET							0x0104			// Latest WiFi AP Channel
#define VAL_WIFI_AP_SECURITY_SET						0x0105			// Latest WiFi AP Security Type 0: WPA/WPA2-Enterprise 1: WPA/WPA2-Personal 2: WEP
#define VAL_WIFI_DATA_LOADING							0x010E			// Latest WiFi AP Data Loading
#define VAL_WIFI_DATA_DEL								0x010F			// WiFi AP Data Deletion
#define VAL_WIFI_PRIORITY_NO							0x0110			// WiFi AP #n Priority
#define VAL_WIFI_AP_SSID_DATA							0x0111			// WiFi AP #n SSID
#define VAL_WIFI_AP_UNAME_DATA 							0x0112			// WiFi AP #n UserName
#define VAL_WIFI_AP_PASS_DATA 							0x0113			// WiFi AP #n Password
#define VAL_WIFI_AP_CHAN_DATA 							0x0114			// WiFi AP #n Channel
#define VAL_WIFI_AP_SECURITY_DATA						0x0115			// WiFi AP #n Security Type
#define VAL_KEYBLIGHT_MODE								0x0116			// Keyboard Backlight Control Mode
#define VAL_KEYBLIGHT_TIME								0x0117			// Keyboard Backlight Lighting Time

#define VAL_HCI_END										0xFFFF			// End code

//#define VAL_MAX				VAL_LAST_WIFI_AP_SEC			// ※最大値

// get_set_flg
#define	GET_SET_DISABLE									0
#define	GET_ENABLE										1
#define	SET_ENABLE										2
#define	GET_SET_ENABLE									3

struct tval_data{
  int  hci_id;   						/* 読み込み値種別 	*/
  char option_name[20];					/* オプション名 		*/
  int  get_set_flg;   					/* 読み書きフラグ 0：読み書き非対応	1:読み込みのみ対応 2:書き込みのみ対応 3：読み書き対応		*/
};

typedef struct tval_data HCI;

	HCI hci_table[] = {
		 {VAL_WIFI_SSID									,"wifi_ssid"						,GET_SET_ENABLE}
		,{VAL_WIFI_UNAME								,"wifi_uname"						,GET_SET_ENABLE}
		,{VAL_WIFI_PASS									,"wifi_pass"						,GET_SET_ENABLE}
		,{VAL_SCREEN_LOCK_MODE							,"screen_lock_mode"					,GET_SET_ENABLE}
		,{VAL_TSCM_IP									,"tscm_ip"							,GET_SET_ENABLE}
		,{VAL_FTP_IP									,"ftp_ip"							,GET_SET_ENABLE}
		,{VAL_FTP_UNAME									,"ftp_uname"						,GET_SET_ENABLE}
		,{VAL_FTP_PASS									,"ftp_pass"							,GET_SET_ENABLE}
		,{VAL_FTP_DATANAME								,"ftp_dataname"						,GET_SET_ENABLE}
		,{VAL_BT_ONOFF									,"bt_onoff"							,GET_SET_ENABLE}
		,{VAL_VPN_ONOFF									,"vpn_onoff"						,GET_SET_ENABLE}
//        	,{VAL_FTP_DISP_ONOFF							,"ftp_disp_onoff"					,GET_SET_ENABLE}
		,{VAL_TEST_MODE									,"test_mode"						,GET_SET_ENABLE}
		,{VAL_TSCM_PAYLOAD								,"tscm_payload"						,GET_SET_ENABLE}
		,{VAL_TEST_HEAT_RUN_CNT							,"test_heat_run_cnt"				,GET_SET_ENABLE}				// get_hci,set_hciでは使用不可
		,{VAL_NET_DHCP_ONOFF							,"net_dhcp_onoff" 					,GET_SET_ENABLE}
		,{VAL_NET_IP									,"net_ip"				  			,GET_SET_ENABLE}
		,{VAL_NET_SUBNET								,"net_subnet"						,GET_SET_ENABLE}
		,{VAL_NET_GATEWAY								,"net_gateway"						,GET_SET_ENABLE}
		,{VAL_NET_DNS									,"net_dns"							,GET_SET_ENABLE}
		,{VAL_WIFI_AP									,"wifi_ap"							,GET_SET_ENABLE}
		,{VAL_TSCM_URL									,"tscm_url"							,GET_SET_ENABLE}
		,{VAL_TSCM_COM_KEY								,"tscm_com_key"						,GET_SET_ENABLE}				// get_hciのみ使用可
		,{VAL_TSCM_CKID									,"tscm_ckid"						,GET_SET_ENABLE}				// get_hciのみ使用可
		,{VAL_TSCM_TENANT_ID							,"tscm_tenant_id"					,GET_SET_ENABLE}				// get_hciのみ使用可
		,{VAL_TSCM_SERIAL								,"tscm_serial"						,GET_SET_ENABLE}				// get_hciのみ使用可
		,{VAL_VPN_SERVER_IP								,"vpn_server_ip"					,GET_SET_ENABLE}				// VPN Server IP
		,{VAL_VPN_GROUP_NAME							,"vpn_group_name"					,GET_SET_ENABLE}				// VPN Group Name
		,{VAL_VPN_SECRET_KEY							,"vpn_secret_key"					,GET_SET_ENABLE}				// VPN Secret Key
		,{VAL_VPN_DEFAULT_USER_NAME						,"vpn_df_uname"						,GET_SET_ENABLE}				// VPN Default User Name
		,{VAL_VPN_DEFAULT_USER_PASSWORD					,"vpn_df_pass"						,GET_SET_ENABLE}				// VPN Default User Password
		,{VAL_VPN_ONE_TIME_PASSWORD						,"vpn_one_time_pass"				,GET_SET_ENABLE}				// VPN One-Time Password
		,{VAL_FTP_UNZIP_PASSWORD						,"ftp_unzip_pass"					,GET_SET_ENABLE}				// FTP Unzip Password
		,{VAL_VPN_DOMAIN								,"vpn_domain"						,GET_SET_ENABLE}				// VPN Domain
		,{VAL_SATA_DEV_EXISTENCE						,"sata_dev_existence"				,GET_SET_ENABLE}				// SATA Device Existence
		,{VAL_BOOT_KEY_INFO								,"boot_key_info"					,GET_SET_ENABLE}				// Boot Key Information
		,{VAL_MANUFACT_MODE								,"manufact_mode"					,GET_SET_ENABLE}				// Manufacturing Mode
		,{VAL_CUSTOMER_ID								,"customer_id"						,GET_SET_ENABLE}				// Customer ID
		,{VAL_DOWNLOAD_PROTOCOL							,"download_protocol"				,GET_SET_ENABLE}				// Download Protocol
		,{VAL_HOST_NAME									,"host_name"						,GET_SET_ENABLE}				// Host Name
		,{VAL_SECOND_TSCM_URL							,"sec_tscm_url"						,GET_SET_ENABLE}				// Secondary TSCM(TZCS) Server Address
		,{VAL_SECOND_DOWNLOAD_SERVER_IP					,"sec_dl_server_ip"					,GET_SET_ENABLE}				// Secondary Download Server Address
		,{VAL_SECOND_TSCM_COM_KEY						,"sec_tscm_com_key"					,GET_SET_ENABLE}				// Secondary TSCM(TZCS) com key    get_hciのみ使用可
		,{VAL_SECOND_TSCM_CKID							,"sec_tscm_ckid"					,GET_SET_ENABLE}				// Secondary TSCM(TZCS) ckid       get_hciのみ使用可
		,{VAL_SECOND_TSCM_TENANT_ID						,"sec_tscm_tenant_id"				,GET_SET_ENABLE}				// Secondary TSCM(TZCS) tenant id  get_hciのみ使用可
		,{VAL_LAN_LINK_STATUS							,"lan_link_status"					,GET_SET_ENABLE}				// LAN Link Status
		,{VAL_DHCP_WAIT_TIME							,"dhcp_wait_time"					,GET_SET_ENABLE}				// DHCP Response Timeout
//			,{VAL_BIOS_VERSION								,"bios_ver"							,GET_SET_ENABLE}			// Bios version
		,{VAL_EULA_ACCEPTANCE_STATE						,"eula_state"						,GET_SET_ENABLE}				// EULA Acceptance State
		,{VAL_CACHE_ENC_MAGIC_NUM 						,"cache_enc_magic"					,GET_SET_ENABLE}				// Cache Encryption Magic Number
		,{VAL_CACHE_ENC_KEY0 							,"cache_enc_key0"					,GET_SET_ENABLE}				// Cache Encryption Key 0
		,{VAL_CACHE_ENC_KEY1 							,"cache_enc_key1"					,GET_SET_ENABLE}				// Cache Encryption Key 1
		,{VAL_CACHE_ENC_KEY2 							,"cache_enc_key2"					,GET_SET_ENABLE}				// Cache Encryption Key 2
		,{VAL_CACHE_ENC_KEY3 							,"cache_enc_key3"					,GET_SET_ENABLE}				// Cache Encryption Key 3
		,{VAL_CACHE_DEC_MAGIC_NUM 						,"cache_dec_magic"					,GET_SET_ENABLE}				// Cache Decryption Magic Number
		,{VAL_CACHE_DEC_KEY0 							,"cache_dec_key0"					,GET_SET_ENABLE}				// Cache Decryption Key 0
		,{VAL_CACHE_DEC_KEY1 							,"cache_dec_key1"					,GET_SET_ENABLE}				// Cache Decryption Key 1
		,{VAL_CACHE_DEC_KEY2 							,"cache_dec_key2"					,GET_SET_ENABLE}				// Cache Decryption Key 2
		,{VAL_CACHE_DEC_KEY3 							,"cache_dec_key3"					,GET_SET_ENABLE}				// Cache Decryption Key 3
		,{VAL_NETWORK_MODE 								,"net_mode"							,GET_SET_ENABLE}				// Network Mode
		,{VAL_TZCS_INI_PACK_URL							,"ini_pack_url"						,GET_SET_ENABLE}				// TZCS Initial Package URL
		,{VAL_TZCS_INI_PACK_PASS						,"ini_pack_pass"					,GET_SET_ENABLE}				// TZCS Initial Package Password
		,{VAL_SD_CACHE_FUNC								,"sd_cache_func"					,GET_SET_ENABLE}				// SD Cache Function 0: OFF (デフォルト) 1: ON
		,{VAL_TZCS_INI_PACK_NAME						,"ini_pack_name"					,GET_SET_ENABLE}				// TZCS Initial Package Name
		,{VAL_DHCP_WAIT_TIME_WLAN						,"dhcp_wait_time_wlan"				,GET_SET_ENABLE}				// DHCP Response Timeout (WLAN)
		,{VAL_LANGUAGE		 							,"language"							,GET_SET_ENABLE}				// Language 0: <English>（デフォルト）1: <Japanese>
		,{VAL_BIOS_VER									,"bios_ver"							,GET_SET_ENABLE}				// BIOS Version
		,{VAL_BIOS_ID			 						,"bios_id"							,GET_SET_ENABLE}				// BIOS-ID 戻り値(テキストで16進数4桁)
       	,{VAL_EC_VER									,"ec_ver"							,GET_SET_ENABLE}				// EC Version
		,{VAL_PROXY_INFO_SPEC_VER						,"proxy_spec_ver"					,GET_SET_ENABLE}				// Proxy Information Spec Version
		,{VAL_PROXY_SETTING								,"proxy_setting"					,GET_SET_ENABLE}				// Proxy Settings 0: OFF 1: ON
		,{VAL_PROXY_URL		 							,"proxy_url"						,GET_SET_ENABLE}				// Proxy Server URL
		,{VAL_PROXY_PORT	 							,"proxy_port"						,GET_SET_ENABLE}				// Proxy Port
		,{VAL_PROXY_AUTH_SETTING						,"proxy_auth_setting"				,GET_SET_ENABLE}				// Proxy Authentication 0: OFF 1: ON
		,{VAL_PROXY_AUTH_UNAME							,"proxy_auth_uname"					,GET_SET_ENABLE}				// Proxy Authentication User Name
		,{VAL_PROXY_AUTH_PASS							,"proxy_auth_pass"					,GET_SET_ENABLE}				// Proxy Authentication Password
		,{VAL_POLICY_VER	 							,"policy_ver"						,GET_SET_ENABLE}				// Policy version
		,{VAL_RECONNECT_RETRY_CNT						,"rc_retry_cnt"						,GET_SET_ENABLE}				// Retry count for re-connection
		,{VAL_RECONNECT_INTERVAL_LAN					,"rc_interval_lan"					,GET_SET_ENABLE}				// Wired LAN retry interval for re-connection
		,{VAL_RECONNECT_INTERVAL_WLAN					,"rc_interval_wlan"					,GET_SET_ENABLE}				// Wireless LAN retry interval for re-connection
		,{VAL_POLLING_INTERVAL							,"polling_interval"					,GET_SET_ENABLE}				// Polling interval of the authentication server
		,{VAL_VDI_PROFILE 								,"vdi_profile"						,GET_SET_ENABLE}				// VDI profile 0:Vmware 1:Citrix
		,{VAL_VDI_URL			 						,"vdi_url1"							,GET_SET_ENABLE}				// VDI connection URL
		,{VAL_WIFI_AP_WAIT_TIME							,"wifi_ap_wait_time"				,GET_SET_ENABLE}				// Wifi AP Connect Wait Timer
// -add [S-001](ZCS (1.1)) 2016/01/12 Tanaka
		,{VAL_WEB_AUTH											,"web_auth"							,GET_SET_ENABLE}			// web authentication setting
		,{VAL_WEB_AUTH_TIME									,"web_auth_time"				,GET_SET_ENABLE}			// web authentication timeout period
// -end [S-001](ZCS (1.1))
// -add [S-002](ZCS (1.1)) 2016/01/12 Tanaka
		,{VAL_BIG_IP_INFO_NO								,"big_ip_info_no"				,GET_SET_ENABLE}			// BIG IP（VPN） #n information
		,{VAL_BIG_IP_NAME										,"big_ip_name"					,GET_SET_ENABLE}			// BIG IP（VPN） #n name
		,{VAL_BIG_IP_URL										,"big_ip_url"						,GET_SET_ENABLE}			// BIG IP（VPN） #n url
// -end [S-002](ZCS (1.1))
///// -Add [S-005](ZCS (Phase2-azusa2)) 2015/06/20 Shimizu
		,{VAL_ERROR_LOG_RECORD							,"err_log_record"					,GET_SET_ENABLE}				// Error Log Recording ※現時点では読み込み値は未変換
///// -End [S-005](ZCS (Phase2-azusa2))
///// -Add [[S-xx3](ZCS (Phase2-base)) 2015/08/17 Tanaka
		,{VAL_WATCHDOG_T_START							,"watchdog_t_start"					,GET_SET_ENABLE}				// Watchdog Timer Star
		,{VAL_WATCHDOG_T_STOP							  ,"watchdog_t_stop"					,GET_SET_ENABLE}				// Watchdog Timer Stop
///// -End [[S-xx3](ZCS (Phase2-base)) 
		,{VAL_WIFI_INFO_SPEC_VER						,"wifi_spec_ver"					,GET_SET_ENABLE}				// WiFi AP Information Spec Version
		,{VAL_WIFI_AP_SSID_SET							,"wifi_ap_ssid_set"					,GET_SET_ENABLE}				// Latest WiFi AP SSID
		,{VAL_WIFI_AP_UNAME_SET							,"wifi_ap_uname_set"				,GET_SET_ENABLE}				// Latest WiFi AP UserName
		,{VAL_WIFI_AP_PASS_SET							,"wifi_ap_pass_set"					,GET_SET_ENABLE}				// Latest WiFi AP Password
		,{VAL_WIFI_AP_CHAN_SET							,"wifi_ap_chan_set"					,GET_SET_ENABLE}				// Latest WiFi AP Channel
		,{VAL_WIFI_AP_SECURITY_SET						,"wifi_ap_sec_set"					,GET_SET_ENABLE}				// Latest WiFi AP Security Type 0: WPA/WPA2-Enterprise 1: WPA/WPA2-Personal 2: WEP
		,{VAL_WIFI_DATA_LOADING							,"wifi_data_loading"				,GET_SET_ENABLE}				// Latest WiFi AP Data Loading
		,{VAL_WIFI_DATA_DEL								,"wifi_data_del"					,GET_SET_ENABLE}				// WiFi AP Data Deletion
		,{VAL_WIFI_PRIORITY_NO							,"wifi_priority_no"					,GET_SET_ENABLE}				// WiFi AP #n Priority
		,{VAL_WIFI_AP_SSID_DATA							,"wifi_ap_ssid_data"				,GET_SET_ENABLE}				// WiFi AP #n SSID
		,{VAL_WIFI_AP_UNAME_DATA 						,"wifi_ap_uname_data"				,GET_SET_ENABLE}				// WiFi AP #n UserName
		,{VAL_WIFI_AP_PASS_DATA 						,"wifi_ap_pass_data"				,GET_SET_ENABLE}				// WiFi AP #n Password
		,{VAL_WIFI_AP_CHAN_DATA 						,"wifi_ap_chan_data"				,GET_SET_ENABLE}				// WiFi AP #n Channel
		,{VAL_WIFI_AP_SECURITY_DATA						,"wifi_ap_sec_data"					,GET_SET_ENABLE}				// WiFi AP #n Security Type
        ,{VAL_KEYBLIGHT_MODE							,"keyblight_mode"					,GET_SET_ENABLE}				// Keyboard Backlight Control Mode
        ,{VAL_KEYBLIGHT_TIME							,"keyblight_time"					,GET_SET_ENABLE}				// Backlight Lighting Time

		,{VAL_HCI_END 									,"hci_end" 							,GET_SET_ENABLE}				// Hci End Code

	};


#endif
