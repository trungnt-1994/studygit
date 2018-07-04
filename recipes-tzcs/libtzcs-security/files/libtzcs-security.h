/**
 *  @file     libtzcs-security.h
 *
 *  @brief    Header file for Utility functions for Test Client.
 *
 *  @note     Phase 2
 *
 *
 */
/*==================================================================*/

#if !defined(__TZCS_SECURITY_H__)
#define __TZCS_SECURITY_H__


/*--------------------------------------------------------------------
 * Define
 *------------------------------------------------------------------*/
// wifi_tls_file
// result
#define TLS_FILE_OK 0
#define TLS_FILE_NG 1

// Operation type
#define TLS_FILE_GET 0
#define TLS_FILE_PUT 1

// File type
#define TLS_FILE_ROOT_CERT   "0"
#define TLS_FILE_CLIENT_CERT "1"
#define TLS_FILE_PRIVATE_KEY "2"


/*--------------------------------------------------------------------
 * Prototypes
 *------------------------------------------------------------------*/

int debug_mode_data_decoding(char *in_data ,char *out_data);
int wifi_tls_file(int ope_type ,char *file_no ,char *file_name);




#endif /* __TZCS_SECURITY_H__ */
