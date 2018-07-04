#include <string.h>
#include <stdio.h>
//#include "tvalzctl.h"
#include "network.h"
#include "helper.h"
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>

NetworkInfo network_info_s;

/**  Declare function */
void  init_network(void);
void  get_network_bios_setting(void);
void  get_tls_infor(void);
void  get_ip_command_wired(void);
void  get_ip_command_wifi(void);

int   create_wired_wpa_config_file(void);
int   check_wifi_security_type(void);
int   wifi_authentication_processing(int retry_cnt);
int   create_wifi_wpa_config_file(void);

int   connect_network_wired(void);
int   set_ip_wired(void);
void  wired_connection_closed(void);
int   connect_network_wifi(void);
int   set_ip_wifi(void);
void  wifi_connection_closed(void);

int   switch_network(int network_mode_change);
void  update_network_signal(void);
int   retry_connect_network(void);

/** ------------------------------------------------------------------
 * <B>Function</B>: init_network
 *
 * Initializes all global variables for network
 *
 * @return      WIFI_MODE  : Wi-fi network
 *              WIRE_MODE  : Wired network
 *              NONE_NET   : No network
 *------------------------------------------------------------------*/
void init_network()
{
#ifdef DEBUG_NETWORK
    printf("Initialize network variables\n");
#endif
    int ret = NONE_NET;
    /* Reset all values of network struct */
    memset(&network_info_s, 0, sizeof(NetworkInfo));
    /* Get BIOS info */
    get_network_bios_setting();
    get_tls_infor();

    /* Create command */
    get_ip_command_wired();
    get_ip_command_wifi();
    create_wired_wpa_config_file();
    create_wifi_wpa_config_file();

    system(ifup_wired);

    /*
     * Initialize current network mode is wrong network mode
     * This mode will be re-correct after initializing GUI completely
     */
    if (!strcmp(trim(get_status(state_network_wired)), "up")) {
        network_info_s.current_network_mode = WIFI_MODE;
    } else {
        network_info_s.current_network_mode = WIRE_MODE;
    }
}

/** ------------------------------------------------------------------
 * <B>Function</B>: get_network_bios_setting
 *
 * Get the last AP from store list
 * Get network setting values from HCI
 *
 *------------------------------------------------------------------*/
void get_network_bios_setting()
{
    /* Get one of AP data from store list */
    memset(&network_info_s.ssid_info, 0, sizeof(wifiInfo_t));
    network_info_s.ssid_info = get_store_list_data("1");    // Indicator should specify No."1"

    /* Get network setting values from HCI */
    PointerWireSetting p_wire_setting = &network_info_s.wired_setting;
    memset(p_wire_setting, 0, sizeof(p_wire_setting));

    p_wire_setting->wired_static_set = get_network_setting(ETH);
    if (!strcmp(p_wire_setting->wired_static_set.dhcpsw, "0")) {
        p_wire_setting->is_static = 1;
        if (strlen(p_wire_setting->wired_static_set.ip)
            && strlen(p_wire_setting->wired_static_set.subnet)
            && strlen(p_wire_setting->wired_static_set.gateway)
            && strlen(p_wire_setting->wired_static_set.dns)) {
            p_wire_setting->is_static_input = 1;
        }
    }
    p_wire_setting->dhcp_wait_time = get_dhcp_retry_count(ETH);
    p_wire_setting->wired_net_auth_setting = get_wired_net_auth_val();

    PointerWifiSetting p_wifi_setting = &network_info_s.wifi_setting;
    memset(p_wifi_setting, 0, sizeof(p_wifi_setting));

    p_wifi_setting->wifi_static_set = get_network_setting(WLAN);
    if (!strcmp(p_wifi_setting->wifi_static_set.dhcpsw, "0")) {
        p_wifi_setting->is_static = 1;
        if (strlen(p_wifi_setting->wifi_static_set.ip)
            && strlen(p_wifi_setting->wifi_static_set.subnet)
            && strlen(p_wifi_setting->wifi_static_set.gateway)
            && strlen(p_wifi_setting->wifi_static_set.dns)) {
            p_wifi_setting->is_static_input = 1;
        }
    }
    p_wifi_setting->dhcp_wait_time = get_dhcp_retry_count(WLAN);
}

