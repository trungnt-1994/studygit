#ifndef _NETWORK_
#define _NETWORK_

#include <tzcslib/TzcsNetwork.h>

/** Debug mode */
//#define DEBUG_NETWORK           1

/**  Buffer size of command */
#define CMD_BUF_SIZE           1024
#define DATA_SIZE              256

/**  System command definition */
#define WIFI_SIGNAL_MAX         70

/**  Retry connect */
#define TRY_CONNECT_NUMBER      3
#define RECONNECT_INTERVAL      3
#define WAITING_CONNECT_TIME    3

/** Get network Wi-fi */
static const char *state_network_wifi     = "cat /sys/class/net/wlan0/operstate";
static const char *signal_wifi            = "cat /proc/net/wireless | awk '{if(NR==3){print $3}}'";

/** Get network wired */
static const char *state_network_wired    = "cat /sys/class/net/eth0/operstate";
static const char *get_ip_wired           = "ifconfig eth0 | grep 'inet addr'";

/**  Up/down network interface */
static const char *ifup_wired             = "ifconfig eth0 up";
static const char *ifdown_wired           = "ifconfig eth0 down";
static const char *ifup_wifi              = "rfkill unblock wifi\n ifconfig wlan0 up";
static const char *ifdown_wifi            = "rfkill block wifi";

/**  Change default route */
static const char *route_del_wired        = "ip route flush dev eth0";
static const char *route_del_wifi         = "ip route flush dev wlan0";

/**  Kill udhcpc */
static const char *kill_udhcpc_wired       = "for k in `ps | grep -v 'sh -c' | awk '/'udhcpc'/' | awk '/'eth0'/{print $1}'`; do kill $k 2>/dev/null; done\n";
static const char *kill_udhcpc_wifi        = "for k in `ps | grep -v 'sh -c' | awk '/'udhcpc'/' | awk '/'wlan0'/{print $1}'`; do kill $k 2>/dev/null; done\n";

/**  Kill wpa_supplicant */
static const char *kill_wpa_supp_wired     = "for k in `ps | grep -v 'sh -c' | awk '/'wpa_supplicant'/' | awk '/'eth0'/'`; do kill $k 2>/dev/null; done\n";
static const char *kill_wpa_supp_wifi      = "for k in `ps | grep -v 'sh -c' | awk '/'wpa_supplicant'/' | awk '/'wlan0'/'`; do kill $k 2>/dev/null; done\n";

/**  Find wpa_supplicant of network device */
static const char *wpa_supplicant_id_wired = "ps | grep -v 'sh -c' | awk '/'wpa_supplicant'/' | awk '/'eth0'/'";
static const char *wpa_supplicant_id_wifi  = "ps | grep -v 'sh -c' | awk '/'wpa_supplicant'/' | awk '/'wlan0'/'";

/**  Network mode */
enum Network_mode {
    WIRE_MODE,  // eth0
    WIFI_MODE,  // wlan0
    NONE_NET
};

/**  Network Error code */
enum Network_error {
    NO_ERR,
    DHCP_FAIL,
    NO_SSID,
    AUTH_FAIL,
    NO_STATIC_INPUT,
	NO_CABLE
};

/**  Network status */
enum Network_connect {
    FAILURE,
    SUCCESS
};

/** Wi-Fi security type */
enum Wifi_security_type {
    WEP_NON_SECURITY,
    WPA_ENTERPRISE_PEAP,
    WPA_ENTERPRISE_EAP_TLS,
    WPA_PERSONAL,
    WEP_PASSWORD,
    ERROR_WIFI
};

/** Struct for LAN setting in BIOS*/
typedef struct _WireSettingStructure {
    network_set_t       wired_static_set;
    int                 dhcp_wait_time;
    int                 is_static;
    int                 is_static_input;
    unsigned int        wired_net_auth_setting;
    unsigned int        create_config_ret;
} WireSetting, *PointerWireSetting;

/** Struct for Wifi setting in BIOS*/
typedef struct _WifiSettingStructure {
    network_set_t       wifi_static_set;
    unsigned int        preferred_band;
    int                 dhcp_wait_time;
    int                 wifi_index;
    int                 is_static;
    int                 is_static_input;
    int                 security_type;
    unsigned int        create_config_ret;
} WifiSetting, *PointerWifiSetting;

/** Struct for TLS information*/
typedef struct _TLSInfoStructure {
    char                eap_identifier[DATA_SIZE];
    char                root_cert_name[DATA_SIZE];
    char                client_cert_name[DATA_SIZE];
    char                private_key_name[DATA_SIZE];
    char                private_key_pass[DATA_SIZE];
} TLSInfo, *PointerTLSInfo;

/** Struct network*/
typedef struct _NetworkInfoStructure{
    WireSetting         wired_setting;
    WifiSetting         wifi_setting;
    wifiInfo_t          ssid_info;
    TLSInfo             tls_info;
    enum Network_mode   current_network_mode;
    int                 is_switching;
    int                 is_recovering;
    int                 error_code;
    char                ip_command_wired[CMD_BUF_SIZE];
    char                ip_command_wifi[CMD_BUF_SIZE];
    char                wpa_command[CMD_BUF_SIZE];
} NetworkInfo, *PointerNetworkInfo;


/**  Global variable definition */
extern NetworkInfo network_info_s;

/**  Public functions */
void   init_network(void);
void   update_network_signal(void);
int    switch_network(int network_mode_change);
int    retry_connect_network(void);

#endif