/** -------------------------------------------------------------------
 * <B>Function</B>: get_tls_infor
 *
 * Get EAP-TLS information includes: identifier,
 *                                   root certificate name,
 *                                   client certificate name,
 *                                   private key name,
 *                                   private key password
 *
 *------------------------------------------------------------------*/
void get_tls_infor()
{
    PointerTLSInfo p_tls_infor = &network_info_s.tls_info;
    memset(p_tls_infor, 0, sizeof(p_tls_infor));

    identity_setting(p_tls_infor->eap_identifier);
    root_certificate_setting(p_tls_infor->root_cert_name);
    client_certificate_setting(p_tls_infor->client_cert_name);
    private_key_setting(p_tls_infor->private_key_name);
    private_key_password_setting(p_tls_infor->private_key_pass);
}

/** -------------------------------------------------------------------
 * <B>Function</B>: get_ip_assign_command_eth0
 *
 * Create command to set IP address for Wired
 *
 *------------------------------------------------------------------*/
void get_ip_command_wired()
{
    PointerWireSetting p_network_setting = &network_info_s.wired_setting;
    memset(network_info_s.ip_command_wired, '\0', sizeof(network_info_s.ip_command_wired));

    if (p_network_setting->is_static && p_network_setting->is_static_input) {
//        sprintf(command, "for iface in $(ls /sys/class/net | grep -v ^lo); do ip addr flush dev $iface; done\n");
//        sprintf(command, "while route del default gw 0.0.0.0 dev wlan0 2>/dev/null; do echo \":\" >/dev/null; done\n");
        sprintf(network_info_s.ip_command_wired, "ip addr flush dev eth0\n");
        sprintf(network_info_s.ip_command_wired, "%s ifconfig eth0 inet up %s netmask %s\n",
                network_info_s.ip_command_wired, p_network_setting->wired_static_set.ip, p_network_setting->wired_static_set.subnet);
        sprintf(network_info_s.ip_command_wired, "%s route add -net default gw %s dev eth0\n",
                network_info_s.ip_command_wired, p_network_setting->wired_static_set.gateway);
        sprintf(network_info_s.ip_command_wired, "%s echo 'nameserver %s' > /etc/resolv.conf\n",
                network_info_s.ip_command_wired, p_network_setting->wired_static_set.dns);

#ifdef DEBUG_NETWORK
        printf("DEBUG Wired static command: %s", network_info_s.ip_command_wired);
#endif
    } else if (!p_network_setting->is_static) {
        set_udhcpc_cmd_eth0(network_info_s.ip_command_wired, p_network_setting->dhcp_wait_time, INDICATOR);
    }
}

/** -------------------------------------------------------------------
 * <B>Function</B>: get_ip_assign_command_wlan0
 *
 * Create command to set IP address for Wi-Fi
 *
 *------------------------------------------------------------------*/
void get_ip_command_wifi()
{
    PointerWifiSetting p_network_setting = &network_info_s.wifi_setting;
    memset(network_info_s.ip_command_wifi, '\0', sizeof(network_info_s.ip_command_wifi));

    if (p_network_setting->is_static && p_network_setting->is_static_input) {
//        sprintf(command, "for iface in $(ls /sys/class/net | grep -v ^lo); do ip addr flush dev $iface; done\n");
//        sprintf(command, "while route del default gw 0.0.0.0 dev eth0 2>/dev/null; do echo \":\" >/dev/null; done\n");
        sprintf(network_info_s.ip_command_wifi, "ip addr flush dev wlan0\n");
        sprintf(network_info_s.ip_command_wifi, "%s ifconfig wlan0 inet up %s netmask %s\n",
                network_info_s.ip_command_wifi, p_network_setting->wifi_static_set.ip, p_network_setting->wifi_static_set.subnet);
        sprintf(network_info_s.ip_command_wifi, "%s route add -net default gw %s dev wlan0\n",
                network_info_s.ip_command_wifi, p_network_setting->wifi_static_set.gateway);
        sprintf(network_info_s.ip_command_wifi, "%s echo 'nameserver %s' > /etc/resolv.conf\n",
                network_info_s.ip_command_wifi, p_network_setting->wifi_static_set.dns);

#ifdef DEBUG_NETWORK
        printf("DEBUG Wi-fi static command: %s", network_info_s.ip_command_wifi);
#endif
    } else if (!p_network_setting->is_static) {
        set_udhcpc_cmd_wlan0(network_info_s.ip_command_wifi, p_network_setting->dhcp_wait_time, INDICATOR);
    }
}

/** ------------------------------------------------------------------
 * <B>Function</B>: create_wired_wpa_config_file
 *
 * Wired Network Authentication check and save result into member variable
 * @note      WIRED_NET_AUTH_NON      : nothing
 * @note      WIRED_NET_AUTH_EAP_TLS  : EAP-TLS
 * 
 *------------------------------------------------------------------*/
int create_wired_wpa_config_file()
{
    network_info_s.wired_setting.create_config_ret = CREATE_NG;
    if (network_info_s.wired_setting.wired_net_auth_setting == WIRED_NET_AUTH_EAP_TLS) {
        // Create EAP-TLS configuration file for wpa_supplicant
        network_info_s.wired_setting.create_config_ret = eap_tls_config_file_create(DRIVER_WIRED, NULL);
        if (network_info_s.wired_setting.create_config_ret != CREATE_OK) {
            system("rm wired.conf > /dev/null 2>&1");
            system("dd if=/dev/zero of=/dev/fb0 > /dev/null 2>&1");
        }
    }
    return network_info_s.wired_setting.create_config_ret;
}

/** ------------------------------------------------------------------
 * <B>Function</B>: check_wifi_security_type
 *
 * Check Wi-Fi security type
 * @return        wifi security type
 *
 *------------------------------------------------------------------*/
int check_wifi_security_type()
{
    if (strlen(network_info_s.ssid_info.ssid)) {
        if (!strlen(network_info_s.ssid_info.password) && strcmp(network_info_s.ssid_info.security, "3")) {
#ifdef DEBUG_NETWORK
            printf("WEP with No password: WEP_NON_SECURITY\n");
#endif
            network_info_s.wifi_setting.security_type = WEP_NON_SECURITY;
        } else if (!strcmp(network_info_s.ssid_info.security, "0")) {
#ifdef DEBUG_NETWORK
            printf("WPA-ENTERPRISE PEAP (Username and password)\n");
#endif
            network_info_s.wifi_setting.security_type = WPA_ENTERPRISE_PEAP;
        } else if (!strcmp(network_info_s.ssid_info.security, "3")) {
#ifdef DEBUG_NETWORK
            printf("WPA-ENTERPRISE EAP-TLS (Using certificates)\n");
#endif
            network_info_s.wifi_setting.security_type = WPA_ENTERPRISE_EAP_TLS;
        } else if(!strcmp(network_info_s.ssid_info.security, "1")) {
#ifdef DEBUG_NETWORK
            printf("WPA-PERSONAL (Only password)\n");
#endif
            network_info_s.wifi_setting.security_type = WPA_PERSONAL;
        } else {
#ifdef DEBUG_NETWORK
            printf("WEP with Password\n");
#endif
            network_info_s.wifi_setting.security_type = WEP_PASSWORD;
        }
    } else {
        network_info_s.wifi_setting.security_type = ERROR_WIFI;
    }
    return network_info_s.wifi_setting.security_type;
}

/** ------------------------------------------------------------------
 * <B>Function</B>: wifi_authentication_processing
 *
 * Wi-Fi network authentication processing
 * @param     retry_cnt    : DHCP retry count
 * @return    authentication result
 *              AUTH_OK  : success
 *              AUTH_NG  : failed
 *
 *------------------------------------------------------------------*/
int wifi_authentication_processing(int retry_cnt)
{
#ifdef DEBUG_NETWORK
    printf("Enter to %d: %s:%s time %s\n", __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
    int auth_result = AUTH_NG;
    int auth_timeout = 0;
    auth_timeout = retry_cnt * 3;
    char string[DATA_SIZE];
    memset(string, '\0', sizeof(string));
    while(auth_timeout > 0) {
        get_info("cat /var/log/wifi.log | grep \"CTRL-EVENT-EAP-SUCCESS\" | grep \"completed\"", string);
        if (strlen(string) != 0) {
            auth_result = AUTH_OK;
            break;
        }
        auth_timeout = auth_timeout - 1;
        sleep(1);
    }
#ifdef DEBUG_NETWORK
    printf("Leave at %d: %s:%s time %s\n",  __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
    return(auth_result);
}

/** ------------------------------------------------------------------
 * <B>Function</B>: create_wifi_wpa_config_file
 *
 * Create twlan2.conf file from BIOS setting
 * Then create wpa_supplicant command based on wireless type and twlan2.conf
 * The wpa_supplicant command will be store to member variable
 *
 *------------------------------------------------------------------*/
int create_wifi_wpa_config_file()
{
    struct stat st;

#ifdef DEBUG_NETWORK
    printf("Enter to %d: %s:%s time %s\n",  __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
    network_info_s.wifi_setting.create_config_ret = CREATE_NG;
    char password_buff[STR_BUF_SIZE];
    memset(password_buff, '\0', sizeof(password_buff));

    system("mkdir -p /etc/wpa_supplicant\n");
    if (stat("log_get_mode", &st) == 0) {
      system("mkdir -p /var/log\n");
    }
    memset(network_info_s.wpa_command, '\0', sizeof(network_info_s.wpa_command));

    check_wifi_security_type();
    switch (network_info_s.wifi_setting.security_type) {
    case WEP_NON_SECURITY:
    case WEP_PASSWORD:
        network_info_s.wifi_setting.create_config_ret = wep_config_file_create(network_info_s.ssid_info.ssid, network_info_s.ssid_info.password);
        break;
    case WPA_ENTERPRISE_PEAP:
        network_info_s.wifi_setting.create_config_ret = peap_config_file_create(network_info_s.ssid_info.ssid,
            network_info_s.ssid_info.uname, network_info_s.ssid_info.password);
        break;
    case WPA_ENTERPRISE_EAP_TLS:
        network_info_s.wifi_setting.create_config_ret = eap_tls_config_file_create(DRIVER_WIRELESS, network_info_s.ssid_info.ssid);
        break;
    case WPA_PERSONAL:
        network_info_s.wifi_setting.create_config_ret = personal_config_file_create(network_info_s.ssid_info.ssid, network_info_s.ssid_info.password);
        break;
    case ERROR_WIFI:
        break;
    default:
        break;
    }
    if (network_info_s.wifi_setting.create_config_ret == CREATE_OK) {
        if (stat("log_get_mode", &st) == 0) {
            sprintf(network_info_s.wpa_command, "wpa_supplicant -i wlan0 -D nl80211 -c twlan2.conf -B -dddt -f \"/var/log/wifi.log\" > /dev/null 2>&1");
        } else {
            sprintf(network_info_s.wpa_command, "wpa_supplicant -i wlan0 -D nl80211 -c twlan2.conf -B -f \"/var/log/wifi.log\" > /dev/null 2>&1");
        }
    } else {
        system("rm twlan2.conf > /dev/null 2>&1");
        system("dd if=/dev/zero of=/dev/fb0 > /dev/null 2>&1");
    }
#ifdef DEBUG_NETWORK
    printf("Leave at %d: %s:%s time %s\n",  __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
    return network_info_s.wifi_setting.create_config_ret;
}

/** ------------------------------------------------------------------
 * <B>Function</B>: connect_network_eth0
 *
 * Connects to wired network
 *
 * @return        1: Connected and successful release an IP address
 *                0: Can not connect to Wired LAN (DHCP failure)
 *                2: Missing static IP address in BIOS
 *
 *------------------------------------------------------------------*/
int connect_network_wired()
{
#ifdef DEBUG_NETWORK
    printf("Enter to %d: %s:%s time %s\n",  __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
    int return_status = SUCCESS;
    int result = NO_ERR;
    int auth_result = AUTH_NG;

    if (network_info_s.wired_setting.is_static && !network_info_s.wired_setting.is_static_input) {
        return_status = FAILURE;
        network_info_s.error_code = NO_STATIC_INPUT;
    } else if (!strcmp(trim(get_status(state_network_wired)), "up")) {
            if (network_info_s.wired_setting.wired_net_auth_setting == WIRED_NET_AUTH_EAP_TLS
                && network_info_s.wired_setting.create_config_ret == CREATE_OK) {
                auth_result = wired_authentication_processing(network_info_s.wired_setting.dhcp_wait_time, "1", INDICATOR);
            }
            if (network_info_s.wired_setting.wired_net_auth_setting == WIRED_NET_AUTH_NON
                || auth_result == AUTH_OK) {
                result = set_ip_wired();
                if (result == NO_ERR) {
                    network_info_s.current_network_mode = WIRE_MODE;
//                    system("for k in `ps | awk '/'wlan0'/{print $1}'`; do kill $k 2>/dev/null; done\n");
                } else {
                    return_status = FAILURE;
                    network_info_s.error_code = result;
                }
            }
            if (network_info_s.wired_setting.wired_net_auth_setting == WIRED_NET_AUTH_EAP_TLS
                && auth_result == AUTH_NG) {
                network_info_s.error_code = AUTH_FAIL;
                return_status = FAILURE;
            }
    } else {
        return_status = FAILURE;
        network_info_s.error_code = NO_CABLE;
    }
#ifdef DEBUG_NETWORK
    printf("Leave to %d: %s:%s time %s\n",  __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
    return return_status;
}

/** ------------------------------------------------------------------
 * <B>Function</B>: set_ip_eth0
 *
 * Sets IP address for wired network
 *
 * @return        1: Connected and successful release an IP address
 *                0: Can not connect to Wired LAN (DHCP failure)
 *                2: Missing static IP address in BIOS
 *
 *------------------------------------------------------------------*/
int set_ip_wired()
{
#ifdef DEBUG_NETWORK
    printf("Enter to %d: %s:%s \n", __LINE__, __FUNCTION__, __FILE__);
#endif
    int error_code = NO_ERR;

    system(kill_udhcpc_wired);
    system(network_info_s.ip_command_wired);
    if (network_info_s.wired_setting.is_static) {
        int timer_request = network_info_s.wired_setting.dhcp_wait_time;
        while (!strcmp(trim(get_status(state_network_wired)), "down") && timer_request > 0) {
            sleep(WAITING_CONNECT_TIME);
            timer_request = timer_request - 1;
        }
    }
#ifdef DEBUG_NETWORK
    printf("cat /sys/class/net/eth0/operstate: %s\n", trim(get_status(state_network_wired)));
#endif // DEBUG
    if (!strcmp(trim(get_status(get_ip_wired)), "")) {
        error_code = DHCP_FAIL;
        wired_connection_closed();
    } else {
        wifi_connection_closed();
    }
#ifdef DEBUG_NETWORK
    printf("Leave to %d: %s:%s \n", __LINE__, __FUNCTION__, __FILE__);
#endif
    return error_code;
}

/**-------------------------------------------------------------------
 * <B>Function</B>: wired_connection_closed
 *
 * wired_connection_closed
 *
 * @param      nothing
 * @return     nothing
 * @note       nothing
 *
 *------------------------------------------------------------------*/
void wired_connection_closed(void)
{
#ifdef DEBUG_NETWORK
    printf("Enter to %d: %s:%s time %s\n", __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
    system(route_del_wired);
    system(kill_wpa_supp_wired);
//    system(kill_udhcpc_wired);
    system("rm -rf /var/run/wpa_supplicant/eth0 > /dev/null 2>&1");
    system("rm -rf /etc/wpa_supplicant/wired.log > /dev/null 2>&1");
#ifdef DEBUG_NETWORK
    printf("Leave to %d: %s:%s time %s\n", __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
}

/** ------------------------------------------------------------------
 * <B>Function</B>: connect_network_wlan0
 *
 * Connects to wireless network
 *
 * @return        1: Connected and successfully release an IP address
 *                0: Can not connect to AP (DHCP failure)
 *                2: No SSID
 *
 *------------------------------------------------------------------*/
int connect_network_wifi()
{
#ifdef DEBUG_NETWORK
    printf("Enter to %d: %s:%s time %s\n", __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
    int return_status = SUCCESS;
    int result = NO_ERR;
    int auth_result = AUTH_NG;

    if (network_info_s.wifi_setting.is_static && !network_info_s.wifi_setting.is_static_input) {
        return_status = FAILURE;
        network_info_s.error_code = NO_STATIC_INPUT;
    } else if (!strlen(network_info_s.ssid_info.ssid)) {
        return_status = FAILURE;
        network_info_s.error_code = NO_SSID;
    } else {
        system(ifup_wifi);
        if (!strcmp(trim(get_status(wpa_supplicant_id_wifi)), "")) {
            system(network_info_s.wpa_command);
        }
        if (network_info_s.wifi_setting.security_type == WPA_ENTERPRISE_EAP_TLS) {
            auth_result = wifi_authentication_processing(network_info_s.wifi_setting.dhcp_wait_time);
        }
        if ((network_info_s.wifi_setting.security_type != WPA_ENTERPRISE_EAP_TLS && network_info_s.wifi_setting.create_config_ret == CREATE_OK)
            || auth_result == AUTH_OK) {
            result = set_ip_wifi();
            if (result == NO_ERR) {
                network_info_s.current_network_mode = WIFI_MODE;
                update_network_signal();
            } else {
                return_status = FAILURE;
                network_info_s.error_code = result;
//                system(ifdown_wlan0);
//                system("for k in `ps | awk '/'wlan0'/{print $1}'`; do kill $k 2>/dev/null; done\n");
            }
        }
        if (network_info_s.wifi_setting.security_type == WPA_ENTERPRISE_EAP_TLS && auth_result == AUTH_NG) {
            network_info_s.error_code = AUTH_FAIL;
            return_status = FAILURE;
        }
    }
#ifdef DEBUG_NETWORK
    printf("Leave to %d: %s:%s time %s\n", __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
    return return_status;
}

/** ------------------------------------------------------------------
 * <B>Function</B>: set_ip_wlan0
 *
 * Sets IP for wireless network
 *
 * @return    1: Connected and successfully release an IP address
 *            0: Can not connect to AP (DHCP failure)
 *            2: No SSID
 *
 *------------------------------------------------------------------*/
int set_ip_wifi()
{
#ifdef DEBUG_NETWORK
    printf("Enter to %d: %s:%s time %s\n", __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
    int error_code = NO_ERR;
    system(kill_udhcpc_wifi);
    system(network_info_s.ip_command_wifi);
    if (network_info_s.wifi_setting.is_static) {
        int timer_request = network_info_s.wifi_setting.dhcp_wait_time;
        while (!strcmp(trim(get_status(state_network_wifi)), "down") && timer_request > 0) {
            sleep(WAITING_CONNECT_TIME);
            timer_request = timer_request - 1;
        }
    }
#ifdef DEBUG_NETWORK
    printf("cat /sys/class/net/wlan0/operstate: %s\n", trim(get_status(state_network_wifi)));
#endif // DEBUG
    if (!strcmp(trim(get_status(state_network_wifi)), "down")) {
        error_code = DHCP_FAIL;
        wifi_connection_closed();
    } else {
        wired_connection_closed();
    }
#ifdef DEBUG_NETWORK
    printf("Leave to %d: %s:%s time %s\n", __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
    return error_code;
}

/**-------------------------------------------------------------------
 * <B>Function</B>: wifi_connection_closed
 *
 * wifi_connection_closed
 *
 * @param      nothing
 * @return     nothing
 * @note       nothing
 *
 *------------------------------------------------------------------*/
void wifi_connection_closed(void)
{
    struct stat st;

#ifdef DEBUG_NETWORK
    printf("Enter to %d: %s:%s time %s\n", __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
    system(route_del_wifi);
    system(kill_wpa_supp_wifi);
    system(kill_udhcpc_wifi);
    system("rm -rf /var/run/wpa_supplicant/wlan0 > /dev/null 2>&1");
    if (stat("log_get_mode", &st) != 0) {
      system("rm -rf /var/log/wifi.log > /dev/null 2>&1");
    }
    system(ifdown_wifi);
#ifdef DEBUG_NETWORK
    printf("Leave to %d: %s:%s time %s\n", __LINE__, __FUNCTION__, __FILE__, __TIME__);
#endif
}


/** -------------------------------------------------------------------
 * <B>Function</B>:    switch_network
 * 
 * Switch current network to selected network
 *
 * @return  Return 1 if switch network successfully or error code if an error occurred.
 *
 * @param   network_mode_change
 *
 *------------------------------------------------------------------*/
int switch_network(int network_mode_change)
{
#ifdef DEBUG_NETWORK
    printf("Enter to %d: %s:%s \n", __LINE__, __FUNCTION__, __FILE__);
#endif
    int return_status = SUCCESS;
    char net_name[10];
    memset(net_name, '\0', sizeof(net_name));

    if (network_mode_change == WIRE_MODE && network_info_s.current_network_mode != WIRE_MODE) {
        return_status = connect_network_wired();
        sprintf(net_name, "%s", "Wired");
    } else if (network_mode_change == WIFI_MODE && network_info_s.current_network_mode != WIFI_MODE) {
        return_status = connect_network_wifi();
        sprintf(net_name, "%s", "Wi-Fi");
    }
    /* Handle the error */
    if (return_status == FAILURE) {
        char message[CMD_BUF_SIZE];
        memset(message,  '\0', sizeof(message));
        switch (network_info_s.error_code) {
        case DHCP_FAIL:
        case AUTH_FAIL:
            if (!strcmp(net_name, "Wi-Fi")) {
                sprintf(message, "gtkpopup \"Failed to connect to %s network\n            SSID: %s\" &", net_name, network_info_s.ssid_info.ssid);
            } else {
                sprintf(message, "gtkpopup \"Failed to connect to %s network\n\" &", net_name);
            }
            break;
        case NO_SSID:
        case NO_CABLE:
            sprintf(message, "gtkpopup \"Failed to connect to %s network\n\" &", net_name);
            break;
        case NO_STATIC_INPUT:
            sprintf(message, "gtkpopup \"Please input Static IP Address using\n         BIOS Setup Utility\" &");
            break;
        default:
            perror("Error handle!");
            break;
        }
        system(message);
    } else {
        network_info_s.current_network_mode = network_mode_change;
    }
#ifdef DEBUG_NETWORK
    printf("Leave to %d: %s:%s \n", __LINE__, __FUNCTION__, __FILE__);
#endif
    return return_status;
}

/** -------------------------------------------------------------------
 * <B>Function</B>:    update_network_signal
 * 
 * Update wifi signal level
 *
 *------------------------------------------------------------------*/
void update_network_signal()
{
    network_info_s.wifi_setting.wifi_index = (get_info_val(signal_wifi)*100) / WIFI_SIGNAL_MAX;
}

/** ------------------------------------------------------------------
 * <B>Function</B>: retry_connect_network
 *
 * When it detects the state of network is changed (up -> down), it tries to connect other network
 *
 *------------------------------------------------------------------*/
int retry_connect_network()
{
#ifdef DEBUG_NETWORK
    printf("Enter to %d: %s:%s \n", __LINE__, __FUNCTION__, __FILE__);
#endif
    int connect_result = FAILURE;
    if (/*!strcmp(trim(get_status(state_network_wifi)), "down") && */network_info_s.current_network_mode == WIFI_MODE) {
        if (!strcmp(trim(get_status(state_network_wifi)), "down")) {
            connect_result = connect_network_wired();
        } else {
            connect_result = connect_network_wifi();
        }
        // ome-20161219 modify-s
        get_client_status_kill();
        // ome-20161219 modify-e
    }
    if (/*!strcmp(trim(get_status(state_network_wired)), "down") && */network_info_s.current_network_mode == WIRE_MODE) {
        if (!strcmp(trim(get_status(state_network_wired)), "down")) {
            connect_result = connect_network_wifi();
        } else {
            connect_result = connect_network_wired();
        }
        // ome-20161219 modify-s
        get_client_status_kill();
        // ome-20161219 modify-e
    }
#ifdef DEBUG_NETWORK
    printf("Leave to %d: %s:%s \n", __LINE__, __FUNCTION__, __FILE__);
#endif
    return connect_result;
}
